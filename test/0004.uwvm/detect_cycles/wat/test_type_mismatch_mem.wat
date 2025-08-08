;; Type mismatch test module - imports table but target exports memory
;; This module imports a table, but the target module exports memory
(module
  ;; Import a table
  (import "target_mem" "exported_mem" (table 1 1 funcref))
  
  ;; Export a function
  (func $func_main (export "func"))
) 