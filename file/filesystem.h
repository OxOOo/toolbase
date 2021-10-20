#ifndef TOOLBASE_FILE_FILESYSTEM_H_
#define TOOLBASE_FILE_FILESYSTEM_H_

#include <sys/stat.h>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

namespace file {

class Stat {
    explicit Stat(struct stat stat) : stat(stat) {}

   private:
    const struct stat stat;
};

absl::StatusOr<bool> Exists(absl::string_view path);

}  // namespace file

#endif  // TOOLBASE_FILE_FILESYSTEM_H_