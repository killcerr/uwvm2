;; Large cycle test: Module D
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module D imports from E
  (import "E" "func" (func $import_from_e))
  
  ;; Module D exports a function
  (func $func_d (export "func"))
  
  ;; Call E's function
  (func $call_e
    call $import_from_e
  )
) 