#ifndef TOOLBASE_UTILS_STATUS_MACROS_H_
#define TOOLBASE_UTILS_STATUS_MACROS_H_

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace status_macros_internal {
inline const ::absl::Status& GetStatus(const ::absl::Status& status) {
    return status;
}

template <typename T>
inline const ::absl::Status& GetStatus(const ::absl::StatusOr<T>& status) {
    return status.status();
}

}  // namespace status_macros_internal

#define RETURN_IF_ERROR(expr)                        \
    do {                                             \
        const ::absl::Status _status =               \
            status_macros_internal::GetStatus(expr); \
        if (!_status.ok()) return _status;           \
    } while (0)

#define STATUS_MACROS_CONCAT_NAME_INNER(x, y) x##y
#define STATUS_MACROS_CONCAT_NAME(x, y) STATUS_MACROS_CONCAT_NAME_INNER(x, y)

#define ASSIGN_OR_RETURN_IMPL(rvalue, lhs, rexpr)      \
    auto rvalue = (rexpr);                             \
    if (!rvalue.status().ok()) return rvalue.status(); \
    lhs = std::move(rvalue.value())

#define ASSIGN_OR_RETURN(lhs, rexpr) \
    ASSIGN_OR_RETURN_IMPL(           \
        STATUS_MACROS_CONCAT_NAME(_status_or_value, __COUNTER__), lhs, rexpr);

#endif  // TOOLBASE_UTILS_STATUS_MACROS_H_
