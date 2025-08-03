#!/bin/bash

# Simplified detect_cycles function test script
# Focused on core functionality validation

echo "=== Simplified detect_cycles Function Test ==="
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
    exit 1
fi

echo "Using uwvm2 path: $UWVM_PATH"
echo ""

# Test counters
passed=0
failed=0

# Test function
test_cycle_detection() {
    local test_name="$1"
    local test_command="$2"
    local expected="$3"
    
    echo "Test: $test_name"
    echo "Command: $test_command"
    
    # Run command and check output
    local output
    output=$(eval "$test_command" 2>&1)
    local exit_code=$?
    
    if echo "$output" | grep -q "$expected"; then
        echo "✅ Passed: Detected '$expected'"
        ((passed++))
    else
        echo "❌ Failed: Did not detect '$expected'"
        echo "Output: $output"
        ((failed++))
    fi
    echo ""
}

# Test 1: Basic cycle detection
echo "=== Test 1: Basic Cycle Detection (main <-> other) ==="
test_cycle_detection "Basic Cycle" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_other_module.wasm other --run test_basic_cycle.wasm" \
    "cyclic dependencies"

# Test 2: Complex cycle detection
echo "=== Test 2: Complex Cycle Detection (A -> B -> C -> A) ==="
test_cycle_detection "Complex Cycle" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_complex_cycle_b.wasm B --wasm-preload-library test_complex_cycle_c.wasm C --run test_complex_cycle.wasm" \
    "cyclic dependencies"

# Test 3: No cycle module
echo "=== Test 3: No Cycle Module ==="
test_cycle_detection "No Cycle" \
    "$UWVM_PATH --run test_no_cycle.wasm" \
    ""

# Test 4: Missing dependency detection
echo "=== Test 4: Missing Dependency Detection ==="
test_cycle_detection "Missing Dependency" \
    "$UWVM_PATH --run test_basic_cycle.wasm" \
    "Missing module dependency"

# Test 5: Multiple cycles detection
echo "=== Test 5: Multiple Cycles Detection ==="
test_cycle_detection "Multiple Cycles" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_multiple_cycles_b.wasm B --wasm-preload-library test_multiple_cycles_c.wasm C --wasm-preload-library test_multiple_cycles_d.wasm D --wasm-preload-library test_multiple_cycles_e.wasm E --wasm-preload-library test_multiple_cycles_f.wasm F --wasm-preload-library test_multiple_cycles_g.wasm G --wasm-preload-library test_multiple_cycles_h.wasm H --run test_multiple_cycles_a.wasm" \
    "cyclic dependencies"

# Test 6: Large cycle detection
echo "=== Test 6: Large Cycle Detection (10 nodes) ==="
test_cycle_detection "Large Cycle" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_large_cycle_b.wasm B --wasm-preload-library test_large_cycle_c.wasm C --wasm-preload-library test_large_cycle_d.wasm D --wasm-preload-library test_large_cycle_e.wasm E --wasm-preload-library test_large_cycle_f.wasm F --wasm-preload-library test_large_cycle_g.wasm G --wasm-preload-library test_large_cycle_h.wasm H --wasm-preload-library test_large_cycle_i.wasm I --wasm-preload-library test_large_cycle_j.wasm J --run test_large_cycle_a.wasm" \
    "cyclic dependencies"

# Test 7: Mixed cycles detection
echo "=== Test 7: Mixed Cycles Detection ==="
test_cycle_detection "Mixed Cycles" \
    "$UWVM_PATH --wasm-set-main-module-name A --wasm-preload-library test_mixed_cycles_b.wasm B --wasm-preload-library test_mixed_cycles_c.wasm C --wasm-preload-library test_mixed_cycles_d.wasm D --wasm-preload-library test_mixed_cycles_e.wasm E --wasm-preload-library test_mixed_cycles_f.wasm F --wasm-preload-library test_mixed_cycles_g.wasm G --wasm-preload-library test_mixed_cycles_h.wasm H --run test_mixed_cycles_a.wasm" \
    "cyclic dependencies"

# Test 8: Missing export detection
echo "=== Test 8: Missing Export Detection ==="
test_cycle_detection "Missing Export" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_module.wasm target --run test_missing_export.wasm" \
    "Missing export"

# Test 9: Type mismatch detection - func vs table
echo "=== Test 9: Type Mismatch Detection (func vs table) ==="
test_cycle_detection "Type Mismatch Func vs Table" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_table.wasm target_table --run test_type_mismatch_func.wasm" \
    "Type mismatch"

# Test 10: Type mismatch detection - table vs memory
echo "=== Test 10: Type Mismatch Detection (table vs memory) ==="
test_cycle_detection "Type Mismatch Table vs Memory" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_memory.wasm target_mem --run test_type_mismatch_mem.wasm" \
    "Type mismatch"

# Test 11: Type mismatch detection - global vs func
echo "=== Test 11: Type Mismatch Detection (global vs func) ==="
test_cycle_detection "Type Mismatch Global vs Func" \
    "$UWVM_PATH --wasm-set-main-module-name main --wasm-preload-library test_target_function.wasm target_func --run test_type_mismatch_global.wasm" \
    "Type mismatch"

# Show results
echo "=== Test Results ==="
echo "Passed: $passed"
echo "Failed: $failed"
echo "Total: $((passed + failed))"

if [[ $failed -eq 0 ]]; then
    echo ""
    echo "🎉 All tests passed! detect_cycles function is working correctly."
    exit 0
else
    echo ""
    echo "⚠️  $failed tests failed."
    exit 1
fi 