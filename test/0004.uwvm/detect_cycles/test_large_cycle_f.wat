;; Large cycle test: Module F
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module F imports from G
  (import "G" "func" (func $import_from_g))
  
  ;; Module F exports a function
  (func $func_f (export "func"))
  
  ;; Call G's function
  (func $call_g
    call $import_from_g
  )
) 