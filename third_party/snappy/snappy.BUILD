package(default_visibility = ["//visibility:public"])

genrule(
    name = "config_stubs_h",
    srcs = ["@toolbase//third_party/snappy:gen_compile_configs.py"],
    outs = ["config.h", "snappy-stubs-public.h"],
    cmd = "python3 $< $(OUTS)",
)

SRCS = [
    "snappy-internal.h",
    "snappy-stubs-internal.h",
    "snappy-c.cc",
    "snappy-sinksource.cc",
    "snappy-stubs-internal.cc",
    "snappy.cc",
    "snappy-c.h",
    "snappy-sinksource.h",
    "snappy.h",
]

cc_library(
    name = "snappy",
    srcs = [file for file in SRCS if file.endswith(".cc")],
    hdrs = [file for file in SRCS if file.endswith(".h")] + [":config_stubs_h"],
)
