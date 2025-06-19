# WebAssembly parser
Provide a conceptualization of an extensible parser

* `base` Some basic structures
* `binfmt` Parsing implementations and storage constructs for different wasm binary format versions, automatic synthesis via the concept of binfmt
* `concept` Some template metaprogramming, or the lowest level wasm composition concepts, including binfmt categorization, etc.
* `feature` Provide supported feature macros
* `proposal` Support for the wasm proposal. Includes structures corresponding to proposals, parsing programs, command code definition
* `standard` Support for the wasm standard. Includes standardized structural equivalents, parsing programs, command code definition

```txt
concept:                         root (concept)
                                      |
                         ____________/ \______________
                        /                             \
binfmt:           binfmt_ver1(binfmt1)                ...
                 /       |        |    \
featurs:       wasm1    wasm1.1  wasm2  ... (standard, proposal)

```

## Features 
* `Thread-safe`: wasm-parser is thread-safe. All functions are reentrant and do not use global mutable state. You can safely call functions within the wasm-parser concurrently from multiple threads.