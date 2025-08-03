;; Multiple cycles test: Module D
;; Cycle 2: C -> D -> E -> C
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