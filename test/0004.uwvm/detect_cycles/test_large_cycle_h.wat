;; Large cycle test: Module H
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module H imports from I
  (import "I" "func" (func $import_from_i))
  
  ;; Module H exports a function
  (func $func_h (export "func"))
  
  ;; Call I's function
  (func $call_i
    call $import_from_i
  )
) 