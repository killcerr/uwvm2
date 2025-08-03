;; Large cycle test: Module I
;; A -> B -> C -> D -> E -> F -> G -> H -> I -> J -> A
(module
  ;; Module I imports from J
  (import "J" "func" (func $import_from_j))
  
  ;; Module I exports a function
  (func $func_i (export "func"))
  
  ;; Call J's function
  (func $call_j
    call $import_from_j
  )
) 