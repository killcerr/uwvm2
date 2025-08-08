;; Multiple cycles test: Module C
;; Cycle 2: C -> D -> E -> C
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