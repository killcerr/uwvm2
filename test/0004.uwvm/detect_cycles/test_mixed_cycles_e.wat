;; Mixed cycles test: Module E
;; Cycle 2: C -> D -> E -> C
(module
  ;; Module E imports from C
  (import "C" "func" (func $import_from_c))
  
  ;; Module E exports a function
  (func $func_e (export "func"))
  
  ;; Call C's function
  (func $call_c
    call $import_from_c
  )
) 