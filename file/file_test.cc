#include "file/file.h"

#include "file/filesystem.h"
#include "gtest/gtest.h"
#include "utils/testing.h"

namespace file {
namespace {

using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::utils::testing::IsOkAndHolds;
using ::utils::testing::StatusIs;

static constexpr absl::string_view kFile = "/tmp/test_file";

class File : public ::testing::Test {
   protected:
    void SetUp() override {
        if (*Exists(kFile)) {
            ASSERT_OK(Unlink(kFile));
        }
        ASSERT_THAT(Exists(kFile), IsOkAndHolds(false));
    }
    void TearDown() override {
        if (*Exists(kFile)) {
            ASSERT_OK(Unlink(kFile));
        }
    }
};

TEST_F(File, TestAutoClose) {
    int fd = -1;
    {
        auto file = Open(kFile, O_WRONLY | O_CREAT, 0644);
        EXPECT_OK(file);

        fd = (*file)->fd();
        EXPECT_EQ(write(fd, "hello world", 11), 11);
    }
    EXPECT_EQ(write(fd, "hello world", 11), -1);
    EXPECT_EQ(errno, EBADF);
}

TEST_F(File, ReadNotExists) {
    EXPECT_THAT(Open(kFile, O_RDONLY), StatusIs(absl::StatusCode::kInternal));
}

TEST_F(File, ReadWrite) {
    {
        auto file = Open(kFile, O_WRONLY | O_CREAT, 0644);
        EXPECT_OK(file);
        EXPECT_THAT((*file)->Write("hello world"), IsOkAndHolds(11));
    }
    {
        auto file = Open(kFile, O_RDONLY);
        EXPECT_OK(file);
        EXPECT_THAT((*file)->Read(1024), IsOkAndHolds("hello world"));
    }
}

TEST_F(File, PReadWrite) {
    {
        auto file = Open(kFile, O_WRONLY | O_CREAT, 0644);
        EXPECT_OK(file);
        EXPECT_THAT((*file)->PWrite("hello world", 0), IsOkAndHolds(11));
        EXPECT_THAT((*file)->PWrite("hello world", 0), IsOkAndHolds(11));
    }
    {
        auto file = Open(kFile, O_RDONLY);
        EXPECT_OK(file);
        EXPECT_THAT((*file)->PRead(1024, 0), IsOkAndHolds("hello world"));
        EXPECT_THAT((*file)->PRead(1024, 0), IsOkAndHolds("hello world"));
    }
}

TEST_F(File, Seek) {
    {
        auto file = Open(kFile, O_WRONLY | O_CREAT, 0644);
        EXPECT_OK(file);
        EXPECT_THAT((*file)->Write("hello world"), IsOkAndHolds(11));
    }
    {
        auto file = Open(kFile, O_RDONLY);
        EXPECT_OK(file);
        EXPECT_THAT((*file)->Read(1024), IsOkAndHolds("hello world"));
        EXPECT_THAT((*file)->LSeek(0, SEEK_SET), IsOkAndHolds(0));
        EXPECT_THAT((*file)->Read(1024), IsOkAndHolds("hello world"));
    }
}

}  // namespace
}  // namespace file
