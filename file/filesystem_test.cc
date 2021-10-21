#include "file/filesystem.h"

#include "file/path.h"
#include "gtest/gtest.h"
#include "utils/testing.h"

namespace file {
namespace {

using ::testing::Contains;
using ::testing::ElementsAre;
using ::testing::IsEmpty;
using ::utils::testing::IsOkAndHolds;
using ::utils::testing::StatusIs;

TEST(Stat, FileNotExists) {
    EXPECT_THAT(Stat("/notexists"), StatusIs(absl::StatusCode::kInternal));
}

TEST(Stat, TestFile) {
    auto result = Stat("/etc/passwd");
    EXPECT_OK(result);
    EXPECT_TRUE(result->IsFile());
}

TEST(Stat, TestDirectory) {
    auto result = Stat("/etc");
    EXPECT_OK(result);
    EXPECT_TRUE(result->IsDirectory());
}

TEST(Stat, TestFilePermossion) {
    auto result = Stat("/etc/passwd");
    EXPECT_OK(result);
    EXPECT_EQ(result->mode() & (S_IRUSR | S_IWUSR), S_IRUSR | S_IWUSR);
    EXPECT_EQ(result->mode() & S_IROTH, S_IROTH);
    EXPECT_EQ(result->mode() & S_IWOTH, 0);
}

TEST(LStat, FileNotExists) {
    EXPECT_THAT(LStat("/notexists"), StatusIs(absl::StatusCode::kInternal));
}

TEST(LStat, TestFile) {
    auto result = LStat("/etc/passwd");
    EXPECT_OK(result);
    EXPECT_TRUE(result->IsFile());
}

TEST(LStat, TestDirectory) {
    auto result = LStat("/etc");
    EXPECT_OK(result);
    EXPECT_TRUE(result->IsDirectory());
}

TEST(LStat, TestFilePermossion) {
    auto result = LStat("/etc/passwd");
    EXPECT_OK(result);
    EXPECT_EQ(result->mode() & (S_IRUSR | S_IWUSR), S_IRUSR | S_IWUSR);
    EXPECT_EQ(result->mode() & S_IROTH, S_IROTH);
    EXPECT_EQ(result->mode() & S_IWOTH, 0);
}

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

TEST(CreateFile, TestCreateFile) {
    static constexpr absl::string_view kFile = "/tmp/test_creat";
    if (*Exists(kFile)) {
        ASSERT_OK(Unlink(kFile));
    }
    EXPECT_OK(CreateFile(kFile, S_IRWXU));
    EXPECT_THAT(CreateFile(kFile, S_IRWXU),
                StatusIs(absl::StatusCode::kInternal));
    EXPECT_THAT(Exists(kFile), IsOkAndHolds(true));
    EXPECT_EQ(Stat(kFile)->mode() & S_IRWXU, S_IRWXU);
    EXPECT_EQ(Stat(kFile)->mode() & S_IRWXG, 0);
    EXPECT_EQ(Stat(kFile)->mode() & S_IRWXO, 0);
    EXPECT_OK(Unlink(kFile));
}

TEST(Chmod, FileNotExists) {
    EXPECT_THAT(Chmod("/notexists", S_IRUSR | S_IWUSR),
                StatusIs(absl::StatusCode::kInternal));
}

TEST(Chmod, TestChmod) {
    static constexpr absl::string_view kFile = "/tmp/test_chmod";
    if (*Exists(kFile)) {
        ASSERT_OK(Unlink(kFile));
    }
    EXPECT_OK(CreateFile(kFile, S_IRWXU));
    EXPECT_EQ(Stat(kFile)->mode() & S_IRWXU, S_IRWXU);
    EXPECT_OK(Chmod(kFile, S_IRUSR | S_IWUSR));
    EXPECT_EQ(Stat(kFile)->mode() & S_IRWXU, S_IRUSR | S_IWUSR);
    EXPECT_OK(Unlink(kFile));
}

TEST(Unlink, Unlink) {
    static constexpr absl::string_view kFile = "/tmp/test_unlink";
    if (*Exists(kFile)) {
        ASSERT_OK(Unlink(kFile));
    }
    EXPECT_THAT(Exists(kFile), IsOkAndHolds(false));
    EXPECT_OK(CreateFile(kFile, 0644));
    EXPECT_THAT(Exists(kFile), IsOkAndHolds(true));
    EXPECT_OK(Unlink(kFile));
    EXPECT_THAT(Exists(kFile), IsOkAndHolds(false));
}

TEST(RmdirAndMkdir, RmdirAndMkdir) {
    static constexpr absl::string_view kPath = "/tmp/test_rmdir_and_mkdir";
    if (*Exists(kPath)) {
        ASSERT_OK(Rmdir(kPath));
    }
    EXPECT_THAT(Exists(kPath), IsOkAndHolds(false));
    EXPECT_OK(Mkdir(kPath, 0644));
    EXPECT_THAT(Exists(kPath), IsOkAndHolds(true));
    EXPECT_OK(Rmdir(kPath));
    EXPECT_THAT(Exists(kPath), IsOkAndHolds(false));
}

TEST(Rename, Rename) {
    static constexpr absl::string_view kOldPath = "/tmp/test_rename_old";
    static constexpr absl::string_view kNewPath = "/tmp/test_rename_new";
    if (*Exists(kOldPath)) {
        ASSERT_OK(Unlink(kOldPath));
    }
    if (*Exists(kNewPath)) {
        ASSERT_OK(Unlink(kNewPath));
    }
    EXPECT_OK(CreateFile(kOldPath, 0644));
    EXPECT_THAT(Exists(kOldPath), IsOkAndHolds(true));
    EXPECT_OK(Rename(kOldPath, kNewPath));
    EXPECT_THAT(Exists(kOldPath), IsOkAndHolds(false));
    EXPECT_THAT(Exists(kNewPath), IsOkAndHolds(true));
    EXPECT_OK(Unlink(kNewPath));
}

TEST(Rename, RenameNotExistsFile) {
    EXPECT_THAT(Rename("/notexists", "/notexists_new"),
                StatusIs(absl::StatusCode::kInternal));
}

TEST(Rename, RenameToExistsFile) {
    static constexpr absl::string_view kOldPath = "/tmp/test_rename_old";
    static constexpr absl::string_view kNewPath = "/tmp/test_rename_new";
    if (*Exists(kOldPath)) {
        ASSERT_OK(Unlink(kOldPath));
    }
    if (*Exists(kNewPath)) {
        ASSERT_OK(Unlink(kNewPath));
    }
    EXPECT_OK(CreateFile(kOldPath, 0644));
    EXPECT_OK(CreateFile(kNewPath, 0644));
    EXPECT_OK(Rename(kOldPath, kNewPath));
    EXPECT_THAT(Exists(kOldPath), IsOkAndHolds(false));
    EXPECT_THAT(Exists(kNewPath), IsOkAndHolds(true));
    EXPECT_OK(Unlink(kNewPath));
}

TEST(ListDirectory, ListETC) {
    EXPECT_THAT(ListDirectory("/etc"), IsOkAndHolds(Contains("passwd")));
}

TEST(ListDirectory, ListDirectory) {
    static constexpr absl::string_view kPath = "/tmp/test_list_dir";
    if (*Exists(kPath)) {
        ASSERT_OK(RmTree(kPath));
    }
    ASSERT_THAT(Exists(kPath), IsOkAndHolds(false));
    EXPECT_OK(Mkdir(kPath, 0744));
    EXPECT_THAT(ListDirectory(kPath), IsOkAndHolds(IsEmpty()));
    EXPECT_OK(CreateFile(PathJoin(kPath, "a"), 0644));
    EXPECT_THAT(ListDirectory(kPath), IsOkAndHolds(ElementsAre("a")));
    EXPECT_OK(CreateFile(PathJoin(kPath, "b"), 0644));
    EXPECT_THAT(ListDirectory(kPath), IsOkAndHolds(ElementsAre("a", "b")));
    ASSERT_OK(RmTree(kPath));
    ASSERT_THAT(Exists(kPath), IsOkAndHolds(false));
}

TEST(ListDirectory, PathNotExists) {
    EXPECT_THAT(ListDirectory("/notexists"),
                StatusIs(absl::StatusCode::kInternal));
}

TEST(RmTree, PathNotExists) { EXPECT_OK(RmTree("/notexists")); }

TEST(RmTree, RmTree) {
    static constexpr absl::string_view kPath = "/tmp/test_rm_tree";
    if (*Exists(kPath)) {
        ASSERT_OK(RmTree(kPath));
    }
    ASSERT_THAT(Exists(kPath), IsOkAndHolds(false));
    ASSERT_OK(Mkdir(kPath, 0744));
    ASSERT_OK(CreateFile(PathJoin(kPath, "a"), 0644));
    ASSERT_OK(CreateFile(PathJoin(kPath, "b"), 0644));
    EXPECT_OK(RmTree(kPath));
    EXPECT_THAT(Exists(kPath), IsOkAndHolds(false));
}

TEST(GetPutContents, GetPutContents) {
    static constexpr absl::string_view kFile = "/tmp/test_get_put_contents";
    if (*Exists(kFile)) {
        ASSERT_OK(Unlink(kFile));
    }
    EXPECT_OK(PutContents("hello world", kFile));
    EXPECT_THAT(GetContents(kFile), IsOkAndHolds("hello world"));
    EXPECT_OK(PutContents("hello world2", kFile));
    EXPECT_THAT(GetContents(kFile), IsOkAndHolds("hello world2"));
    ASSERT_OK(Unlink(kFile));
}

TEST(GetContents, FileNotExists) {
    EXPECT_THAT(GetContents("/notexists"),
                StatusIs(absl::StatusCode::kInternal));
}

TEST(PutContents, PathNotExists) {
    EXPECT_THAT(PutContents("data", "/notexists/file"),
                StatusIs(absl::StatusCode::kInternal));
}

}  // namespace
}  // namespace file
