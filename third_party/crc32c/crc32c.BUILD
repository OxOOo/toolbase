package(default_visibility = ["//visibility:public"])

genrule(
    name = "crc32c_config_h",
    srcs = ["@toolbase//third_party/crc32c:gen_compile_configs.py"],
    outs = ["include/crc32c/crc32c_config.h"],
    cmd = "python3 $< > $@",
)

SRCS = [
    "src/crc32c_arm64.h",
    "src/crc32c_arm64_check.h",
    "src/crc32c_internal.h",
    "src/crc32c_portable.cc",
    "src/crc32c_prefetch.h",
    "src/crc32c_read_le.h",
    "src/crc32c_round_up.h",
    "src/crc32c_sse42.h",
    "src/crc32c_sse42_check.h",
    "src/crc32c.cc",
    "include/crc32c/crc32c.h",
]

cc_library(
    name = "crc32c",
    srcs = [file for file in SRCS if file.endswith(".cc")],
    hdrs = [file for file in SRCS if file.endswith(".h")] + [":crc32c_config_h"],
    includes = ["include"],
)
