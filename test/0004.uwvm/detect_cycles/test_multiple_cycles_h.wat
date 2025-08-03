;; Multiple cycles test: Module H
;; Cycle 3: F -> G -> H -> F
(module
  ;; Module H imports from F
  (import "F" "func" (func $import_from_f))
  
  ;; Module H exports a function
  (func $func_h (export "func"))
  
  ;; Call F's function
  (func $call_f
    call $import_from_f
  )
) 