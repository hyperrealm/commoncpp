/**
 *
 * This code is modified from the original StackWalker code available at
 * http://www.codeproject.com/threads/StackWalker.asp. That code does not
 * seem to be distributed under any kind of license.
 */

#include "stacktrace.h"

#include <windows.h>
#include <dbghelp.h>
#include <stdlib.h>
#include <psapi.h>

#define STACKWALK_MAX_NAMELEN 1024

#define USED_CONTEXT_FLAGS CONTEXT_FULL

typedef struct CallstackEntry
{
  DWORD64 offset; // if 0, we have no valid entry
  CHAR    name[STACKWALK_MAX_NAMELEN];
  CHAR    undName[STACKWALK_MAX_NAMELEN];
  CHAR    undFullName[STACKWALK_MAX_NAMELEN];
  DWORD64 offsetFromSmybol;
  DWORD   offsetFromLine;
  DWORD   lineNumber;
  CHAR    lineFileName[STACKWALK_MAX_NAMELEN];
  DWORD   symType;
  LPCSTR  symTypeString;
  CHAR    moduleName[STACKWALK_MAX_NAMELEN];
  DWORD64 baseOfImage;
  CHAR    loadedImageName[STACKWALK_MAX_NAMELEN];
} CallstackEntry;


#if defined(_M_IX86)
#ifdef CURRENT_THREAD_VIA_EXCEPTION
// TODO: The following is not a "good" implementation, 
// because the callstack is only valid in the "__except" block...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    EXCEPTION_POINTERS *pExp = NULL; \
    __try { \
      throw 0; \
    } __except( ( (pExp = GetExceptionInformation()) ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_EXECUTE_HANDLER)) {} \
    if (pExp != NULL) \
      memcpy(&c, pExp->ContextRecord, sizeof(CONTEXT)); \
      c.ContextFlags = contextFlags; \
  } while(0);
#else
// The following should be enough for walking the callstack...
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    __asm    call x \
    __asm x: pop eax \
    __asm    mov c.Eip, eax \
    __asm    mov c.Ebp, ebp \
    __asm    mov c.Esp, esp \
  } while(0);
#endif

#else

// The following is defined for x86 (XP and higher), x64 and IA64:
#define GET_CURRENT_CONTEXT(c, contextFlags) \
  do { \
    memset(&c, 0, sizeof(CONTEXT)); \
    c.ContextFlags = contextFlags; \
    RtlCaptureContext(&c); \
} while(0);
#endif

// ---------------------------------------------------------------------------

typedef enum CallstackEntryType { firstEntry, nextEntry, lastEntry };

// ---------------------------------------------------------------------------

static void __safe_strcpy(char *dst, const char *src, size_t maxlen)
{
  strncpy(dst, src, maxlen - 1);
  dst[maxlen - 1] = 0;
}

// ---------------------------------------------------------------------------

static BOOL stacktrace_get_module_info(HANDLE hProcess, DWORD64 baseAddr,
                                       IMAGEHLP_MODULE64 *pModuleInfo)
{
  void *pData = malloc(4096); // reserve enough memory, so the bug in
                              // v6.3.5.1 does not lead to
                              // memory-overwrites...

  pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
  if(pData == NULL)
  {
    SetLastError(ERROR_NOT_ENOUGH_MEMORY);
    return FALSE;
  }
  memcpy(pData, pModuleInfo, sizeof(IMAGEHLP_MODULE64));
 
  if(SymGetModuleInfo64(hProcess, baseAddr, (IMAGEHLP_MODULE64 *)pData)
     != FALSE)
  {
    // only copy as much memory as is reserved...
    memcpy(pModuleInfo, pData, sizeof(IMAGEHLP_MODULE64));
    pModuleInfo->SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
    free(pData);
    return TRUE;
  }
  
  free(pData);
  SetLastError(ERROR_DLL_INIT_FAILED);
  return FALSE;
}

// ---------------------------------------------------------------------------

static DWORD __stacktrace_load_module(HANDLE hProcess, LPCSTR img, LPCSTR mod,
                                      DWORD64 baseAddr, DWORD size)
{
  CHAR *szImg = _strdup(img);
  CHAR *szMod = _strdup(mod);
  DWORD result = ERROR_SUCCESS;
  ULONGLONG fileVersion = 0;
  IMAGEHLP_MODULE64 Module;
  const char *szSymType = "-unknown-";

  if((szImg == NULL) || (szMod == NULL))
    result = ERROR_NOT_ENOUGH_MEMORY;
  else
  {
    if(SymLoadModule64(hProcess, 0, szImg, szMod, baseAddr, size) == 0)
      result = GetLastError();
  }
  if(szImg != NULL)
  {
    // Retrive some additional-infos about the module
    if(stacktrace_get_module_info(hProcess, baseAddr, &Module) != FALSE)
    {
      switch(Module.SymType)
      {
        case SymNone:
          szSymType = "-nosymbols-";
          break;
        case SymCoff:
          szSymType = "COFF";
          break;
        case SymCv:
          szSymType = "CV";
          break;
        case SymPdb:
          szSymType = "PDB";
          break;
        case SymExport:
          szSymType = "-exported-";
          break;
        case SymDeferred:
          szSymType = "-deferred-";
          break;
        case SymSym:
          szSymType = "SYM";
          break;
        case 8: //SymVirtual:
          szSymType = "Virtual";
          break;
        case 9: // SymDia:
          szSymType = "DIA";
          break;
      }
    }
  }
  
  if(szImg != NULL)
    free(szImg);
  if(szMod != NULL)
    free(szMod);
  
  return result;
}

// ---------------------------------------------------------------------------

static BOOL __stacktrace_load_modules(HANDLE hProcess)
{
  DWORD i;
  DWORD cbNeeded;
  MODULEINFO mi;
  HMODULE *hMods = 0;
  char *tt = NULL;
  char *tt2 = NULL;
  const DWORD TTBUFLEN = 8096;
  int cnt = 0;
  DWORD dwRes;
  
  hMods = (HMODULE*)malloc(sizeof(HMODULE) * (TTBUFLEN / sizeof(HMODULE)));
  tt = (char*)malloc(sizeof(char) * TTBUFLEN);
  tt2 = (char*)malloc(sizeof(char) * TTBUFLEN);
  if((hMods == NULL) || (tt == NULL) || (tt2 == NULL))
    goto cleanup;
  
  if(! EnumProcessModules(hProcess, hMods, TTBUFLEN, &cbNeeded))
  {
    //_ftprintf(fLogFile, _T("%lu: EPM failed, GetLastError = %lu\n"), g_dwShowCount, gle );
    goto cleanup;
  }
  
  if(cbNeeded > TTBUFLEN)
  {
    //_ftprintf(fLogFile, _T("%lu: More than %lu module handles. Huh?\n"), g_dwShowCount, lenof( hMods ) );
    goto cleanup;
  }
  
  for(i = 0; i < (cbNeeded / sizeof(hMods[0])); ++i)
  {
    // base address, size
    GetModuleInformation(hProcess, hMods[i], &mi, sizeof(mi));
    // image file name
    tt[0] = 0;

    GetModuleFileNameExA(hProcess, hMods[i], tt, TTBUFLEN);
    // module name
    tt2[0] = 0;
    GetModuleBaseNameA(hProcess, hMods[i], tt2, TTBUFLEN);
    
    dwRes = __stacktrace_load_module(hProcess, tt, tt2,
                                     (DWORD64)mi.lpBaseOfDll,
                                     mi.SizeOfImage);
    if (dwRes != ERROR_SUCCESS)
    {
      // this->m_parent->OnDbgHelpErr("LoadModule", dwRes, 0);
    }
    cnt++;
  }
  
cleanup:

  if(tt2 != NULL)
    free(tt2);
  if(tt != NULL)
    free(tt);
  if(hMods != NULL)
    free(hMods);
  
  return(cnt != 0);
}                                

// ---------------------------------------------------------------------------

static BOOL __stdcall __stacktrace_read_proc_mem(HANDLE hProcess,
                                                 DWORD64 qwBaseAddress,
                                                 PVOID lpBuffer, DWORD nSize,
                                                 LPDWORD lpNumberOfBytesRead)
{
  DWORD st;
  BOOL bRet = ReadProcessMemory(hProcess, (LPVOID)qwBaseAddress, lpBuffer,
    nSize, &st);
  *lpNumberOfBytesRead = st;
  return(bRet);
}

// ---------------------------------------------------------------------------

static void __stacktrace_dump_callstack_entry(enum CallstackEntryType eType,
                                              CallstackEntry *entry,
                                              FILE *stream)
{
  if((eType != lastEntry) && (entry->offset != 0))
  {
    if(entry->name[0] == 0)
      __safe_strcpy(entry->name, "???", STACKWALK_MAX_NAMELEN);

    if(entry->undName[0] != 0)
      __safe_strcpy(entry->name, entry->undName, STACKWALK_MAX_NAMELEN);
    
    if(entry->undFullName[0] != 0)
      __safe_strcpy(entry->name, entry->undFullName, STACKWALK_MAX_NAMELEN);
    
    if(entry->lineFileName[0] == 0)
    {
      __safe_strcpy(entry->lineFileName, "???", STACKWALK_MAX_NAMELEN);
      if(entry->moduleName[0] == 0)
        __safe_strcpy(entry->moduleName, "(module-name not available)",
                      STACKWALK_MAX_NAMELEN);
//      fprintf(stream, "%p (%s): %s: %s()\n",
//              (LPVOID)entry->offset, entry->moduleName, entry->lineFileName,
//              entry->name);
      fprintf(stream, "%s: %s()\n", entry->lineFileName, entry->name);
    }
    else
    {
      char *file = entry->lineFileName;
      char *p = strrchr(file, '\\');
      if(p)
        file = ++p;
      fprintf(stream, "%s(%d): %s()\n", file, entry->lineNumber, entry->name);
    }
  }
}

// ---------------------------------------------------------------------------

static BOOL __stacktrace_walk_stack(FILE *stream, unsigned int max_frames,
                                    int flags)
{
  CONTEXT c;
  CallstackEntry csEntry;
  IMAGEHLP_SYMBOL64 *pSym = NULL;
  IMAGEHLP_MODULE64 Module;
  IMAGEHLP_LINE64 Line;
  unsigned int frameNum;
  HANDLE hThread = GetCurrentThread();
  HANDLE hProcess;
  DWORD imageType;
  enum CallstackEntryType et;
  STACKFRAME64 s; // in/out stackframe
  memset(&s, 0, sizeof(s));

  GET_CURRENT_CONTEXT(c, USED_CONTEXT_FLAGS);

  // init STACKFRAME for first call
#ifdef _M_IX86
  // normally, call ImageNtHeader() and use machine info from PE header
  imageType = IMAGE_FILE_MACHINE_I386;
  s.AddrPC.Offset = c.Eip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Ebp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.Esp;
  s.AddrStack.Mode = AddrModeFlat;
#elif _M_X64
  imageType = IMAGE_FILE_MACHINE_AMD64;
  s.AddrPC.Offset = c.Rip;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.Rsp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.Rsp;
  s.AddrStack.Mode = AddrModeFlat;
#elif _M_IA64
  imageType = IMAGE_FILE_MACHINE_IA64;
  s.AddrPC.Offset = c.StIIP;
  s.AddrPC.Mode = AddrModeFlat;
  s.AddrFrame.Offset = c.IntSp;
  s.AddrFrame.Mode = AddrModeFlat;
  s.AddrBStore.Offset = c.RsBSP;
  s.AddrBStore.Mode = AddrModeFlat;
  s.AddrStack.Offset = c.IntSp;
  s.AddrStack.Mode = AddrModeFlat;
#else
#error "Platform not supported!"
#endif

  pSym = (IMAGEHLP_SYMBOL64 *)malloc(sizeof(IMAGEHLP_SYMBOL64)
                                     + STACKWALK_MAX_NAMELEN);
  if (!pSym)
    goto cleanup;  // not enough memory...
  
  memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL64) + STACKWALK_MAX_NAMELEN);
  pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
  pSym->MaxNameLength = STACKWALK_MAX_NAMELEN;

  memset(&Line, 0, sizeof(Line));
  Line.SizeOfStruct = sizeof(Line);

  memset(&Module, 0, sizeof(Module));
  Module.SizeOfStruct = sizeof(Module);

  hProcess = GetCurrentProcess();
  
  for(frameNum = 0; frameNum < max_frames; ++frameNum)
  {
    fprintf(stream, "% 3d: ", frameNum + 1);

    // get next stack frame (StackWalk64(),
    // SymFunctionTableAccess64(), SymGetModuleBase64()) if this
    // returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998),
    // you can assume that either you are done, or that the stack is
    // so hosed that the next deeper frame could not be found.
    // CONTEXT need not to be suplied if imageTyp is
    // IMAGE_FILE_MACHINE_I386!

    if(! StackWalk64(imageType, hProcess, hThread, &s, &c,
                     __stacktrace_read_proc_mem, SymFunctionTableAccess64,
                     SymGetModuleBase64, NULL))

    
    {
//      this->OnDbgHelpErr("StackWalk64", GetLastError(), s.AddrPC.Offset);
      break;
    }

    csEntry.offset = s.AddrPC.Offset;
    csEntry.name[0] = 0;
    csEntry.undName[0] = 0;
    csEntry.undFullName[0] = 0;
    csEntry.offsetFromSmybol = 0;
    csEntry.offsetFromLine = 0;
    csEntry.lineFileName[0] = 0;
    csEntry.lineNumber = 0;
    csEntry.loadedImageName[0] = 0;
    csEntry.moduleName[0] = 0;
    if(s.AddrPC.Offset == s.AddrReturn.Offset)
    {
//      this->OnDbgHelpErr("StackWalk64-Endless-Callstack!", 0, s.AddrPC.Offset);
      break;
    }
    if(s.AddrPC.Offset != 0)
    {
      // we seem to have a valid PC
      // show procedure info (SymGetSymFromAddr64())
      if(SymGetSymFromAddr64(hProcess, s.AddrPC.Offset,
                             &(csEntry.offsetFromSmybol), pSym) != FALSE)
      {
        __safe_strcpy(csEntry.name, pSym->Name, STACKWALK_MAX_NAMELEN);
        UnDecorateSymbolName(pSym->Name, csEntry.undName,
                             STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY);
        UnDecorateSymbolName(pSym->Name, csEntry.undFullName,
                             STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE);
      }
      else
      {
//        this->OnDbgHelpErr("SymGetSymFromAddr64", GetLastError(),
//                           s.AddrPC.Offset);
      }

      // show line number info, NT5.0-method (SymGetLineFromAddr64())
      if(SymGetLineFromAddr64(hProcess, s.AddrPC.Offset,
                              &(csEntry.offsetFromLine), &Line) != FALSE)
      {
        csEntry.lineNumber = Line.LineNumber;
        __safe_strcpy(csEntry.lineFileName, Line.FileName,
                      STACKWALK_MAX_NAMELEN);
      }
      else
      {
//          this->OnDbgHelpErr("SymGetLineFromAddr64", GetLastError(), s.AddrPC.Offset);
      }
      
      // show module info (SymGetModuleInfo64())
      if(stacktrace_get_module_info(hProcess, s.AddrPC.Offset, &Module)
         != FALSE)
      { // got module info OK
        switch(Module.SymType)
        {
          case SymNone:
            csEntry.symTypeString = "-nosymbols-";
            break;
          case SymCoff:
            csEntry.symTypeString = "COFF";
            break;
          case SymCv:
            csEntry.symTypeString = "CV";
            break;
          case SymPdb:
            csEntry.symTypeString = "PDB";
            break;
          case SymExport:
            csEntry.symTypeString = "-exported-";
            break;
          case SymDeferred:
            csEntry.symTypeString = "-deferred-";
            break;
          case SymSym:
            csEntry.symTypeString = "SYM";
            break;
#if API_VERSION_NUMBER >= 9
          case SymDia:
            csEntry.symTypeString = "DIA";
            break;
#endif
          case 8: //SymVirtual:
            csEntry.symTypeString = "Virtual";
            break;
          default:
            //_snprintf( ty, sizeof ty, "symtype=%ld", (long) Module.SymType );
            csEntry.symTypeString = NULL;
            break;
        }
        
        __safe_strcpy(csEntry.moduleName, Module.ModuleName,
                      STACKWALK_MAX_NAMELEN);
        csEntry.baseOfImage = Module.BaseOfImage;
        __safe_strcpy(csEntry.loadedImageName, Module.LoadedImageName,
                      STACKWALK_MAX_NAMELEN);
      } // got module info OK
      else
      {
//        this->OnDbgHelpErr("SymGetModuleInfo64", GetLastError(),
//                           s.AddrPC.Offset);
      }
    } // we seem to have a valid PC
    
    et = nextEntry;
    if(frameNum == 0)
      et = firstEntry;
    __stacktrace_dump_callstack_entry(et, &csEntry, stream);
    
    if(s.AddrReturn.Offset == 0)
    {
      __stacktrace_dump_callstack_entry(lastEntry, &csEntry, stream);
      SetLastError(ERROR_SUCCESS);
      break;
    }
  } // for(frameNum)

cleanup:
  if(pSym)
    free(pSym);
    
  return TRUE;
}

/*
 */

void stacktrace_init(void)
{
  DWORD opts;
  HANDLE hProcess = GetCurrentProcess();
  
  if(SymInitialize(hProcess, NULL, FALSE) == FALSE)
  {
    fprintf(stderr, "SymInitialize() failed!\n");
    return;
  }

  opts = SymGetOptions();
  opts |= SYMOPT_LOAD_LINES;
  opts |= SYMOPT_FAIL_CRITICAL_ERRORS;
  SymSetOptions(opts);

  __stacktrace_load_modules(hProcess);
}

/*
 */

void stacktrace_print(FILE *stream, unsigned int max_frames, int flags)
{
  if(! __stacktrace_walk_stack(stream, max_frames, flags))
    fprintf(stream, "(Stack tracing not implemented on this platform.)\n");
}

/*
 */

void stacktrace_cleanup(void)
{
  HANDLE hProcess = GetCurrentProcess();
  
  SymCleanup(hProcess);
}
