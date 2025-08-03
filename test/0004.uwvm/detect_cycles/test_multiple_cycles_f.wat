;; Multiple cycles test: Module F
;; Cycle 3: F -> G -> H -> F
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