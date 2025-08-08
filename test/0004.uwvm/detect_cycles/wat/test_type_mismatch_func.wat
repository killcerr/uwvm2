;; Type mismatch test module - imports func but target exports table
;; This module imports a function, but the target module exports a table
(module
  ;; Import a function
  (import "target_table" "exported_table" (func $import_func))
  
  ;; Export a function
  (func $func_main (export "func"))
  
  ;; Call the imported function
  (func $call_func
    call $import_func
  )
) 