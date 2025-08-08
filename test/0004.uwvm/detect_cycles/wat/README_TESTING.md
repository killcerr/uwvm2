# Detect Cycles Function Testing Guide

This directory contains a complete test suite for validating the correctness of the `detect_cycles` function.

## 📋 Test Scripts Description

### 1. `compile_all_wat.sh` - Compilation Script
```bash
# Compile all WAT files to WASM files
./compile_all_wat.sh
```

**Features:**
- Automatically detects if `wat2wasm` tool is available
- Batch compiles all `.wat` files in the current directory
- Shows compilation result statistics
- Checks compilation errors and reports them

**Requirements:**
- Install WebAssembly Binary Toolkit (wabt)
- macOS: `brew install wabt`
- Ubuntu/Debian: `sudo apt-get install wabt`

### 2. `test_detect_cycles_simple.sh` - Simplified Test Script
```bash
# Run simplified tests
./test_detect_cycles_simple.sh
```

**Test Cases:**
- Basic cycle detection (main ↔ other)
- Complex cycle detection (A → B → C → A)
- No cycle module verification
- Missing dependency detection
- Multiple cycles detection
- Large cycle detection (10 nodes)
- Mixed cycles detection
- Missing export detection
- Type mismatch detection (func vs table)
- Type mismatch detection (table vs memory)
- Type mismatch detection (global vs func)

### 3. `test_detect_cycles_validation.sh` - Complete Test Script
```bash
# Run complete test suite
./test_detect_cycles_validation.sh
```

**Test Cases:**
- Basic cycle detection
- Complex cycle detection
- No cycle module
- Missing dependency detection
- Multiple cycles detection
- Large cycle detection (10 nodes)
- Mixed cycles detection
- Missing export detection
- Type mismatch detection (func vs table)
- Type mismatch detection (table vs memory)
- Type mismatch detection (global vs func)

## 🎯 Testing Objectives

### Validate the following characteristics of the `detect_cycles` function:

1. **Correctness**
   - Accurately detect cyclic dependencies
   - Avoid false positives for modules without cycles
   - Correctly handle missing dependencies
   - Detect missing exports
   - Detect type mismatches

2. **Efficiency**
   - Johnson algorithm performance optimization
   - Node-level deduplication optimization
   - Avoid duplicate detection of the same cycles

3. **Robustness**
   - Handle various complex dependency graphs
   - Support large cycles (10+ nodes)
   - Handle multiple independent cycles
   - Handle import/export validation

## 📊 Test Cases Details

### Basic Cycle Test
```
test_basic_cycle.wat  → Imports "other"
test_other_module.wat → Imports "main"
```
**Expected Result:** Detect 1 cycle

### Complex Cycle Test
```
test_complex_cycle.wat    → Imports "B"
test_complex_cycle_b.wat  → Imports "C"
test_complex_cycle_c.wat  → Imports "A"
```
**Expected Result:** Detect 1 cycle (A → B → C → A)

### No Cycle Test
```
test_no_cycle.wat → Linear dependency chain
```
**Expected Result:** No cycle detection, module runs normally

### Large Cycle Test
```
test_large_cycle_a.wat → test_large_cycle_j.wat (10 nodes)
```
**Expected Result:** Detect 1 large cycle

### Multiple Cycles Test
```
test_multiple_cycles_a.wat → test_multiple_cycles_h.wat (3 independent cycles)
```
**Expected Result:** Detect 3 independent cycles

### Missing Export Test
```
test_missing_export.wat → Imports "nonexistent_func" from "target"
test_target_module.wat  → Only exports "func", not "nonexistent_func"
```
**Expected Result:** Detect missing export error

### Type Mismatch Tests
```
test_type_mismatch_func.wat   → Imports func from "target_table"
test_target_table.wat         → Exports table, not func

test_type_mismatch_mem.wat    → Imports table from "target_mem"
test_target_memory.wat        → Exports memory, not table

test_type_mismatch_global.wat → Imports global from "target_func"
test_target_function.wat      → Exports function, not global
```
**Expected Result:** Detect type mismatch error for each case

## 🚀 Usage Steps

### Step 1: Compile WAT Files
```bash
cd test/0004.uwvm/detect_cycles
./compile_all_wat.sh
```

### Step 2: Run Tests
```bash
# Run simplified tests
./test_detect_cycles_simple.sh

# Or run complete tests
./test_detect_cycles_validation.sh
```

### Step 3: Check Results
- ✅ All tests passed: `detect_cycles` function is working correctly
- ❌ Some tests failed: Need to check function implementation

## 🔧 Important Notes

### Module Name Setting
Test scripts use the `--wasm-set-main-module-name` parameter to correctly set module names, which is crucial for cycle detection:

```bash
# Correct way
uwvm2 --wasm-set-main-module-name main --wasm-preload-library other.wasm other --run main.wasm

# Wrong way (will cause missing dependency error instead of cycle detection)
uwvm2 --wasm-preload-library other.wasm other --run main.wasm
```

### Cycle Detection Mechanism
- uwvm2 enables cycle detection by default (`show_depend_warning = true`)
- Cycle detection occurs during module loading and dependency checking phase
- Detected cycles will output warning messages

### Error Detection Types
- **Missing Module Dependency**: When a module imports from a non-existent module
- **Missing Export**: When a module imports something that doesn't exist in the target module
- **Type Mismatch**: When import and export types don't match (e.g., importing func but exporting table)

## 🔧 Troubleshooting

### Common Issues

1. **wat2wasm not found**
   ```bash
   # macOS
   brew install wabt
   
   # Ubuntu/Debian
   sudo apt-get install wabt
   ```

2. **uwvm2 not found**
   ```bash
   # Ensure uwvm2 is compiled
   xmake build
   
   # Or ensure uwvm2 is in PATH
   export PATH=$PATH:/path/to/uwvm2
   ```

3. **Test failures**
   - Check if WAT file syntax is correct
   - Confirm WASM files are compiled correctly
   - Verify module names are set correctly
   - Ensure all dependency modules are preloaded

4. **Cycle detection not working**
   - Ensure `--wasm-set-main-module-name` parameter is used
   - Check if all related modules are preloaded
   - Verify module names match import statements

5. **Import/Export validation issues**
   - Check if target modules export the expected items
   - Verify import/export type compatibility
   - Ensure module names match between import and export

## 📈 Performance Benchmarks

| Test Type | Node Count | Cycle Count | Expected Performance |
|-----------|------------|-------------|---------------------|
| Basic Cycle | 2 | 1 | Fast detection |
| Complex Cycle | 3 | 1 | Medium speed |
| Large Cycle | 10 | 1 | Optimized algorithm |
| Multiple Cycles | 8 | 3 | Efficient deduplication |
| No Cycles | 3 | 0 | Fast skip |
| Missing Export | 2 | 0 | Fast validation |
| Type Mismatch (func vs table) | 2 | 0 | Fast validation |
| Type Mismatch (table vs memory) | 2 | 0 | Fast validation |
| Type Mismatch (global vs func) | 2 | 0 | Fast validation |

## 🎯 Validation Points

1. **Cycle Detection Accuracy**
   - Correctly identify all cyclic dependencies
   - Avoid false positives for linear dependencies
   - Correctly handle complex cycle structures

2. **Deduplication Optimization Effects**
   - Avoid duplicate detection of the same cycles
   - Node-level deduplication works correctly
   - Performance optimization effects are obvious

3. **Performance**
   - Large cycle detection time is reasonable
   - Memory usage is controllable
   - Algorithm efficiency meets expectations

4. **Error Handling**
   - Correctly handle missing dependencies
   - Correctly handle missing exports
   - Correctly handle type mismatches
   - Provide meaningful error messages
   - Distinguish between different error types

## 📝 Test Reports

After running tests, scripts will generate detailed test reports including:
- Test case execution results
- Pass/fail statistics
- Performance metrics
- Error details (if any)

## ✅ Test Status

**Current Status:** All tests passed ✅

- ✅ Basic cycle detection: Working correctly
- ✅ Complex cycle detection: Working correctly
- ✅ No cycle module: Working correctly
- ✅ Missing dependency detection: Working correctly
- ✅ Multiple cycles detection: Working correctly
- ✅ Large cycle detection: Working correctly
- ✅ Mixed cycles detection: Working correctly
- ✅ Missing export detection: Working correctly
- ✅ Type mismatch detection (func vs table): Working correctly
- ✅ Type mismatch detection (table vs memory): Working correctly
- ✅ Type mismatch detection (global vs func): Working correctly

These tests ensure the `detect_cycles` function is correct and reliable in various scenarios. 