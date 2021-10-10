# encoding: utf-8

import sys
import os
import tempfile
import subprocess

"""
python3 gen_compile_configs.py path/to/config.h path/to/snappy-stubs-public.h
"""

if len(sys.argv) <= 2:
    print('need more args')
    sys.exit(1)

def check_include_file(include_file):
    code = f"""
    #include <{include_file}>
    int main() {{ return 0; }}
    """
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


CONFIG_TEMPLATE = """
#ifndef THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_
#define THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_

/* Define to 1 if the compiler supports __attribute__((always_inline)). */
#cmakedefine HAVE_ATTRIBUTE_ALWAYS_INLINE 1

/* Define to 1 if the compiler supports __builtin_ctz and friends. */
#cmakedefine HAVE_BUILTIN_CTZ 1

/* Define to 1 if the compiler supports __builtin_expect. */
#cmakedefine HAVE_BUILTIN_EXPECT 1

/* Define to 1 if you have a definition for mmap() in <sys/mman.h>. */
#cmakedefine HAVE_FUNC_MMAP 1

/* Define to 1 if you have a definition for sysconf() in <unistd.h>. */
#cmakedefine HAVE_FUNC_SYSCONF 1

/* Define to 1 if you have the `lzo2' library (-llzo2). */
#cmakedefine HAVE_LIBLZO2 1

/* Define to 1 if you have the `z' library (-lz). */
#cmakedefine HAVE_LIBZ 1

/* Define to 1 if you have the `lz4' library (-llz4). */
#cmakedefine HAVE_LIBLZ4 1

/* Define to 1 if you have the <sys/mman.h> header file. */
#cmakedefine HAVE_SYS_MMAN_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#cmakedefine HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/uio.h> header file. */
#cmakedefine HAVE_SYS_UIO_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Define to 1 if you have the <windows.h> header file. */
#cmakedefine HAVE_WINDOWS_H 1

/* Define to 1 if you target processors with SSSE3+ and have <tmmintrin.h>. */
#cmakedefine01 SNAPPY_HAVE_SSSE3

/* Define to 1 if you target processors with BMI2+ and have <bmi2intrin.h>. */
#cmakedefine01 SNAPPY_HAVE_BMI2

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#cmakedefine SNAPPY_IS_BIG_ENDIAN 1

#endif  // THIRD_PARTY_SNAPPY_OPENSOURCE_CMAKE_CONFIG_H_
"""

STUBS_TEMPLATE = """
#ifndef THIRD_PARTY_SNAPPY_OPENSOURCE_SNAPPY_STUBS_PUBLIC_H_
#define THIRD_PARTY_SNAPPY_OPENSOURCE_SNAPPY_STUBS_PUBLIC_H_

#include <cstddef>

#if ${HAVE_SYS_UIO_H_01}  // HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif  // HAVE_SYS_UIO_H

#define SNAPPY_MAJOR ${PROJECT_VERSION_MAJOR}
#define SNAPPY_MINOR ${PROJECT_VERSION_MINOR}
#define SNAPPY_PATCHLEVEL ${PROJECT_VERSION_PATCH}
#define SNAPPY_VERSION \
    ((SNAPPY_MAJOR << 16) | (SNAPPY_MINOR << 8) | SNAPPY_PATCHLEVEL)

namespace snappy {

#if !${HAVE_SYS_UIO_H_01}  // !HAVE_SYS_UIO_H
// Windows does not have an iovec type, yet the concept is universally useful.
// It is simple to define it ourselves, so we put it inside our own namespace.
struct iovec {
  void* iov_base;
  size_t iov_len;
};
#endif  // !HAVE_SYS_UIO_H

}  // namespace snappy

#endif  // THIRD_PARTY_SNAPPY_OPENSOURCE_SNAPPY_STUBS_PUBLIC_H_
"""


def define(name, value):
    global CONFIG_TEMPLATE
    global STUBS_TEMPLATE
    if value:
        CONFIG_TEMPLATE = CONFIG_TEMPLATE.replace(
            f'#cmakedefine01 {name}', f'#define {name} 1')
        CONFIG_TEMPLATE = CONFIG_TEMPLATE.replace(
            f'#cmakedefine {name} 1', f'#define {name} 1')
        STUBS_TEMPLATE = STUBS_TEMPLATE.replace(f'${{{name}}}', '1')
    else:
        CONFIG_TEMPLATE = CONFIG_TEMPLATE.replace(
            f'#cmakedefine01 {name}', f'#define {name} 0')
        CONFIG_TEMPLATE = CONFIG_TEMPLATE.replace(
            f'#cmakedefine {name} 1', f'/* #undef {name} */')
        STUBS_TEMPLATE = STUBS_TEMPLATE.replace(f'${{{name}}}', '0')
    return value


define('SNAPPY_IS_BIG_ENDIAN', sys.byteorder != 'little')

define('HAVE_SYS_MMAN_H', check_include_file("sys/mman.h"))
define('HAVE_SYS_RESOURCE_H', check_include_file("sys/resource.h"))
define('HAVE_SYS_TIME_H', check_include_file("sys/time.h"))
HAVE_SYS_UIO_H = define('HAVE_SYS_UIO_H', check_include_file("sys/uio.h"))
define('HAVE_UNISTD_H', check_include_file("unistd.h"))
define('HAVE_WINDOWS_H', check_include_file("windows.h"))

define('HAVE_LIBZ', True)
define('HAVE_LIBLZO2', False)
define('HAVE_LIBLZ4', True)

define('HAVE_BUILTIN_EXPECT', check_cxx_source_compiles(
    """
int main() {
  return __builtin_expect(0, 1);
}
"""))

define('HAVE_BUILTIN_CTZ', check_cxx_source_compiles(
    """
int main() {
  return __builtin_ctzll(0);
}
"""))

define('HAVE_ATTRIBUTE_ALWAYS_INLINE', check_cxx_source_compiles(
    """
__attribute__((always_inline)) int zero() { return 0; }
int main() {
  return zero();
}
"""))

define('SNAPPY_HAVE_SSSE3', check_cxx_source_compiles(
    """
#include <tmmintrin.h>
int main() {
  const __m128i *src = 0;
  __m128i dest;
  const __m128i shuffle_mask = _mm_load_si128(src);
  const __m128i pattern = _mm_shuffle_epi8(_mm_loadl_epi64(src), shuffle_mask);
  _mm_storeu_si128(&dest, pattern);
  return 0;
}
"""))

define('SNAPPY_HAVE_BMI2', check_cxx_source_compiles(
    """
#include <immintrin.h>
int main() {
  return _bzhi_u32(0, 1);
}
"""))

define('HAVE_FUNC_MMAP', True)
define('HAVE_FUNC_SYSCONF', True)

define('HAVE_SYS_UIO_H_01', HAVE_SYS_UIO_H)

with open(sys.argv[1], 'w') as fd:
    fd.write(CONFIG_TEMPLATE)

with open(sys.argv[2], 'w') as fd:
    fd.write(STUBS_TEMPLATE)
