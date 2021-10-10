# encoding: utf-8

import sys
import os
import tempfile
import subprocess


def check_cxx_source_compiles(code):
    [fd, name] = tempfile.mkstemp(suffix='.cc')
    os.write(fd, code.encode())
    os.close(fd)
    try:
        p = subprocess.Popen(['g++', name, '-o', '/dev/null'],
                             stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        p.wait(10)
        return p.poll() == 0
    except Exception as e:
        return False
    finally:
        os.unlink(name)


TEMPLATE = """
// Copyright 2017 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_PORT_PORT_CONFIG_H_
#define STORAGE_LEVELDB_PORT_PORT_CONFIG_H_

// Define to 1 if you have a definition for fdatasync() in <unistd.h>.
#if !defined(HAVE_FDATASYNC)
#cmakedefine01 HAVE_FDATASYNC
#endif  // !defined(HAVE_FDATASYNC)

// Define to 1 if you have a definition for F_FULLFSYNC in <fcntl.h>.
#if !defined(HAVE_FULLFSYNC)
#cmakedefine01 HAVE_FULLFSYNC
#endif  // !defined(HAVE_FULLFSYNC)

// Define to 1 if you have a definition for O_CLOEXEC in <fcntl.h>.
#if !defined(HAVE_O_CLOEXEC)
#cmakedefine01 HAVE_O_CLOEXEC
#endif  // !defined(HAVE_O_CLOEXEC)

// Define to 1 if you have Google CRC32C.
#if !defined(HAVE_CRC32C)
#cmakedefine01 HAVE_CRC32C
#endif  // !defined(HAVE_CRC32C)

// Define to 1 if you have Google Snappy.
#if !defined(HAVE_SNAPPY)
#cmakedefine01 HAVE_SNAPPY
#endif  // !defined(HAVE_SNAPPY)

#endif  // STORAGE_LEVELDB_PORT_PORT_CONFIG_H_
"""


def define(name, value):
    global TEMPLATE
    if value:
        TEMPLATE = TEMPLATE.replace(
            f'#cmakedefine01 {name}', f'#define {name} 1')
    else:
        TEMPLATE = TEMPLATE.replace(
            f'#cmakedefine01 {name}', f'#define {name} 0')


define('HAVE_CRC32C', True)
define('HAVE_SNAPPY', True)
define('HAVE_TCMALLOC', True)

define('HAVE_FDATASYNC', True)
define('HAVE_FULLFSYNC', False)
define('HAVE_O_CLOEXEC', True)

define('HAVE_CXX17_HAS_INCLUDE', check_cxx_source_compiles(
    """
#if defined(__has_include) &&  __has_include(<string>)
#include <string>
#endif
int main() { std::string str; return 0; }
"""))

print(TEMPLATE)
