;; Mixed cycles test: Module B
;; Cycle 1: A -> B -> A
(module
  ;; Module B imports from A
  (import "A" "func" (func $import_from_a))
  
  ;; Module B exports a function
  (func $func_b (export "func"))
  
  ;; Call A's function
  (func $call_a
    call $import_from_a
  )
) 