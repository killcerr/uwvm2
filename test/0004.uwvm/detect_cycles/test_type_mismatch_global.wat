;; Type mismatch test module - imports global but target exports func
;; This module imports a global, but the target module exports a function
(module
  ;; Import a global
  (import "target_func" "exported_func" (global $import_global i32))
  
  ;; Export a function
  (func $func_main (export "func"))
) 