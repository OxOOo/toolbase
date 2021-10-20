#include "file/path.h"

#include "gtest/gtest.h"

namespace file {
namespace {

TEST(PathJoin, PathJoin) {
    EXPECT_EQ(PathJoin(), "");
    EXPECT_EQ(PathJoin("a", "b", "c"), "a/b/c");
    EXPECT_EQ(PathJoin("/root", "b", "c"), "/root/b/c");
    EXPECT_EQ(PathJoin("/root", "/path"), "/root/path");
    EXPECT_EQ(PathJoin("a", "/path"), "a/path");
    EXPECT_EQ(PathJoin("a/", "b/"), "a/b");
    EXPECT_EQ(PathJoin("a/", "/"), "a");
    EXPECT_EQ(PathJoin("a/", "/", "b"), "a/b");
}

TEST(PathJoinRespectAbsolute, PathJoinRespectAbsolute) {
    EXPECT_EQ(PathJoinRespectAbsolute(), "");
    EXPECT_EQ(PathJoinRespectAbsolute("a", "b", "c"), "a/b/c");
    EXPECT_EQ(PathJoinRespectAbsolute("/root", "b", "c"), "/root/b/c");
    EXPECT_EQ(PathJoinRespectAbsolute("/root", "/path"), "/path");
    EXPECT_EQ(PathJoinRespectAbsolute("a", "/path"), "/path");
    EXPECT_EQ(PathJoinRespectAbsolute("a/", "b/"), "a/b");
    EXPECT_EQ(PathJoinRespectAbsolute("a/", "/"), "/");
    EXPECT_EQ(PathJoinRespectAbsolute("a/", "/", "b"), "/b");
}

}  // namespace
}  // namespace file
