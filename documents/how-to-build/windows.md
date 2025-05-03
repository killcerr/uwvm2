# Windows


## Use MSVC
1. Install [[xmake]](https://github.com/xmake-io/xmake/)
2. Install [[MSVC]](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022)
3. Build
```shell
$ xmake f -m [debug|release|releasedbg|minsizerel]
$ xmake
```
4. Install UWVM2
```shell
$ xmake i -o <install_path>
```

### Additional Options
1. `--static` Static links
2. `--use-cxx-module=y` Use cpp module to compile, compiler may not be supported

## Use LLVM
1. Install [[xmake]](https://github.com/xmake-io/xmake/)
2. Install [[llvm]](https://github.com/llvm/llvm-project/releases)
3. Build
```shell
$ xmake f -m [debug|release|releasedbg|minsizerel] --use-llvm=y --sysroot=<unknown-windows-msvc sysroot path>
$ xmake
```
4. Install UWVM2
```shell
$ xmake i -o <install_path>
```

### Additional Options
1. `--static` Static links
2. `--march` The default is native, which uses the cpu designator to control it
3. `--use-cxx-module=y` Use cpp module to compile, compiler may not be supported