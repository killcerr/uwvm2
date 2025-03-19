# Supported platforms

## POSIX Environment
* Linux: (x86\_64, i386, aarch64, arm, loongarch64, powerpc64, riscv, mips, etc.)-linux-(gnu, musl, llvm, mlibc, uclibc, etc.)
* Darwin (macosx, iphoneos, watchos): (x86\_64, i386, aarch64, arm, powerpc64)-apple-darwin
* FreeBSD: (x86\_64, i386, aarch64, arm, powerpc64, etc.)-freebsd(Version)
* NetBSD: unknown(50+)-netbsd
* OpenBSD: unknown-openbsd
* DragonflyBSD: (x86\_64, i386, aarch64, arm, powerpc64, etc.)-dragonfly
* WASI (bootstrap): (wasm32, wasm64)-(wasip1, wasip2)-(none, threads, neoh)
* SerenityOS: (x86\_64, i686)-serenity
* SunOS: (sparc, x86)-(sunos, solaris)-gnu

## Windows NT
* Supported system versions: Windows 2000, Windows XP, Windows Vista, Windows 7, Windows 8, Windows Blue, Windows 10, Windows 11
* MSVC: (x86\_64, i686, aarch64, arm)-windows-msvc
* MinGW: (x86\_64, i686, aarch64, arm)-windows-gnu (aka. unknown-w64-mingw32)

## Windows 9x
* Supported system versions: Windows 95, Windows ME, Windows 98
* MinGW: i686-windows-gnu (aka. i686-w64-mingw32)

## DOS
* MS-DOS, FreeDOS (DJGPP): i386-msdosdjgpp

## Host C Library Environment
* newlib: unknown-elf; (x86\_64, i386, etc.)-(cygwin, msys2)
* avr: avr-elf

# Compiler Support
- GCC >= 15
- Clang >= 21
- MSVC >= 14.30

# C++ standard library support (Version is the same as the version released with the corresponding supported compiler)
- GCC libstdc++
- LLVM libc++
- MSVC STL