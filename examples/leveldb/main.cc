#include <cassert>

#include "absl/strings/string_view.h"
#include "leveldb/db.h"

constexpr absl::string_view DB_PATH = "/tmp/testdb";

int main() {
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    options.error_if_exists = true;
    leveldb::Status status = leveldb::DB::Open(options, DB_PATH.data(), &db);
    assert(status.ok());

    return 0;
}
