/* ---------------------------------------------------------------------------
   commonc++ - A C++ Common Class Library
   Copyright (C) 2005-2014  Mark A Lindner

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

#ifdef HAVE_CONFIG_H
#include "cpp_config.h"
#endif

#include "commonc++/File.h++"
#include "commonc++/FilePtr.h++"
#include "commonc++/FileTraverser.h++"
#include "commonc++/UTF8Decoder.h++"

#ifdef CCXX_OS_WINDOWS
#include "commonc++/Windows.h++"

#include <io.h>

#define FS '\\'
#define EOL "\r\n"
#else
#define FS '/'
#define EOL "\n"
#endif

#ifdef CCXX_OS_POSIX
#include "commonc++/POSIX.h++"

#include <fcntl.h>
#include <utime.h>
#include <unistd.h>
#endif

#include "commonc++/System.h++"

#include <sys/types.h>
#include <sys/stat.h>

#ifdef HAVE_SYS_VFS_H
#include <sys/vfs.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_MOUNT_H
#include <sys/mount.h>
#endif

#include <cstdio>
#include <cerrno>

namespace ccxx {

const char File::separator = FS;
const char File::separatorString[] = { File::separator, NUL };

const char File::validSeparators[] = { File::separator,
#ifdef CCXX_OS_WINDOWS
                                       '/',
#endif
                                       NUL };

const char *File::eol = EOL;

/*
 */

class FileTreeDeleter : public FileTraverser
{
  public:

  FileTreeDeleter(const String& path)
    : FileTraverser(path),
      _succeeded(true)
  {
  }

  virtual ~FileTreeDeleter() { }

  inline bool succeeded() const
  { return(_succeeded); }

  protected:

  virtual bool processFile(const FileName& name, bool isDir, uint_t depth)
  {
    bool ok = false;

    if(isDir)
      ok = File::removeDirectory(name.getPathName());
    else
      ok = File::remove(name.getPathName());

    if(!ok)
      _succeeded = false;

    return(true);
  }

  private:

  bool _succeeded;
};

/*
 */

File::File(const String& path)
  : _name(path)
{
}

/*
 */

File::~File()
{
}

/*
 */

void File::open(IOMode mode /* = IOReadWrite */,
                FileOpenMode openMode /* = FileOpenExisting */,
                const Permissions& perm /* = Permissions::USER_READ_WRITE */)
{
  bool canRead = false, canWrite = false;

#ifdef CCXX_OS_WINDOWS

  SECURITY_ATTRIBUTES sa;
  WinPerms wperm;

  Windows::encodePermissions(perm, wperm);

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = wperm.pdesc;
  sa.bInheritHandle = FALSE;

  DWORD share = 0, f = 0, disp = 0;

  switch(openMode)
  {
    case FileOpen:
      disp = OPEN_EXISTING;
      break;

    case FileCreate:
      disp = CREATE_NEW;
      break;

    case FileOpenElseCreate:
      disp = OPEN_ALWAYS;
      break;

    case FileTruncate:
      disp = TRUNCATE_EXISTING;
      break;

    case FileTruncateElseCreate:
      disp = CREATE_ALWAYS;
      break;
  }

  switch(mode)
  {
    case IORead:
      f = FILE_READ_DATA;
      canRead = true;
      break;

    case IOWrite:
      f = FILE_WRITE_DATA;
      canWrite = true;
      break;

    case IOReadWrite:
      f = FILE_READ_DATA | FILE_WRITE_DATA;
      canRead = true;
      canWrite = true;
      break;
  }

  share = (FILE_SHARE_READ | FILE_SHARE_WRITE);

  FileHandle r = ::CreateFileW(_name.data(), f, share, &sa, disp,
                              FILE_FLAG_OVERLAPPED, NULL);

  if(r == CCXX_INVALID_FILE_HANDLE)
  {
    int err = ::GetLastError();

    if((err == ERROR_FILE_NOT_FOUND) || (err == ERROR_PATH_NOT_FOUND))
      throw PathNotFoundException(_name);
    else
      throw IOException(System::getErrorString("CreateFile"));
  }

#else

  int flags = 0;
  mode_t perm_;

  POSIX::encodePermissions(perm, perm_);

  switch(openMode)
  {
    case FileOpen:
      break;

    case FileCreate:
      flags |= (O_CREAT | O_EXCL);
      break;

    case FileOpenElseCreate:
      flags |= O_CREAT;
      break;

    case FileTruncate:
      flags |= O_TRUNC;
      break;

    case FileTruncateElseCreate:
      flags |= (O_CREAT | O_TRUNC);
      break;
  }

  switch(mode)
  {
    case IORead:
      flags |= O_RDONLY;
      canRead = true;
      break;

    case IOWrite:
      flags |= O_WRONLY;
      canWrite = true;
      break;

    case IOReadWrite:
      flags |= O_RDWR;
      canRead = true;
      canWrite = true;
      break;
  }

  CString cstr_name = _name.toUTF8();
  FileHandle r = ::open(cstr_name.data(), flags, perm_);
  if(r < 0)
  {
    if(errno == ENOENT)
      throw PathNotFoundException(_name);
    else
      throw IOException(System::getErrorString("open"));
  }

#endif

  Stream::_init(r, true, canRead, canWrite);
}

/*
 */

bool File::makeDirectory(const String& path,
                         bool createAll /* = false */,
                         const Permissions& perm
                         /* = Permissions::defaultDirPerms */)
{
  if(path.isEmpty())
    return(false);

#ifdef CCXX_OS_WINDOWS

  SECURITY_ATTRIBUTES sa;
  WinPerms wperm;

  Windows::encodePermissions(perm, wperm);

  sa.nLength = sizeof(SECURITY_ATTRIBUTES);
  sa.lpSecurityDescriptor = wperm.pdesc;
  sa.bInheritHandle = FALSE;

#else

  mode_t mode;
  POSIX::encodePermissions(perm, mode);

#endif

  if(createAll)
  {
    String s = "";
    uint_t index = 0;
    bool addslash = isValidSeparator(path[0].toChar16());
    String tok;

    while((tok = path.nextToken(index, validSeparators)))
    {
      if(addslash)
        s += separator;

      s += tok;
      addslash = true;

      if(tok.endsWith(':'))
        continue;

#ifdef CCXX_OS_WINDOWS

      bool ok = ((::CreateDirectoryW(s.data(), &sa) == TRUE)
                 || (::GetLastError() == ERROR_ALREADY_EXISTS));

#else

      CString cstr_s = s.toUTF8();
      bool ok = ((::mkdir(cstr_s.data(), mode) == 0) || (errno == EEXIST));

#endif

      if(! ok)
        return(false);
    }

    return(true);
  }
  else
  {
#ifdef CCXX_OS_WINDOWS

    return((::CreateDirectoryW(removeTrailingSeparators(path).data(), &sa)
            == TRUE)
           || (::GetLastError() == ERROR_ALREADY_EXISTS));

#else

    CString cstr_path = path.toUTF8();
    return((::mkdir(cstr_path.data(), mode) == 0) || (errno == EEXIST));

#endif
  }
}

/*
 */

bool File::remove(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  return(::DeleteFileW(path.data()) == TRUE);

#else

  CString cstr_path = path.toUTF8();
  return(std::remove(cstr_path.data()) == 0);

#endif
}

/*
 */

bool File::removeDirectory(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  return(::RemoveDirectoryW(removeTrailingSeparators(path).data()) == TRUE);

#else

  CString cstr_path = path.toUTF8();
  return(::rmdir(cstr_path.data()) == 0);

#endif
}

/*
 */

bool File::removeDirectoryTree(const String& path)
{
  FileTreeDeleter deleter(path);
  return(deleter.traverse() && deleter.succeeded());
}

/*
 */

bool File::exists() const
{
  return(exists(_name));
}

/*
 */

bool File::isDirectory() const
{
  return(isDirectory(_name));
}

/*
 */

bool File::isPipe() const
{
  return(isPipe(_name));
}

/*
 */

bool File::isFile() const
{
  return(isFile(_name));
}

/*
 */

bool File::isSymLink() const
{
  return(isSymLink(_name));
}

/*
 */

bool File::exists(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  return(::_waccess(removeTrailingSeparators(path).data(), 0) == 0);

#else

  CString cstr_path = path.toUTF8();
  return(::access(cstr_path.data(), F_OK) == 0);

#endif
}

/*
 */

bool File::isDirectory(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  struct _stat stbuf;
  if(::_wstat(removeTrailingSeparators(path).data(), &stbuf) != 0)
    return(false);

  return((stbuf.st_mode & _S_IFDIR) != 0);

#else

  struct stat stbuf;
  CString cstr_path = path.toUTF8();
  if(::stat(cstr_path.data(), &stbuf) != 0)
    return(false);

  return(S_ISDIR(stbuf.st_mode));

#endif
}

/*
 */

bool File::isFile(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  struct _stat stbuf;
  if(::_wstat(removeTrailingSeparators(path).data(), &stbuf) != 0)
    return(false);

  return((stbuf.st_mode & _S_IFREG) != 0);

#else

  struct stat stbuf;
  CString cstr_path = path.toUTF8();
  if(::stat(cstr_path.data(), &stbuf) != 0)
    return(false);

  return(S_ISREG(stbuf.st_mode));

#endif
}

/*
 */

bool File::isPipe(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  struct _stat stbuf;
  if(::_wstat(removeTrailingSeparators(path).data(), &stbuf) != 0)
    return(false);

  return((stbuf.st_mode & _S_IFIFO) != 0);

  return(false);

#else

  struct stat stbuf;
  CString cstr_path = path.toUTF8();
  if(::stat(cstr_path.data(), &stbuf) != 0)
    return(false);

  return(S_ISFIFO(stbuf.st_mode)
#ifdef S_ISSOCK
         || S_ISSOCK(stbuf.st_mode)
#endif
    );

#endif
}

/*
 */

bool File::isSymLink(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  return(false); // no symlinks on Windows

#else

  struct stat stbuf;
  CString cstr_path = path.toUTF8();
  if(::lstat(cstr_path.data(), &stbuf) != 0)
    return(false);

  return(S_ISLNK(stbuf.st_mode));

#endif
}

/*
 */

bool File::readSymLink(String& path)
{
#ifdef CCXX_OS_WINDOWS

  return(false); // no symlinks on Windows

#else

  char buf[PATH_MAX + 1];
  buf[PATH_MAX] = NUL;

  CString cstr_path = path.toUTF8();
  int r = ::readlink(cstr_path.data(), buf, PATH_MAX);
  if(r < 0)
    return(false);

  buf[r] = NUL;

  path = buf;
  return(true);

#endif
}

/*
 */

bool File::resolveSymLink(String& path)
{
#ifdef CCXX_OS_WINDOWS

  return(false);

#else

  String lpath = path;
  if(! readSymLink(lpath))
    return(false);

  FileName fn(removeTrailingSeparators(path));
  fn.setFileName(lpath);
  fn.canonicalize();

  path = fn.getPathName();

  return(true);

#endif
}

/*
 */

bool File::createLink(const String& oldPath, const String& newPath,
                      bool hardLink /* = false */)
{
#ifdef CCXX_OS_WINDOWS

  if(! hardLink)
    return(false);

  return(::CreateHardLinkW(removeTrailingSeparators(oldPath).data(),
                           removeTrailingSeparators(newPath).data(), NULL)
         == TRUE);

#else

  int r;
  CString cstr_oldPath = oldPath.toUTF8();
  CString cstr_newPath = newPath.toUTF8();


  if(hardLink)
    r = ::link(cstr_oldPath.data(), cstr_newPath.data());
  else
    r = ::symlink(cstr_oldPath.data(), cstr_newPath.data());

  return(r == 0);

#endif
}

/*
 */

bool File::setPermissions(const Permissions& perm)
{
  return(setPermissions(_name, perm));
}

/*
 */

bool File::getPermissions(Permissions& perm)
{
  return(getPermissions(_name, perm));
}

/*
 */

bool File::setPermissions(const String& path, const Permissions& perm)
{
#ifdef CCXX_OS_WINDOWS

  WinPerms wperm;

  if(! Windows::encodePermissions(perm, wperm))
    return(false);

  return(::SetFileSecurityW(removeTrailingSeparators(path).data(),
                            DACL_SECURITY_INFORMATION, wperm.pdesc) == TRUE);

#else

  mode_t mode;
  POSIX::encodePermissions(perm, mode);

  CString cstr_path = path.toUTF8();
  return(::chmod(cstr_path.data(), mode) == 0);

#endif
}

/*
 */

bool File::getPermissions(const String& path, Permissions& perm)
{
#ifdef CCXX_OS_WINDOWS

  WinPerms wperm; // needs members to be the above crap

  if(::GetNamedSecurityInfoW(removeTrailingSeparators(path).data(),
                             SE_FILE_OBJECT,
                             (OWNER_SECURITY_INFORMATION
                              | GROUP_SECURITY_INFORMATION
                              | DACL_SECURITY_INFORMATION),
                             &wperm.user, &wperm.group, &wperm.dacl, NULL,
                             &wperm.pdesc) != ERROR_SUCCESS)
    return(false);

  return Windows::decodePermissions(wperm, perm);

#else

  struct stat stbuf;

  CString cstr_path = path.toUTF8();
  if(::stat(cstr_path.data(), &stbuf) != 0)
    return(false);

  POSIX::decodePermissions(stbuf.st_mode, perm);
  return(true);

#endif

}

/*
 */

bool File::getAttributes(const String& path, FileAttributes& attrs)
{
#ifdef CCXX_OS_WINDOWS

  WIN32_FILE_ATTRIBUTE_DATA data;

  if(::GetFileAttributesExW(removeTrailingSeparators(path).data(),
                            GetFileExInfoStandard, &data) == 0)
    return(false);

  if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    attrs.type = TypeDir;
  else
    attrs.type = TypeFile;

  attrs.size = (((uint64_t)data.nFileSizeHigh << 32) | data.nFileSizeLow);

  attrs.created = Windows::decodeFileTime(data.ftCreationTime);
  attrs.modified = Windows::decodeFileTime(data.ftLastWriteTime);
  attrs.accessed = Windows::decodeFileTime(data.ftLastAccessTime);

  File::getPermissions(path, attrs.permissions);

  return(true);

#else

  struct stat stbuf;
  CString cstr_path = path.toUTF8();

  if(::stat(cstr_path.data(), &stbuf) != 0)
    return(false);

    if(S_ISREG(stbuf.st_mode))
      attrs.type = TypeFile;
    else if(S_ISDIR(stbuf.st_mode))
      attrs.type = TypeDir;
    else if(S_ISFIFO(stbuf.st_mode))
      attrs.type = TypePipe;
#ifdef S_ISSOCK
    else if(S_ISSOCK(stbuf.st_mode))
      attrs.type = TypePipe;
#endif
    else
      attrs.type = TypeOther;

    attrs.created = INT64_CONST(0); // can't determine on POSIX systems
    attrs.modified = static_cast<time_ms_t>(stbuf.st_mtime) * 1000;
    attrs.accessed = static_cast<time_ms_t>(stbuf.st_atime) * 1000;

    attrs.size = static_cast<uint64_t>(stbuf.st_size);

    POSIX::decodePermissions(stbuf.st_mode, attrs.permissions);

#endif

  return(true);
}

/*
 */

int64_t File::getSize(const String& path)
{
  int64_t size = -1;

#ifdef CCXX_OS_WINDOWS

  struct _stati64 stbuf;
  if(::_wstati64(removeTrailingSeparators(path).data(), &stbuf) == 0)
    size = stbuf.st_size;

#else

  struct stat stbuf;
  CString cstr_path = path.toUTF8();
  if(::stat(cstr_path.data(), &stbuf) == 0)
    size = stbuf.st_size;

#endif

  return(size);
}

/*
 */

bool File::touch()
{
  return(touch(_name));
}

/*
 */

String File::readToString(const String& path)
{
  String result;

  char inputBuf[32768];
  char16_t outputBuf[32768];
  UTF8Decoder decoder;
  char *input = inputBuf;
  char16_t *output = outputBuf;
  int inputLength = CCXX_LENGTHOF(inputBuf);
  int outputLength = CCXX_LENGTHOF(outputBuf);
  int inputCountLeft = 0;
  int outputCountLeft = outputLength;

  FilePtr fp(path, "r");
  if(!fp.isValid())
    return(String::null);

  bool refill = true;

  for(;;)
  {
    if(refill)
    {
      input = inputBuf;
      inputCountLeft = ::fread(input, 1, inputLength, fp);
      if(inputCountLeft <= 0)
        break;

      refill = false;
    }

    int status = decoder.decode((const char **)&input, &inputCountLeft,
                                &output, &outputCountLeft);

    if(status == UTF8Decoder::STATUS_OUTPUT_BUFFER_FULL)
    {
      result.append(output, outputLength - outputCountLeft);
      output = outputBuf;
      outputCountLeft = outputLength;
    }
    else if(status == UTF8Decoder::STATUS_NEED_MORE_INPUT)
      refill = true;
    else if(status == UTF8Decoder::STATUS_OK)
    {
      result.append(output, outputLength - outputCountLeft);
      break;
    }
    else
      return(String::null);
  }

  return result;
}

/*
 */

bool File::remove()
{
  return(remove(_name));
}

/*
 */

bool File::touch(const String& path)
{
  bool ok = false;

#ifdef CCXX_OS_WINDOWS

  HANDLE handle = ::CreateFileW(removeTrailingSeparators(path).data(),
                                GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
                                OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if(handle != CCXX_INVALID_FILE_HANDLE)
  {
    SYSTEMTIME now;
    FILETIME ft;

    ::GetSystemTime(&now);
    ::SystemTimeToFileTime(&now, &ft);

    ok = (::SetFileTime(handle, &ft, &ft, &ft) == TRUE);
    ::CloseHandle(handle);
  }

#else

  int r;

  time_t now = ::time(NULL);
  struct utimbuf buf = { now, now };
  CString cstr_path = path.toUTF8();

  if(::utime(cstr_path.data(), &buf) != 0)
  {
    if(errno == ENOENT)
    {
      r = ::mknod(cstr_path.data(),
                  (S_IFREG | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP), 0);

      ok = (r == 0);
    }
  }
  else
    ok = true;

#endif

  return(ok);
}

/*
 */

bool File::lockRange(uint64_t start, uint64_t length, LockType type,
                     bool wait /* = true */)
{
  if(! _canWrite)
    return(false);

#ifdef CCXX_OS_WINDOWS

  OVERLAPPED ov;

  ov.Offset = static_cast<DWORD>(start & 0xFFFFFFFF);
  ov.OffsetHigh = static_cast<DWORD>((start >> 32) & 0xFFFFFFFF);
  ov.hEvent = 0;

  DWORD flags = (type == LockRead ? 0 : LOCKFILE_EXCLUSIVE_LOCK);
  if(! wait)
    flags |= LOCKFILE_FAIL_IMMEDIATELY;

  return(::LockFileEx(_handle, flags, 0,
                      static_cast<DWORD>(length & 0xFFFFFFFF),
                      static_cast<DWORD>((length >> 32) & 0xFFFFFFFF),
                      &ov) == TRUE);

#else

  struct flock fl;

  fl.l_type = (type == LockRead ? F_RDLCK : F_WRLCK);
  fl.l_whence = SEEK_SET;
  fl.l_start = static_cast<off_t>(start);
  fl.l_len = static_cast<off_t>(length);

  return(::fcntl(_handle, (wait ? F_SETLKW : F_SETLK), &fl) == 0);

#endif
}

/*
 */

bool File::unlockRange(uint64_t start, uint64_t length)
{
  if(! _canWrite)
    return(false);

#ifdef CCXX_OS_WINDOWS

  OVERLAPPED ov;

  ov.Offset = static_cast<DWORD>(start & 0xFFFFFFFF);
  ov.OffsetHigh = static_cast<DWORD>((start >> 32) & 0xFFFFFFFF);
  ov.hEvent = 0;

  return(::UnlockFileEx(_handle,  0,
                        static_cast<DWORD>(length & 0xFFFFFFFF),
                        static_cast<DWORD>((length >> 32) & 0xFFFFFFFF),
                        &ov) == TRUE);

#else

  struct flock fl;

  fl.l_type = F_UNLCK;
  fl.l_whence = SEEK_SET;
  fl.l_start = static_cast<off_t>(start);
  fl.l_len = static_cast<off_t>(length);

  return(::fcntl(_handle, F_SETLK, &fl) == 0);

#endif
}

/*
 */

void File::truncate(uint64_t size /* = 0 */)
{
  uint64_t curSize = 0;

  // determine current size

#ifdef CCXX_OS_WINDOWS

  seek(size);

  DWORD hi, lo;

  lo = ::GetFileSize(_handle, &hi);
  if((lo == INVALID_FILE_SIZE) && (::GetLastError() != NO_ERROR))
    throw IOException(System::getErrorString());

  curSize = hi;
  curSize <<= 32;
  curSize |= lo;

#else

  struct stat stbuf;

  if(::fstat(_handle, &stbuf) != 0)
    throw IOException(System::getErrorString());

  curSize = stbuf.st_size;

#endif

  // verify current size not less than truncate size

  if(curSize < size)
    throw IOException("Truncating to a larger size not supported");

  // truncate the file

#ifdef CCXX_OS_WINDOWS

  if(::SetEndOfFile(_handle) == FALSE)
    throw IOException(System::getErrorString());

#else

  if(::ftruncate(_handle, size) == -1)
    throw IOException(System::getErrorString());

#endif

  // ensure consistent behavior on all platforms:
  seek(0, SeekEnd);
}

/*
 */

int64_t File::getSize()
{
  if(isOpen())
  {
#ifdef CCXX_OS_WINDOWS

    uint64_t size;
    DWORD hi, lo;

    lo = ::GetFileSize(_handle, &hi);
    if((lo == INVALID_FILE_SIZE) && (::GetLastError() != NO_ERROR))
      throw IOException(System::getErrorString());

    size = hi;
    size <<= 32;
    size |= lo;
    return(static_cast<int64_t>(size));

#else

    struct stat stbuf;

    if(fstat(_handle, &stbuf) != 0)
      throw IOException(System::getErrorString());

    return(stbuf.st_size);

#endif
  }
  else
  {
    int64_t sz = getSize(_name);

    if(sz < 0)
      throw IOException(System::getErrorString());

    return(sz);
  }
}

/*
 */

bool File::rename(const String& oldName, const String& newName)
{
  CString cstr_oldName = oldName.toUTF8();
  CString cstr_newName = newName.toUTF8();

  return(::rename(cstr_oldName.data(), cstr_newName.data()) == 0);
}

/*
 */

bool File::copy(const String& oldName, const String& newName)
{

#ifdef CCXX_OS_WINDOWS

  return(::CopyFileW(oldName.data(), newName.data(), FALSE) == TRUE);

#else

  byte_t buf[4096];
  size_t r, w;
  int fdin, fdout;
  mode_t mode = S_IRUSR | S_IWUSR;

  CString cstr_oldName = oldName.toUTF8();
  CString cstr_newName = newName.toUTF8();

  fdin = ::open(cstr_oldName.data(), O_RDONLY);
  if(fdin < 0)
    return(false);

  fdout = ::open(cstr_newName.data(), O_WRONLY | O_CREAT | O_EXCL, mode);
  if(fdout < 0)
  {
    ::close(fdin);
    return(false);
  }

  bool ok = true;

  while(ok && ((r = ::read(fdin, buf, sizeof(buf))) != 0))
  {
    if(r < 0)
    {
      if(errno == EINTR)
        continue;
      else
      {
        ok = false;
        break;
      }
    }

    const byte_t *p = buf;
    while(r > 0)
    {
      w = ::write(fdout, p, r);

      if(w < 0)
      {
        if(errno == EINTR)
          continue;
        else
        {
          ok = false;
          break;
        }
      }

      p += w;
      r -= w;
    }
  }

  ::close(fdin);
  ::close(fdout);

  return(ok);

#endif
}

/*
 */

bool File::move(const String& oldFile, const String& newFile)
{
#ifdef CCXX_OS_WINDOWS

  return(::MoveFileW(removeTrailingSeparators(oldFile).data(),
                     removeTrailingSeparators(newFile).data()) == TRUE);

#else

  CString cstr_oldFile = oldFile.toUTF8();
  CString cstr_newFile = newFile.toUTF8();

  int r = ::link(cstr_oldFile.data(), cstr_newFile.data());
  if(r != 0)
    return(false);

  if(::unlink(cstr_oldFile.data()) != 0)
  {
    ::unlink(cstr_newFile.data());

    return(false);
  }
  else
    return(true);

#endif
}

/*
 */

void File::setCreationMask(const Permissions& perm)
{
#ifdef CCXX_OS_POSIX

  mode_t perm_;

  POSIX::encodePermissions(perm, perm_);
  ::umask(perm_);

#endif
}

/*
 */

uint64_t File::getDiskFreeSpace(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  ULARGE_INTEGER x, avail;

  if(::GetDiskFreeSpaceExW(removeTrailingSeparators(path).data(), &x, &x,
                           &avail) == FALSE)
    return(UINT64_CONST(0));

  return(static_cast<uint64_t>(avail.QuadPart));

#else

  struct statfs buf;
  CString cstr_path = path.toUTF8();
  if(::statfs(cstr_path.data(), &buf) != 0)
    return(UINT64_CONST(0));

  return(static_cast<uint64_t>(buf.f_bavail) * 1024);

#endif
}

/*
 */

size_t File::getDiskBlockSize(const String& path)
{
#ifdef CCXX_OS_WINDOWS

  DWORD sectorsPerCluster, bytesPerSector, x;

  if(::GetDiskFreeSpaceW(removeTrailingSeparators(path).data(),
                         &sectorsPerCluster, &bytesPerSector, &x, &x) == FALSE)
  {
    // This shouldn't fail, but if it does, make an educated guess of 4K, which
    // is standard for drives > 2Gb (a safe bet these days).

    return(4096);
  }

  return(static_cast<size_t>(sectorsPerCluster * bytesPerSector));

#else

  struct statfs buf;
  CString cstr_path = path.toUTF8();
  if(::statfs(cstr_path.data(), &buf) != 0)
    return(4096); // educated guess

  return(static_cast<size_t>(buf.f_bsize));

#endif
}

/*
 */

int64_t File::roundSizeToBlockSize(const int64_t& fileSize, size_t blockSize)
{
  int64_t rem = (fileSize % blockSize);

  return((rem > 0) ? (fileSize + blockSize - rem) : fileSize);
}

/*
 */

void File::getFileSystemRoots(StringVec& roots, bool localOnly /* = true */)
{
  roots.clear();

#ifdef CCXX_OS_WINDOWS

  DWORD mask = ::GetLogicalDrives();

  for(int i = 0; i < 26; i++)
  {
    if(mask & 0x01)
    {
      char path[4];

      std::sprintf(path, "%c:\\", 'A' + i);

      if(! localOnly || (::GetDriveType(path) == DRIVE_FIXED))
        roots << path;
    }

    mask >>= 1;
  }

#else

  roots << "/";

#endif
}

/*
 */

bool File::isValidSeparator(char c)
{
  return(std::strchr(validSeparators, c) != NULL);
}

/*
 */

bool File::isValidSeparator(char16_t c)
{
  for(const char* p = validSeparators; *p; ++p)
  {
    if(c == static_cast<char16_t>(*p))
      return true;
  }

  return(false);
}

/*
 */

String File::removeTrailingSeparators(const String& path)
{
  if(path.isEmpty())
    return(path);

  uint_t min = 0;
  size_t len = path.length();

  if(isValidSeparator(path[0].toChar16()))
    min = 1;

#ifdef CCXX_OS_WINDOWS

  else if((len >= 3) && (path[1] == ':')
          && isValidSeparator(path[2].toChar16()))
    min = 3;

#endif

  uint_t n = 0;

  for(uint_t i = len - 1; i >= min; --i)
  {
    if(isValidSeparator(path[i].toChar16()))
      ++n;
    else
      break;
  }

  return((n > 0) ? path.substring(0, len - n) : path);
}


} // namespace ccxx
