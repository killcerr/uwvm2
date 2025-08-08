;; Large cycle test: Module E
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module E imports from F
  (import "F" "func" (func $import_from_f))
  
  ;; Module E exports a function
  (func $func_e (export "func"))
  
  ;; Call F's function
  (func $call_f
    call $import_from_f
  )
) 