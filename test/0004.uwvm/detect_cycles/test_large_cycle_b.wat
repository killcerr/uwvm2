;; Large cycle test: Module B
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module B imports from C
  (import "C" "func" (func $import_from_c))
  
  ;; Module B exports a function
  (func $func_b (export "func"))
  
  ;; Call C's function
  (func $call_c
    call $import_from_c
  )
) 