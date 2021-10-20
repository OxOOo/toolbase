#include "file/filesystem.h"

#include <errno.h>
#include <unistd.h>

namespace file {

absl::StatusOr<bool> Exists(absl::string_view path) {
    std::string path_str = std::string(path);
    if (access(path_str.c_str(), F_OK) == 0) {
        return true;
    } else if (errno == ENOENT) {
        return false;
    } else {
        return absl::InternalError(strerror(errno));
    }
}

}  // namespace file
