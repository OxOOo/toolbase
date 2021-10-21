#ifndef TOOLBASE_FILE_FILESYSTEM_H_
#define TOOLBASE_FILE_FILESYSTEM_H_

#include <sys/stat.h>

#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

namespace file {

// ref to https://man7.org/linux/man-pages/man7/inode.7.html
class PathStat {
   public:
    explicit PathStat(struct stat stat) : stat_(stat) {}

    bool IsFile() const { return S_ISREG(stat_.st_mode); }
    bool IsDirectory() const { return S_ISDIR(stat_.st_mode); }
    bool IsBlock() const { return S_ISBLK(stat_.st_mode); }
    bool IsFIFO() const { return S_ISFIFO(stat_.st_mode); }
    bool IsLink() const { return S_ISLNK(stat_.st_mode); }
    bool IsSocket() const { return S_ISSOCK(stat_.st_mode); }
    mode_t Mode() const { return stat_.st_mode & 07777; }
    size_t Size() const { return stat_.st_size; }

   private:
    const struct stat stat_;
};

// Finds the stat of the `path`.
absl::StatusOr<PathStat> Stat(absl::string_view path);
absl::StatusOr<PathStat> LStat(absl::string_view path);

absl::StatusOr<bool> Exists(absl::string_view path);

absl::Status CreateFile(absl::string_view path, mode_t mode);

absl::Status Chmod(absl::string_view path, mode_t mode);

absl::Status Unlink(absl::string_view path);

absl::Status Rmdir(absl::string_view path);

absl::Status Mkdir(absl::string_view path, mode_t mode);

absl::Status Rename(absl::string_view oldpath, absl::string_view newpath);

// Returns the file or directory names of the `path`, the result will not
// contain '.' and '..'.
absl::StatusOr<std::vector<std::string>> ListDirectory(absl::string_view path);

// Removes all the sub items of `path`. Will return ok if `path` not exists.
absl::Status RmTree(absl::string_view path);

}  // namespace file

#endif  // TOOLBASE_FILE_FILESYSTEM_H_