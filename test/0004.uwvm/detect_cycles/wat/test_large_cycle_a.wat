;; Large cycle test: Module A
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module A imports from B
  (import "B" "func" (func $import_from_b))
  
  ;; Module A exports a function
  (func $func_a (export "func"))
  
  ;; Call B's function
  (func $call_b
    call $import_from_b
  )
) 