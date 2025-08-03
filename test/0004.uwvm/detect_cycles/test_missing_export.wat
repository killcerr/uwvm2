;; Missing export test module
;; This module imports a function that doesn't exist in the target module
(module
  ;; Import a function that doesn't exist in the target module
  (import "target" "nonexistent_func" (func $import_nonexistent))
  
  ;; Export a function
  (func $func_main (export "func"))
  
  ;; Call the imported function
  (func $call_nonexistent
    call $import_nonexistent
  )
) 