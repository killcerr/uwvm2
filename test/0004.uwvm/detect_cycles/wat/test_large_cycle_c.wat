;; Large cycle test: Module C
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module C imports from D
  (import "D" "func" (func $import_from_d))
  
  ;; Module C exports a function
  (func $func_c (export "func"))
  
  ;; Call D's function
  (func $call_d
    call $import_from_d
  )
) 