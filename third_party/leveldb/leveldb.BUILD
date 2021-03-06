package(default_visibility = ["//visibility:public"])

genrule(
    name = "port_config_h",
    srcs = ["@toolbase//third_party/leveldb:gen_compile_configs.py"],
    outs = ["port/port_config.h"],
    cmd = "python3 $< > $@",
)

genrule(
    name = "port_h",
    srcs = ["@toolbase//third_party/leveldb:port.h"],
    outs = ["port/port.h"],
    cmd = "cp $< $@",
)

SRCS = [
    "db/builder.cc",
    "db/builder.h",
    "db/c.cc",
    "db/db_impl.cc",
    "db/db_impl.h",
    "db/db_iter.cc",
    "db/db_iter.h",
    "db/dbformat.cc",
    "db/dbformat.h",
    "db/dumpfile.cc",
    "db/filename.cc",
    "db/filename.h",
    "db/log_format.h",
    "db/log_reader.cc",
    "db/log_reader.h",
    "db/log_writer.cc",
    "db/log_writer.h",
    "db/memtable.cc",
    "db/memtable.h",
    "db/repair.cc",
    "db/skiplist.h",
    "db/snapshot.h",
    "db/table_cache.cc",
    "db/table_cache.h",
    "db/version_edit.cc",
    "db/version_edit.h",
    "db/version_set.cc",
    "db/version_set.h",
    "db/write_batch_internal.h",
    "db/write_batch.cc",
    "port/port_stdcxx.h",
    "port/thread_annotations.h",
    "table/block_builder.cc",
    "table/block_builder.h",
    "table/block.cc",
    "table/block.h",
    "table/filter_block.cc",
    "table/filter_block.h",
    "table/format.cc",
    "table/format.h",
    "table/iterator_wrapper.h",
    "table/iterator.cc",
    "table/merger.cc",
    "table/merger.h",
    "table/table_builder.cc",
    "table/table.cc",
    "table/two_level_iterator.cc",
    "table/two_level_iterator.h",
    "util/arena.cc",
    "util/arena.h",
    "util/bloom.cc",
    "util/cache.cc",
    "util/coding.cc",
    "util/coding.h",
    "util/comparator.cc",
    "util/crc32c.cc",
    "util/crc32c.h",
    "util/env.cc",
    "util/filter_policy.cc",
    "util/hash.cc",
    "util/hash.h",
    "util/logging.cc",
    "util/logging.h",
    "util/mutexlock.h",
    "util/no_destructor.h",
    "util/options.cc",
    "util/random.h",
    "util/status.cc",
    "include/leveldb/c.h",
    "include/leveldb/cache.h",
    "include/leveldb/comparator.h",
    "include/leveldb/db.h",
    "include/leveldb/dumpfile.h",
    "include/leveldb/env.h",
    "include/leveldb/export.h",
    "include/leveldb/filter_policy.h",
    "include/leveldb/iterator.h",
    "include/leveldb/options.h",
    "include/leveldb/slice.h",
    "include/leveldb/status.h",
    "include/leveldb/table_builder.h",
    "include/leveldb/table.h",
    "include/leveldb/write_batch.h",
    "util/env_posix.cc",
    "util/env_posix_test_helper.h",
    "util/posix_logger.h",
    "helpers/memenv/memenv.cc",
    "helpers/memenv/memenv.h",
]

cc_library(
    name = "leveldb",
    srcs = [file for file in SRCS if file.endswith(".cc")],
    hdrs = [file for file in SRCS if file.endswith(".h")] + [
        ":port_h",
        ":port_config_h",
    ],
    includes = [
        "include",
    ],
    deps = [
        "@com_google_crc32c//:crc32c",
        "@com_google_snappy//:snappy",
    ],
    linkopts = ["-lpthread"],
)
