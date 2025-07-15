# WebAssembly Custom Section Parser
Handling of the wasm custom section

## Features 
* `Thread-safe`: wasm-custom-parser is thread-safe. All functions are reentrant and do not use global mutable state. You can safely call functions within the wasm-custom-parser concurrently from multiple threads.
* `Freestanding`: Support for c++ standalone implementations (need to define your own concepts) for use in OS-less environments.