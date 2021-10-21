#include "utils/testing.h"

#include "gtest/gtest-spi.h"
#include "gtest/gtest.h"

namespace utils {
namespace testing {
namespace {

using ::testing::Eq;

TEST(IsOk, IsOkForOkStatus) {
    auto ok = absl::OkStatus();
    EXPECT_OK(ok);
    auto ok_value = absl::StatusOr<int>(1024);
    EXPECT_OK(ok_value);
}

TEST(IsOkAndHolds, IsOkAndHolds) {
    auto ok = absl::StatusOr<int>(1024);
    EXPECT_THAT(ok, IsOkAndHolds(Eq(1024)));
}

TEST(StatusIs, StatusIs) {
    EXPECT_THAT(absl::OkStatus(), StatusIs(absl::StatusCode::kOk));
    EXPECT_THAT(absl::InternalError("error"),
                StatusIs(absl::StatusCode::kInternal));
}

}  // namespace
}  // namespace testing
}  // namespace utils