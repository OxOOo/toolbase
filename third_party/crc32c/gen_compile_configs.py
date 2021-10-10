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
// Copyright 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef CRC32C_CRC32C_CONFIG_H_
#define CRC32C_CRC32C_CONFIG_H_

// Define to 1 if building for a big-endian platform.
#cmakedefine01 BYTE_ORDER_BIG_ENDIAN

// Define to 1 if the compiler has the __builtin_prefetch intrinsic.
#cmakedefine01 HAVE_BUILTIN_PREFETCH

// Define to 1 if targeting X86 and the compiler has the _mm_prefetch intrinsic.
#cmakedefine01 HAVE_MM_PREFETCH

// Define to 1 if targeting X86 and the compiler has the _mm_crc32_u{8,32,64}
// intrinsics.
#cmakedefine01 HAVE_SSE42

// Define to 1 if targeting ARM and the compiler has the __crc32c{b,h,w,d} and
// the vmull_p64 intrinsics.
#cmakedefine01 HAVE_ARM64_CRC32C

// Define to 1 if the system libraries have the getauxval function in the
// <sys/auxv.h> header. Should be true on Linux and Android API level 20+.
#cmakedefine01 HAVE_STRONG_GETAUXVAL

// Define to 1 if the compiler supports defining getauxval as a weak symbol.
// Should be true for any compiler that supports __attribute__((weak)).
#cmakedefine01 HAVE_WEAK_GETAUXVAL

// Define to 1 if CRC32C tests have been built with Google Logging.
#cmakedefine01 CRC32C_TESTS_BUILT_WITH_GLOG

#endif  // CRC32C_CRC32C_CONFIG_H_
"""


def define(name, value):
    global TEMPLATE
    if value:
        TEMPLATE = TEMPLATE.replace(
            f'#cmakedefine01 {name}', f'#define {name} 1')
    else:
        TEMPLATE = TEMPLATE.replace(
            f'#cmakedefine01 {name}', f'#define {name} 0')


define('BYTE_ORDER_BIG_ENDIAN', sys.byteorder != 'little')

define('HAVE_BUILTIN_PREFETCH', check_cxx_source_compiles(
    """
int main() {
  char data = 0;
  const char* address = &data;
  __builtin_prefetch(address, 0, 0);
  return 0;
}
"""
))

define('HAVE_MM_PREFETCH', check_cxx_source_compiles(
    """
#if defined(_MSC_VER)
#include <intrin.h>
#else  // !defined(_MSC_VER)
#include <xmmintrin.h>
#endif  // defined(_MSC_VER)
int main() {
  char data = 0;
  const char* address = &data;
  _mm_prefetch(address, _MM_HINT_NTA);
  return 0;
}
"""
))

define('HAVE_SSE42', check_cxx_source_compiles(
    """
#if defined(_MSC_VER)
#include <intrin.h>
#else  // !defined(_MSC_VER)
#include <cpuid.h>
#include <nmmintrin.h>
#endif  // defined(_MSC_VER)
int main() {
  _mm_crc32_u8(0, 0); _mm_crc32_u32(0, 0);
#if defined(_M_X64) || defined(__x86_64__)
   _mm_crc32_u64(0, 0);
#endif // defined(_M_X64) || defined(__x86_64__)
  return 0;
}
"""
))

define('HAVE_ARM64_CRC32C', check_cxx_source_compiles(
    """
#include <arm_acle.h>
#include <arm_neon.h>
int main() {
  __crc32cb(0, 0); __crc32ch(0, 0); __crc32cw(0, 0); __crc32cd(0, 0);
  vmull_p64(0, 0);
  return 0;
}
"""
))

define('HAVE_STRONG_GETAUXVAL', check_cxx_source_compiles(
    """
#include <arm_acle.h>
#include <arm_neon.h>
#include <sys/auxv.h>
int main() {
  getauxval(AT_HWCAP);
  return 0;
}
"""
))

define('HAVE_WEAK_GETAUXVAL', check_cxx_source_compiles(
    """
unsigned long getauxval(unsigned long type) __attribute__((weak));
#define AT_HWCAP 16
int main() {
  getauxval(AT_HWCAP);
  return 0;
}
"""
))

define('CRC32C_TESTS_BUILT_WITH_GLOG', False)

print(TEMPLATE)
