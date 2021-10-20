#include "file/filesystem.h"

#include "gtest/gtest.h"
#include "utils/testing.h"

namespace file {
namespace {

using ::utils::testing::IsOkAndHolds;

TEST(Exists, FileExists) {
    EXPECT_THAT(Exists("/etc/passwd"), IsOkAndHolds(true));
}

TEST(Exists, DirExists) { EXPECT_THAT(Exists("/etc"), IsOkAndHolds(true)); }

TEST(Exists, FileNotExists) {
    EXPECT_THAT(Exists("/etc/passwd2"), IsOkAndHolds(false));
}

TEST(Exists, DirNotExists) {
    EXPECT_THAT(Exists("/etc2"), IsOkAndHolds(false));
}

}  // namespace
}  // namespace file
