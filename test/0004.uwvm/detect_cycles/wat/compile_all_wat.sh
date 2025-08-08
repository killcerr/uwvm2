#!/bin/bash

# Script to compile all WAT files in the current directory
# Requires wat2wasm tool (WebAssembly Binary Toolkit)

echo "=== Compiling all WAT files to WASM files ==="

# Check if wat2wasm is available
if ! command -v wat2wasm &> /dev/null; then
    echo "❌ Error: wat2wasm tool not found"
    echo "Please install WebAssembly Binary Toolkit (wabt):"
    echo "  macOS: brew install wabt"
    echo "  Ubuntu/Debian: sudo apt-get install wabt"
    echo "  Windows: Download wabt binary files"
    exit 1
fi

# Counters
compiled_count=0
error_count=0

# Iterate through all .wat files in the current directory
for wat_file in *.wat; do
    # Check if file exists (avoid processing *.wat literal)
    if [[ -f "$wat_file" ]]; then
        # Generate corresponding wasm filename
        wasm_file="${wat_file%.wat}.wasm"
        
        echo "Compiling: $wat_file -> $wasm_file"
        
        # Compile wat file to wasm file
        if wat2wasm "$wat_file" -o "$wasm_file"; then
            echo "✅ Successfully compiled: $wat_file"
            ((compiled_count++))
        else
            echo "❌ Compilation failed: $wat_file"
            ((error_count++))
        fi
    fi
done

echo ""
echo "=== Compilation Complete ==="
echo "Successfully compiled: $compiled_count files"
echo "Compilation failed: $error_count files"

# Show all generated wasm files
echo ""
echo "Generated WASM files:"
for wasm_file in *.wasm; do
    if [[ -f "$wasm_file" ]]; then
        echo "  - $wasm_file"
    fi
done

# If all files compiled successfully, show success message
if [[ $error_count -eq 0 ]]; then
    echo ""
    echo "🎉 All WAT files compiled successfully!"
    echo "Now you can use these WASM files to test the detect_cycles function."
else
    echo ""
    echo "⚠️  $error_count files failed to compile, please check error messages."
fi 