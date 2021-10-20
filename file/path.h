#ifndef TOOLBASE_FILE_PATH_H_
#define TOOLBASE_FILE_PATH_H_

#include <string>

#include "absl/strings/string_view.h"

namespace file {
namespace file_internal {

// Do not call directly - this is not part of the public API.
std::string PathJoin(std::initializer_list<absl::string_view> pieces);
std::string PathJoinRespectAbsolute(
    std::initializer_list<absl::string_view> pieces);

}  // namespace file_internal

// -----------------------------------------------------------------------------
// PathJoin()
// -----------------------------------------------------------------------------
// Joins the path componments into a full path.
//  PathJoin("a", "b", "c") == "a/b/c"
//  PathJoin("/root", "b", "c") == "/root/b/c"
//  PathJoin("/root", "/path") == "/root/path"
//  PathJoin("a", "/path") == "a/path"
//  PathJoin("a/", "b/") == "a/b"
//  PathJoin("a/", "/") == "a"
//  PathJoin("a/", "/", "b") == "a/b"

ABSL_MUST_USE_RESULT inline std::string PathJoin() { return std::string(); }

template <typename... AV>
ABSL_MUST_USE_RESULT inline std::string PathJoin(absl::string_view first,
                                                 AV... args) {
    return file_internal::PathJoin({first, args...});
}

// -----------------------------------------------------------------------------
// PathJoinRespectAbsolute()
// -----------------------------------------------------------------------------
// Joins the path componments into a full path.
//  PathJoinRespectAbsolute("a", "b", "c") == "a/b/c"
//  PathJoinRespectAbsolute("/root", "b", "c") == "/root/b/c"
//  PathJoinRespectAbsolute("/root", "/path") == "/path"
//  PathJoinRespectAbsolute("a", "/path") == "/path"
//  PathJoinRespectAbsolute("a/", "b/") == "a/b"
//  PathJoinRespectAbsolute("a/", "/") == "/"
//  PathJoinRespectAbsolute("a/", "/", "b") == "/b"

ABSL_MUST_USE_RESULT inline std::string PathJoinRespectAbsolute() {
    return std::string();
}

template <typename... AV>
ABSL_MUST_USE_RESULT inline std::string PathJoinRespectAbsolute(
    absl::string_view first, AV... args) {
    return file_internal::PathJoinRespectAbsolute({first, args...});
}

}  // namespace file

#endif  // TOOLBASE_FILE_PATH_H_
