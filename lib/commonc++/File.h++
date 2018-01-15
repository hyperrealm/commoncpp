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

#ifndef __ccxx_File_hxx
#define __ccxx_File_hxx

#include <commonc++/Common.h++>
#include <commonc++/FileName.h++>
#include <commonc++/Permissions.h++>
#include <commonc++/Stream.h++>
#include <commonc++/String.h++>

namespace ccxx {

/** %File types. */
enum FileType {
  /** Regular file. */
  TypeFile,
  /** Directory. */
  TypeDir,
  /** Symbolic link. */
  TypeSymLink,
  /** Named pipe. */
  TypePipe,
  /** Device file. */
  TypeDevice,
  /** Other filetype. */
  TypeOther,
  /** Unknown filetype. */
  TypeUnknown
};

/** %File open modes. */
enum FileOpenMode {
  /** Open the file if it exists, otherwise create the file. */
  FileOpenElseCreate,
  /** Open the file if it exists, otherwise fail. */
  FileOpen,
  /** Create the file if it does not exist, otherwise fail. */
  FileCreate,
  /** Open and truncate the file if it exists, otherwise fail. */
  FileTruncate,
  /** Open and truncate the file if it exists, otherwise create the file. */
  FileTruncateElseCreate
};

/**
 * %File attributes.
 *
 * @author Mark Lindner
 */
struct COMMONCPP_API FileAttributes
{
  /** Constructor. */
  FileAttributes()
    : type(TypeUnknown),
      created(INT64_CONST(0)),
      modified(INT64_CONST(0)),
      accessed(INT64_CONST(0)),
      size(UINT64_CONST(0))
  { }

  /** %File type. */
  FileType type;
  /** %File creation time (not available on all platforms). */
  time_ms_t created;
  /** %File last-modified time. */
  time_ms_t modified;
  /** %File last-accessed time. */
  time_ms_t accessed;
  /** %File size, in bytes. */
  uint64_t size;
  /** %File access permissions. */
  Permissions permissions;

  /** Test if the file is a directory. */
  inline bool isDirectory() const
  { return(type == TypeDir); }

  /** Test if the file is a plain file. */
  inline bool isFile() const
  { return(type == TypeFile); }
};

/** %File lock types. */
enum LockType {
  /** %Lock for reading. */
  LockRead,
  /** %Lock for writing. */
  LockWrite
};

/**
 * A representation of a file in the filesystem. The class also
 * provides some static filesystem-related utility functions.
 *
 * @author Mark Lindner
 */
class COMMONCPP_API File : public Stream
{
 public:

  /**
   * Construct a new File with the given path.
   *
   * @param path The path.
   */
  File(const String& path);

  /** Destructor. */
  virtual ~File();

  /**
   * Open the file.
   *
   * @param mode The I/O mode.
   * @param openMode The file open mode.
   * @param perm The permissions for the file, if the file is being
   * created.
   * @throw IOException If an I/O error occurs.
   */
  void open(IOMode mode = IOReadWrite,
            FileOpenMode openMode = FileOpenElseCreate,
            const Permissions& perm = Permissions::USER_READ_WRITE);

  /** Test if the file exists. */
  bool exists() const;

  /** Test if the file is a directory. */
  bool isDirectory() const;

  /** Test if the file is a plain file. */
  bool isFile() const;

  /** Test if the file is a pipe or socket. */
  bool isPipe() const;

  /**
   * Test if the file is a symbolic link. (Some platforms do not support
   * symbolic links.)
   * @return <b>true</b> if the file exists and is a symbolic link,
   * <b>false</b> otherwise.
   */
  bool isSymLink() const;

  /**
   * Touch the file. The file's last-accessed timestamp is updated if
   * the file exists; otherwise the file is created as an empty file.
   *
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  bool touch();

  /**
   * Remove the file.
   *
   * @return <b>true</b> if the file was successfully removed,
   * <b>false</b> otherwise.
   */
  bool remove();

  /**
   * Set the file permissions.
   *
   * @param perm The Permissions to set.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  bool setPermissions(const Permissions& perm);

  /**
   * Get the file permissions. (NOTE: this call is <i>very</i> slow
   * on Windows.)
   *
   * @param perm The Permissions object into which the file's permissions
   * should be placed.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  bool getPermissions(Permissions& perm);

  /**
   * %Lock a range of bytes in the file. The type of lock may be mandatory
   * or advisory, depending on what the host system supports.
   *
   * @param start The start offset of the range to lock.
   * @param length The length of the range to lock.
   * @param type The type of lock.
   * @param wait A flag indicating whether the method should block until
   * the lock is acquired, or return immediately if it can't be.
   * @return <b>true</b> if the range was successfully locked,
   * <b>false</b> otherwise.
   */
  bool lockRange(uint64_t start, uint64_t length, LockType type,
                 bool wait = true);

  /**
   * Unlock a range of bytes in the file. The range must match a range
   * that was previously locked with <b>lockRange</b>().
   *
   * @param start The start offset of the range to unlock.
   * @param length The length of the range to unlock.
   * @return <b>true</b> if the range was successfully unlocked,
   * <b>false</b> otherwise.
   */
  bool unlockRange(uint64_t start, uint64_t length);

  /**
   * Truncate the file to a given size. On return, the file seek pointer
   * will be at the end of the file.
   *
   * @param size The new size of the file, which must be less than or
   * equal to the file's current size. Truncating a file to extend its
   * size produces different results on POSIX vs. Windows systems, and thus
   * is not supported.
   * @throw IOException If the operation fails, or if the specified size
   * is greater than the file's current size.
   */
  void truncate(uint64_t size = 0);

  /** Get the pathname for the file. */
  inline String getPath() const
  { return(_name); }

  /**
   * Get the size of the file.
   *
   * @return The size of the file.
   * @throw IOException If an error occurs.
   */
  int64_t getSize();

  /**
   * Create a directory.
   *
   * @param path The path to the new directory.
   * @param createAll A flag indicating whether any nonexsitent intermediate
   * directories in <i>path</i> should also be created.
   * @param perm The permissions to assign to the new directory.
   * @return <b>true</b> if the directory was successfully created (or
   * already existed), <b>false</b> otherwise.
   */
  static bool makeDirectory(
    const String& path, bool createAll = false,
    const Permissions& perm = Permissions::defaultDirPerms);

  /**
   * Remove a file.
   *
   * @param path The path to the file.
   * @return <b>true</b> if the file was successfully removed,
   * <b>false</b> otherwise.
   */
  static bool remove(const String& path);

  /**
   * Remove a directory. Only empty directories may be removed using
   * this function.
   *
   * @param path The path to the directory.
   * @return <b>true</b> if the directory was successfully removed,
   * <b>false</b> otherwise.
   */
  static bool removeDirectory(const String& path);

  /**
   * Recursively remove all files and directories rooted at a given directory.
   * The removal process will not be interrupted if any errors are encountered.
   *
   * @param path The path to the root of the directory tree to remove.
   * @return <b>true</b> if the directory tree was successfully removed,
   * <b>false</b> if any errors occurred during the removal.
   */
  static bool removeDirectoryTree(const String& path);

  /**
   * Test a file for existence.
   *
   * @param path The path of the file.
   * @return <b>true</b> if the file exists, <b>false</b> otherwise.
   */
  static bool exists(const String& path);

  /**
   * Test if a file is a directory.
   *
   * @param path The path of the file.
   * @return <b>true</b> if the file exists and is a directory,
   * <b>false</b> otherwise.
   */
  static bool isDirectory(const String& path);

  /**
   * Test if a file is a plain file.
   *
   * @param path The path of the file.
   * @return <b>true</b> if the file exists and is a plain file,
   * <b>false</b> otherwise.
   */
  static bool isFile(const String& path);

  /**
   * Test if a file is a pipe or socket.
   *
   * @param path The path of the file.
   * @return <b>true</b> if the file exists and is a pipe or socket,
   * <b>false</b> otherwise.
   */
  static bool isPipe(const String& path);

  /**
   * Test if a file is a symbolic link. (Some platforms do not support
   * symbolic links.)
   *
   * @param path The path of the file.
   * @return <b>true</b> if the file exists and is a symbolic link,
   * <b>false</b> otherwise.
   */
  static bool isSymLink(const String& path);

  /**
   * Read a symbolic link.
   *
   * @param path The path to the symbolic link. On return, the literal
   * path to the file to which the link points (relative to the link
   * itself).
   * @return <b>true</b> on success, or <b>false</b> if the symbolic link
   * could not be read, or if the path does not refer to a symbolic link.
   */
  static bool readSymLink(String& path);

  /**
   * Resolve a symbolic link.
   *
   * @param path The path to the symbolic link. On return, the
   * canonical path to the file to which the link points.
   * @return <b>true</b> on success, or <b>false</b> if the symbolic link
   * could not be read, or if the path does not refer to a symbolic link.
   */
  static bool resolveSymLink(String& path);

  /**
   * Create a hard or soft link to an existing file.
   *
   * @param oldPath The path to the existing file.
   * @param newPath The path of the link.
   * @param hardLink A flag indicating whether the link should be a hard
   * link or soft (symbolic) link. Windows only supports hard links, so
   * attempts to create a symbolic link on that platform will fail.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool createLink(const String& oldPath, const String& newPath,
                         bool hardLink = false);

  /**
   * Set the permissions on a file.
   *
   * @param path The file.
   * @param perm The Permissions to set.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool setPermissions(const String& path, const Permissions& perm);

  /**
   * Get the permissions for a file.
   *
   * @param path The file.
   * @param perm The Permissions object into which the file's permissions
   * should be placed.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool getPermissions(const String& path, Permissions& perm);

  /**
   * Get file attributes.
   *
   * @param path The file.
   * @param attrs The FileAttributes object into which the file's attributes
   * should be placed.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool getAttributes(const String& path, FileAttributes& attrs);

  /**
   * Get the size of a file.
   *
   * @param path The file.
   * @return The size of the file, in bytes, or -1 if the file was not
   * found.
   */
  static int64_t getSize(const String& path);

  /**
   * Rename a file.
   *
   * @param oldName The file to rename.
   * @param newName The new name for the file.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool rename(const String& oldName, const String& newName);

  /**
   * Copy a file to another file.
   *
   * @param oldFile The source file.
   * @param newFile The target file.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool copy(const String& oldFile, const String& newFile);

  /**
   * Move a file or directory to another location within the same
   * volume or filesystem.
   *
   * @param oldFile The source file.
   * @param newFile The target location.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool move(const String& oldFile, const String& newFile);

  /**
   * Touch a file. The file's last-accessed timestamp is updated if
   * the file exists; otherwise the file is created as an empty file.
   *
   * @param path The path to the file.
   * @return <b>true</b> on success, <b>false</b> otherwise.
   */
  static bool touch(const String& path);

  /**
   * Reads a file into a string. The file's contents are assumed to be
   * UTF-8 encoded text.
   *
   * @param path The path to the file.
   * @return The contents of tha file, as a String, or String::null if an
   * error occurred while reading the file.
   */
  static String readToString(const String& path);

  /**
   * Get the filesystem roots. On POSIX systems there is only one root,
   * namely "/". On Windows, there are multiple roots, corresponding to the
   * mapped drive letters such as "C:", "D:", etc.
   *
   * @param roots The vector in which to place the roots.
   * @param localOnly A flag indicating that only roots corresponding to
   * local (non-read-only, non-networked) disks should be returned. This
   * option is only significant on Windows.
   */
  static void getFileSystemRoots(StringVec& roots, bool localOnly = true);

  /**
   * Set the file creation mask for the current process. This function
   * is POSIX-specific; it has no effect on Windows.
   *
   * @param perm The permissions that will be turned off on newly created
   * files.
   */
  static void setCreationMask(const Permissions& perm);

  /**
   * Get the amount of free space, in bytes, on the disk or filesystem
   * associated with the given path.
   *
   * @param path The path. On POSIX systems, the path can refer to any file;
   * the value returned will be the amount of free space on the disk
   * partition containing that file. On Windows systems, the path should be
   * one of the filesystem roots as returned by <b>getFileSystemRoots</b>().
   * @return The amount of free disk space, in bytes.
   */
  static uint64_t getDiskFreeSpace(const String& path);

  /**
   * Get the disk block size, in bytes, on the disk or filesystem
   * associated with the given path.
   *
   * @param path The path. On POSIX systems, the path can refer to any file;
   * the value returned will be the amount of free space on the disk
   * partition containing that file. On Windows systems, the path should be
   * one of the filesystem roots as returned by <b>getFileSystemRoots</b>().
   * @return The size of a disk block, in bytes.
   */
  static size_t getDiskBlockSize(const String& path);

  /**
   * Round a file size to the next higher multiple of the given block
   * size. This method may be used to determine the actual amount of space
   * that a file occupies on disk.
   *
   * @param fileSize The file size.
   * @param blockSize The block size.
   * @return The rounded file size.
   */
  static int64_t roundSizeToBlockSize(const int64_t& fileSize,
                                      size_t blockSize);

  /**
   * Remove trailing file separators from a file path.
   *
   * @param path The path.
   * @return The path with trailing file separators removed.
   */
  static String removeTrailingSeparators(const String& path);

  /** The host system's file separator character. */
  static const char separator;

  /**
   * A string containing all valid file separator characters for the host
   * system. On Windows, this string includes both `/' and '\', while on
   * all other systems it is equivalent to File::separator.
   */
  static const char validSeparators[];

  /**
   * Determine if a given character is a valid file separator for the host
   * system.
   */
  static bool isValidSeparator(char c);

  /**
   * Determine if a given character is a valid file separator for the host
   * system.
   */
  static bool isValidSeparator(char16_t c);

  /** The host system's file separator character, as a string. */
  static const char separatorString[];

  /** The host system's end-of-line character sequence. */
  static const char* eol;

 protected:

  String _name;

 private:

  CCXX_COPY_DECLS(File);
};

} // namespace ccxx

#endif // __ccxx_File_hxx
