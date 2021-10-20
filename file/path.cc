#include "file/path.h"

#include "absl/strings/str_cat.h"

namespace file {
namespace file_internal {

constexpr char kPathSeparator = '/';
constexpr absl::string_view kPathSeparatorView = "/";

std::string PathJoin(std::initializer_list<absl::string_view> pieces) {
    std::string result;
    for (absl::string_view piece : pieces) {
        while (piece.length() > 1 && *piece.rbegin() == kPathSeparator) {
            piece.remove_suffix(1);
        }

        if (result.empty()) {
            result = std::string(piece);
        } else {
            while (!piece.empty() && *piece.begin() == kPathSeparator) {
                piece.remove_prefix(1);
            }
            if (!piece.empty()) {
                if (*result.rbegin() == kPathSeparator) {
                    absl::StrAppend(&result, piece);
                } else {
                    absl::StrAppend(&result, kPathSeparatorView, piece);
                }
            }
        }
    }
    return result;
}

std::string PathJoinRespectAbsolute(
    std::initializer_list<absl::string_view> pieces) {
    std::string result;
    for (absl::string_view piece : pieces) {
        while (piece.length() > 1 && *piece.rbegin() == kPathSeparator) {
            piece.remove_suffix(1);
        }

        if (result.empty()) {
            result = std::string(piece);
        } else if (!piece.empty()) {
            if (*piece.begin() == kPathSeparator) {
                result = std::string(piece);
            } else {
                if (*result.rbegin() == kPathSeparator) {
                    absl::StrAppend(&result, piece);
                } else {
                    absl::StrAppend(&result, kPathSeparatorView, piece);
                }
            }
        }
    }
    return result;
}

}  // namespace file_internal
}  // namespace file