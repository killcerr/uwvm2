;; Large cycle test: Module J
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module J imports from A (completes the cycle)
  (import "A" "func" (func $import_from_a))
  
  ;; Module J exports a function
  (func $func_j (export "func"))
  
  ;; Call A's function
  (func $call_a
    call $import_from_a
  )
) 