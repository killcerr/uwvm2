;; Complex cycle test: A -> B -> C -> A
;; This module imports from "A" and exports "func"
(module
  ;; Module C imports from A
  (import "A" "func" (func $import_from_a))
  
  ;; Module C exports a function
  (func $func_c (export "func"))
  
  ;; Call A's function
  (func $call_a
    call $import_from_a
  )
) 