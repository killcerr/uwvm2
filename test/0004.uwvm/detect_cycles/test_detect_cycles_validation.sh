#!/bin/bash

# Script to test detect_cycles function correctness
# Based on test0004 test cases

echo "=== Testing detect_cycles Function Correctness ==="
echo "Based on test0004 test cases"
echo ""

# Check if uwvm2 is available
UWVM_PATH=""
if command -v uwvm2 &> /dev/null; then
    UWVM_PATH="uwvm2"
elif [[ -f "../../../build/macosx/arm64/debug/uwvm" ]]; then
    UWVM_PATH="../../../build/macosx/arm64/debug/uwvm"
elif [[ -f "../../../build/macosx/x86_64/debug/uwvm" ]]; then
    UWVM_PATH="../../../build/macosx/x86_64/debug/uwvm"
else
    echo "❌ Error: uwvm2 executable not found"
    echo "Please compile uwvm2 first or ensure uwvm2 is in PATH"
    exit 1
fi

echo "Using uwvm2 path: $UWVM_PATH"
echo ""

# Test counters
total_tests=0
passed_tests=0
failed_tests=0

# Test function
run_test() {
    local test_name="$1"
    local test_command="$2"
    local expected_pattern="$3"
    local description="$4"
    
    ((total_tests++))
    echo "Test $total_tests: $test_name"
    echo "Description: $description"
    
    # Run test command and capture output
    local output
    output=$(eval "$test_command" 2>&1)
    local exit_code=$?
    
    # Check if output contains expected pattern
    if echo "$output" | grep -q "$expected_pattern"; then
        echo "✅ Passed: Detected expected pattern '$expected_pattern'"
        ((passed_tests++))
    else
        echo "❌ Failed: Did not detect expected pattern '$expected_pattern'"
        echo "Actual output:"
        echo "$output"
        ((failed_tests++))
    fi
    echo ""
}

# Test case 1: Basic cycle detection (main <-> other)
echo "=== Test Case 1: Basic Cycle Detection ==="
run_test "Basic Cycle" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_other_module.wasm other --run test_basic_cycle.wasm" \
    "cyclic dependencies" \
    "Detect cycle dependency between main <-> other"

# Test case 2: Complex cycle detection (A -> B -> C -> A)
echo "=== Test Case 2: Complex Cycle Detection ==="
run_test "Complex Cycle" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_complex_cycle_b.wasm B --wasm-preload-library test_complex_cycle_c.wasm C --run test_complex_cycle.wasm" \
    "cyclic dependencies" \
    "Detect complex cycle dependency A -> B -> C -> A"

# Test case 3: No cycle module
echo "=== Test Case 3: No Cycle Module ==="
run_test "No Cycle" \
    "$UWVM_PATH --run test_no_cycle.wasm" \
    "" \
    "Verify that modules without cycle dependencies run normally"

# Test case 4: Missing dependency detection
echo "=== Test Case 4: Missing Dependency Detection ==="
run_test "Missing Dependency" \
    "$UWVM_PATH --run test_basic_cycle.wasm" \
    "Missing module dependency" \
    "Detect missing module dependencies"

# Test case 5: Multiple cycles detection
echo "=== Test Case 5: Multiple Cycles Detection ==="
run_test "Multiple Cycles" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_multiple_cycles_b.wasm B --wasm-preload-library test_multiple_cycles_c.wasm C --wasm-preload-library test_multiple_cycles_d.wasm D --wasm-preload-library test_multiple_cycles_e.wasm E --wasm-preload-library test_multiple_cycles_f.wasm F --wasm-preload-library test_multiple_cycles_g.wasm G --wasm-preload-library test_multiple_cycles_h.wasm H --run test_multiple_cycles_a.wasm" \
    "cyclic dependencies" \
    "Detect multiple independent cycle dependencies"

# Test case 6: Large cycle detection
echo "=== Test Case 6: Large Cycle Detection ==="
run_test "Large Cycle" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_large_cycle_b.wasm B --wasm-preload-library test_large_cycle_c.wasm C --wasm-preload-library test_large_cycle_d.wasm D --wasm-preload-library test_large_cycle_e.wasm E --wasm-preload-library test_large_cycle_f.wasm F --wasm-preload-library test_large_cycle_g.wasm G --wasm-preload-library test_large_cycle_h.wasm H --wasm-preload-library test_large_cycle_i.wasm I --wasm-preload-library test_large_cycle_j.wasm J --run test_large_cycle_a.wasm" \
    "cyclic dependencies" \
    "Detect large cycle dependency with 10 nodes"

# Test case 7: Mixed cycles detection
echo "=== Test Case 7: Mixed Cycles Detection ==="
run_test "Mixed Cycles" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_mixed_cycles_b.wasm B --wasm-preload-library test_mixed_cycles_c.wasm C --wasm-preload-library test_mixed_cycles_d.wasm D --wasm-preload-library test_mixed_cycles_e.wasm E --wasm-preload-library test_mixed_cycles_f.wasm F --wasm-preload-library test_mixed_cycles_g.wasm G --wasm-preload-library test_mixed_cycles_h.wasm H --run test_mixed_cycles_a.wasm" \
    "cyclic dependencies" \
    "Detect mixed cycles and linear dependencies"

# Test case 8: Missing export detection
echo "=== Test Case 8: Missing Export Detection ==="
run_test "Missing Export" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_module.wasm target --run test_missing_export.wasm" \
    "Missing export" \
    "Detect missing export in target module"

# Test case 9: Type mismatch detection - func vs table
echo "=== Test Case 9: Type Mismatch Detection (func vs table) ==="
run_test "Type Mismatch Func vs Table" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_table.wasm target_table --run test_type_mismatch_func.wasm" \
    "Type mismatch" \
    "Detect type mismatch between func import and table export"

# Test case 10: Type mismatch detection - table vs memory
echo "=== Test Case 10: Type Mismatch Detection (table vs memory) ==="
run_test "Type Mismatch Table vs Memory" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_memory.wasm target_mem --run test_type_mismatch_mem.wasm" \
    "Type mismatch" \
    "Detect type mismatch between table import and memory export"

# Test case 11: Type mismatch detection - global vs func
echo "=== Test Case 11: Type Mismatch Detection (global vs func) ==="
run_test "Type Mismatch Global vs Func" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_function.wasm target_func --run test_type_mismatch_global.wasm" \
    "Type mismatch" \
    "Detect type mismatch between global import and function export"

echo "=== Test Results Summary ==="
echo "Total tests: $total_tests"
echo "Passed tests: $passed_tests"
echo "Failed tests: $failed_tests"

if [[ $failed_tests -eq 0 ]]; then
    echo ""
    echo "🎉 All tests passed! detect_cycles function is working correctly."
    exit 0
else
    echo ""
    echo "⚠️  $failed_tests tests failed, please check the detect_cycles function implementation."
    exit 1
fi 