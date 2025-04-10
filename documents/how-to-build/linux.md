# Linux

## Use GCC
1. Install [[xmake]](https://github.com/xmake-io/xmake/)
2. Install [[GCC]](git://gcc.gnu.org/git/gcc.git)
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

## Caveat
1. You must add `--use-llvm` if you use llvm underneath, otherwise it will fail to compile, including but not limited to symbolic linking of `gcc` to `clang`
2. On some platforms such as the latest Android which only has llvm, you must also add `--use-llvm`, same as note 1.
3. On some platforms, such as Android, the static link option will have no effect.