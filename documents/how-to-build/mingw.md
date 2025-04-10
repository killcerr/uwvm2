# MinGW

## Use GCC
1. Install [[xmake]](https://github.com/xmake-io/xmake/)
2. Install [[GCC]](https://sourceforge.net/projects/mingw-w64/)
3. Build
```ps
> xmake f -m [debug|release|releasedbg|minsizerel]
> xmake
```
4. Install UWVM2
```ps
> xmake i -o <install_path>
```

### Additional Options
1. `--static` Static links
2. `--march` The default is native, which uses the cpu designator to control the
3. `--winmin` Minimum windows compatible version, supported from Win11 all the way to Win95

## Use LLVM
1. Install [[xmake]](https://github.com/xmake-io/xmake/)
2. Install [[llvm]](https://github.com/llvm/llvm-project/releases)
3. Build
```ps
> xmake f -m [debug|release|releasedbg|minsizerel] --use-llvm=y
> xmake
```
4. Install UWVM2
```ps
> xmake i -o <install_path>
```

### Additional Options
1. `--static` Static links
2. `--march` The default is native, which uses the cpu designator to control the
3. `--winmin` Minimum windows compatible version, supported from Win11 all the way to Win95
