package(default_visibility = ["//visibility:public"])

cc_library(
    name = "path",
    srcs = ["path.cc"],
    hdrs = ["path.h"],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "path_test",
    srcs = ["path_test.cc"],
    deps = [
        ":path",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "filesystem",
    srcs = ["filesystem.cc"],
    hdrs = ["filesystem.h"],
    deps = [
        ":file",
        ":path",
        "//utils:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "filesystem_test",
    srcs = ["filesystem_test.cc"],
    deps = [
        ":filesystem",
        ":path",
        "//utils:testing",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "file",
    srcs = ["file.cc"],
    hdrs = ["file.h"],
    deps = [
        "//utils:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_glog//:glog",
    ],
)
