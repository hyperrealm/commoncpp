/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2011  Mark A Lindner

   This file is part of commonc++.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
   ---------------------------------------------------------------------------
 */

#include "stacktrace.h"

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#ifdef HAVE_UCONTEXT_H
#include <ucontext.h>
#endif

#ifdef HAVE_EXECINFO_H
#include <execinfo.h>
#endif

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif

#ifdef HAVE_BFD_H
#include <bfd.h>
#endif

#ifdef __GNUC__
#include <cxxabi.h>

using namespace __cxxabiv1;
#endif

#include <string.h>
#include <stdlib.h>


// if we have getcontext(), use that
// else if we're __GNUC__, use backtrace()
// else fail

// if we have BFD, use that
// else use backtrace_symbols()

static const char *__stacktrace_target = TARGET;

static const char *__stacktrace_not_avail = "(Stack trace not available.)\n";

/*
 */

#ifdef HAVE_DLADDR

#if defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H)

typedef struct file_symbol_info
{
  bfd *bfdd;
  const char *filename;
  asymbol **symbols;
  struct file_symbol_info *next;
} file_symbol_info;

/*
 */

typedef struct stacktrace_context
{
  file_symbol_info *symtable;
  file_symbol_info *current_syms;
  bfd_vma pc;
  const char *filename; // source file name
  const char *function; // function name
  unsigned int line; // source line number
  int flags;
  bfd_boolean found;
} stacktrace_context;

/*
 */

static file_symbol_info *__stacktrace_lookup_symbols(
  stacktrace_context *context, const char *filename)
{
  file_symbol_info *entry = NULL;
  bfd *bfdd;
  char **matching;

  // return the entry for the file, if it's in our list

  for(entry = context->symtable; entry; entry = entry->next)
  {
    if(!strcmp(entry->filename, filename))
      return(entry);
  }

  // not found; try to open the file, read the symbol table, and create a new
  // entry in our list

  bfdd = bfd_openr(filename, __stacktrace_target);
  if(! bfdd)
  {
    // error;
    return(NULL);
  }

  if(bfd_check_format(bfdd, bfd_archive))
  {
    // error; cannot get addresses from archive
    return(NULL);
  }

  if(! bfd_check_format_matches(bfdd, bfd_object, &matching))
  {
    // error; not a matching format
    return(NULL);
  }

  entry = (file_symbol_info *)malloc(sizeof(file_symbol_info));
  memset(entry, 0, sizeof(file_symbol_info));

  entry->bfdd = bfdd;
  entry->filename = filename;

  // load the symbols

  if((bfd_get_file_flags(bfdd) & HAS_SYMS) != 0)
  {
    long symcount;
    unsigned int size;

    symcount = bfd_read_minisymbols(bfdd, FALSE, (void **)&(entry->symbols),
                                    &size);
    if(symcount == 0)
      symcount = bfd_read_minisymbols(bfdd, TRUE /* dynamic */,
                                      (void **)&(entry->symbols), &size);

    if(symcount < 0)
    {
      // bfd_fatal(bfd_get_filename (bfdd));
      entry->symbols = NULL; // no symbols found
    }
  }

  // chain the new entry onto the list

  entry->next = context->symtable;
  context->symtable = entry;

  return(entry);
}

/*
 */

static void __stacktrace_destroy_context(stacktrace_context *context)
{
  file_symbol_info *entry = context->symtable;

  while(entry)
  {
    file_symbol_info *next_entry = entry->next;

    free(entry->symbols);
    bfd_close(entry->bfdd);

    free(entry);
    entry = next_entry;
  }
}

/*
 */

static char *__stacktrace_demangle(bfd *bfdd, const char *mangled_name)
{
#ifdef HAVE_GCC_ABI_DEMANGLE

  char *result = NULL;
  int status;
  size_t length = 0;

  result = __cxa_demangle(mangled_name, result, &length, &status);

  if(status == 0)
    return(result);
  else if(status == -2)
  {
    char *buf = (char *)malloc(strlen(mangled_name) + 3);
    strcpy(buf, mangled_name);
    strcat(buf, "()");
    return(buf);
  }
  else
    return(NULL);

#else // ! HAVE_GCC_ABI_DEMANGLE

  {
    char *buf = (char *)malloc(strlen(mangled_name) + 3);
    strcpy(buf, mangled_name);
    strcat(buf, "()");
    return(buf);
  }

#endif // HAVE_GCC_ABI_DEMANGLE
}

/*
 */

static void __stacktrace_find_address(bfd *bfdd, asection *section, void *data)
{
  bfd_vma vma;
  bfd_size_type size;
  stacktrace_context *context = (stacktrace_context *)data;

  if(context->found)
    return;

  if((bfd_get_section_flags(bfdd, section) & SEC_ALLOC) == 0)
    return;

  vma = bfd_get_section_vma(bfdd, section);
  if(context->pc < vma)
    return;

  size = bfd_get_section_size(section);
  if(context->pc >= (vma + size))
    return;

  context->found = bfd_find_nearest_line(bfdd, section,
                                         context->current_syms->symbols,
                                         (context->pc - vma),
                                         &(context->filename),
                                         &(context->function),
                                         &(context->line));
}

/*
 */

static int __stacktrace_print_frame(FILE *stream, void *frameaddr,
                                    int framenum, stacktrace_context *context)
{
  Dl_info dlinfo;

  if(!dladdr(frameaddr, &dlinfo))
    return(0); // failure

  // use bfd to obtain function name & line number, and __cxa_demangle() to
  // demangle the function name

  context->found = FALSE;
  context->current_syms = __stacktrace_lookup_symbols(context,
                                                      dlinfo.dli_fname);

  if(context->current_syms)
  {
    context->pc = ((bfd_vma)frameaddr - (bfd_vma)dlinfo.dli_fbase);

    bfd_map_over_sections(context->current_syms->bfdd,
                          __stacktrace_find_address, context);

    if(! context->found)
    {
      context->pc = (bfd_vma)frameaddr;

      bfd_map_over_sections(context->current_syms->bfdd,
                            __stacktrace_find_address, context);
    }
  }

  if(context->found)
  {
    do
    {
      const char *func_name;
      const char *file_name;
      char *alloc = NULL;

      func_name = context->function;
      if (func_name == NULL || *func_name == '\0')
        func_name = "???";
      else
      {
        alloc = __stacktrace_demangle(context->current_syms->bfdd, func_name);
        func_name = alloc;
      }

      file_name = context->filename;

      if((!(context->flags & STACKTRACE_FULL_PATHS)) && context->filename)
      {
        const char *p;

        p = strrchr(context->filename, '/');
        if(p)
          file_name = ++p;
      }

      if(file_name)
        fprintf(stream, "% 3d: %s(%d): %s\n", framenum, file_name,
                context->line, func_name ? func_name : "???");
      else
        fprintf(stream, "% 3d: ???: %s\n", framenum,
                func_name ? func_name : "???");

      if(alloc)
        free(alloc);

      if(context->flags & STACKTRACE_INLINE_FUNCS)
      {
        context->found = bfd_find_inliner_info(context->current_syms->bfdd,
                                               &(context->filename),
                                               &(context->function),
                                               &(context->line));
      }
      else
        break;
    }
    while(context->found);
  }
  else
    fprintf(stream, "% 3d: ???\n", framenum);

  if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
    return(0); // finished

  return(1);
}

#else // ! defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H)

static int __stacktrace_print_frame(FILE *stream, void *frameaddr,
                                    int framenum)
{
  Dl_info dlinfo;

  if(!dladdr(frameaddr, &dlinfo))
    return(0); // failure

  fprintf(stream, "% 3d: %p <%s+%u> (%s)\n",
          framenum,
          frameaddr,
          dlinfo.dli_sname,
          (unsigned)((char *)frameaddr - (char *)dlinfo.dli_saddr),
          dlinfo.dli_fname);

  if(dlinfo.dli_sname && !strcmp(dlinfo.dli_sname, "main"))
    return(0); // finished
}

#endif // defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H)

#endif // HAVE_DLADDR

/*
 */

void stacktrace_init(void)
{
#if defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H)

  bfd_init();
  if(! bfd_set_default_target(__stacktrace_target))
  {
    fprintf(stderr, "Can't set BFD default target to %s: %s\n",
            __stacktrace_target, bfd_errmsg(bfd_get_error()));
  }

#endif
}

/*
 */

void stacktrace_print(FILE *stream,  unsigned int max_frames, int flags)
{
  size_t i;

#if defined(HAVE_BACKTRACE)

  {
    size_t sz;
    void **bt = NULL;

    bt = (void **)malloc(max_frames * sizeof(void *));
    if(! bt)
    {
      // error
      return;
    }

    sz = backtrace(bt, max_frames);

#if defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H) \
  && defined(HAVE_DLADDR)

    {
      int f = 0;
      stacktrace_context context;
      memset(&context, 0, sizeof(stacktrace_context));
      context.flags = flags;

      for(i = 0; i < sz; ++i)
      {
        if(! __stacktrace_print_frame(stream, bt[i], ++f, &context))
          break;
      }

      __stacktrace_destroy_context(&context);
    }

#else // !(defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H)
      // && defined(HAVE_DLADDR))

    {
      char **strings = backtrace_symbols(bt, sz);
      for(i = 0; i < sz; ++i)
      {
        fputs(strings[i], stream);
        fputc('\n', stream);
      }

      free(strings);
    }

#endif // defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H)
       // && defined(HAVE_DLADDR)

    if(bt)
      free(bt);
  }

#elif defined(HAVE_GETCONTEXT)

#if defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_DLADDR)

#if defined(REG_RIP)
# define __STACK_IA64
#elif defined(REG_EIP)
# define __STACK_X86
#else
# define __STACK_GENERIC
#endif

#if defined(__STACK_X86) || defined(__STACK_IA64)

  {
    ucontext_t ctx;
    stacktrace_context context;
    int f = 0;
    void **bp = 0;
    void *ip = 0;

    memset(&context, 0, sizeof(stacktrace_context));
    context.flags = flags;

    if(getcontext(&ctx) != 0)
    {
      return; // error
    }

#if defined(__STACK_IA64)
    ip = (void*)ctx.uc_mcontext.gregs[REG_RIP];
    bp = (void**)ctx.uc_mcontext.gregs[REG_RBP];
#elif defined(__STACK_X86)
    ip = (void*)ctx.uc_mcontext.gregs[REG_EIP];
    bp = (void**)ctx.uc_mcontext.gregs[REG_EBP];
#endif

    for(i = 0; bp && ip && (i < max_frames); ++i)
    {
      if(! __stacktrace_print_frame(stream, ip, ++f))
        break;

      ip = bp[1];
      bp = (void**)bp[0];
    }

    __stacktrace_destroy_context(&context);
  }

#else // !(defined(__STACK_X86) || defined(__STACK_IA64))

  fputs(__stacktrace_not_avail, stream);

#endif // defined(__STACK_X86) || defined(__STACK_IA64)

#else // !(defined(HAVE_LIBBFD) && defined(HAVE_BFD_H) && defined(HAVE_DLADDR))

  fputs(__stacktrace_not_avail, stream);

#endif // defined(USE_BFD) && defined(HAVE_LIBBFD) && defined(HAVE_BFD_H)
       // && defined(HAVE_DLADDR)

#else // neither HAVE_BACKTRACE nor HAVE_GETCONTEXT

  fputs(__stacktrace_not_avail, stream);

#endif // HAVE_BACKTRACE
}

/*
 */

void stacktrace_cleanup(void)
{
  // no-op
}
