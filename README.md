> [!IMPORTANT]
> This project is currently only developer stable
>

<div style="text-align:center">
    <h1>Ultimate WebAssembly Virtual Machine</h1>
    <a href="LICENSE.md">
        <img src="https://img.shields.io/badge/License-Apache%202.0-green.svg" , alt="License" />
    </a>
    <a href="https://en.cppreference.com">
        <img src="https://img.shields.io/badge/language-c++26-blue.svg" ,alt="cppreference" />
    </a>
</div>

## Introduce
Ultimate WebAssembly Virtual Machine (Version 2), the abbreviation is uwvm2.

## Features
### WASM Feature Extensions
| Standardized features                                                                                                                              |     parameters                       |
|----------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------|
| [MVP](https://github.com/WebAssembly/design/blob/main/BinaryEncoding.md)                                                                           |  V1.0.0                              |
| [JS BigInt to Wasm i64 integration](https://github.com/WebAssembly/JS-BigInt-integration)                                                          |  N/A                                 |
| [Bulk memory operations](https://github.com/WebAssembly/bulk-memory-operations/blob/master/proposals/bulk-memory-operations/Overview.md)           |  V1.0.1                              |
| [Extended constant expressions](https://github.com/WebAssembly/extended-const/blob/master/proposals/extended-const/Overview.md)                    |  :x:                                 |
| [Garbage collection](https://github.com/WebAssembly/gc)                                                                                            |  :x:                                 |
| [Multiple memories](https://github.com/WebAssembly/multi-memory/blob/master/proposals/multi-memory/Overview.md)                                    |  ```--enable-multi-memory```         |
| [Multi-value](https://github.com/WebAssembly/spec/blob/master/proposals/multi-value/Overview.md)                                                   |  V1.0.1                              |
| [Mutable globals](https://github.com/WebAssembly/mutable-global/blob/master/proposals/mutable-global/Overview.md)                                  |  V1.0.1                              |
| [Reference types](https://github.com/WebAssembly/reference-types/blob/master/proposals/reference-types/Overview.md)                                |  V1.0.1                              |
| [Relaxed SIMD](https://github.com/WebAssembly/relaxed-simd/tree/main/proposals/relaxed-simd)                                                       |  ```--enable-relaxed-simd```         |
| [Non-trapping float-to-int conversions](https://github.com/WebAssembly/spec/blob/master/proposals/nontrapping-float-to-int-conversion/Overview.md) |  V1.0.1                              |
| [Sign-extension operations](https://github.com/WebAssembly/spec/blob/master/proposals/sign-extension-ops/Overview.md)                              |  :x:                                 |
| [Fixed-width SIMD](https://github.com/WebAssembly/simd/blob/master/proposals/simd/SIMD.md)                                                         |  ```--enable-fixed-width-simd```     |
| [Extended constant expressions](https://github.com/WebAssembly/extended-const/blob/master/proposals/extended-const/Overview.md)                    |  :x:                                 |
| [Tail calls](https://github.com/WebAssembly/tail-call/blob/master/proposals/tail-call/Overview.md)                                                 |  :x:                                 |
| [Threads and atomics](https://github.com/WebAssembly/threads/blob/master/proposals/threads/Overview.md)                                            |  ```--enable-thread```               |
| [Exception handling](https://github.com/WebAssembly/exception-handling/blob/master/proposals/exception-handling/Exceptions.md)                     |  ```--enable-exception-handling```   |
| [JS Promise Integration](https://github.com/WebAssembly/js-promise-integration)                                                                    |  N/A                                 |
| [Memory64](https://github.com/WebAssembly/memory64/blob/master/proposals/memory64/Overview.md)                                                     |  ```--enable-memory64```             |
| [Custom Page Sizes](https://github.com/WebAssembly/custom-page-sizes/blob/main/proposals/custom-page-sizes/Overview.md)                            |  ```--enable-custom-page-sizes```    |
| [Type reflection](https://github.com/WebAssembly/js-types/blob/main/proposals/js-types/Overview.md)                                                |  :x:                                 |
| [Sign-extension operators](https://github.com/WebAssembly/sign-extension-ops/blob/master/proposals/sign-extension-ops/Overview.md)                 |  V1.0.1                              |
| [WASI](https://github.com/WebAssembly/WASI)                                                                                                        |  V1.0.1                              |
| [WASI Preview 2](https://github.com/WebAssembly/WASI/blob/main/preview2/README.md)                                                                 |  :x:                                 |
| [WASIX](https://wasix.org/)                                                                                                                        |  :x:                                 |

### Supports multiple platforms
Supports over 100 triplet platforms, including DOS series, POSIX series, Windows 9x series, Windows NT series, and the Host C Library Series. Supports interpretation execution (INT), just-in-time compilation (JIT), and tiered compilation (TC). 

#### POSIX Environment
* Linux: (x86\_64, i386, aarch64, arm, loongarch64, powerpc64, riscv, mips, etc.)-linux-(gnu, musl, llvm, mlibc, uclibc, etc.)
* Darwin (macosx, iphoneos, watchos): (x86\_64, i386, aarch64, arm, powerpc64)-apple-darwin
* FreeBSD: (x86\_64, i386, aarch64, arm, powerpc64, etc.)-freebsd(Version)
* NetBSD: unknown(50+)-netbsd
* OpenBSD: unknown-openbsd
* DragonflyBSD: (x86\_64, i386, aarch64, arm, powerpc64, etc.)-dragonfly
* WASI (bootstrap): (wasm32, wasm64)-(wasip1, wasip2)-(threads)
* SerenityOS: (x86\_64, i686)-serenity
* SunOS: (sparc, x86)-(sunos, solaris)-gnu

#### Windows NT
* Supported system versions: Windows 2000, Windows XP, Windows Vista, Windows 7, Windows 8, Windows Blue, Windows 10, Windows 11
* MSVC: (x86\_64, i686, aarch64, arm)-windows-msvc
* MinGW: (x86\_64, i686, aarch64, arm)-windows-gnu (aka. unknown-w64-mingw32)

#### Windows 9x
* Supported system versions: Windows 95, Windows ME, Windows 98
* MinGW: i686-windows-gnu (aka. i686-w64-mingw32)

#### DOS
* MS-DOS DJGPP: i386-msdosdjgpp

#### Host C Library Environment
* newlib: unknown-elf; (x86\_64, i386, etc.)-(cygwin, msys2)
* avr: avr-elf

#### Kernel
* Windows NT kernel Driver
* Linux kernel Module

#### Other System
* Managarm
* Vinix
* etc.
