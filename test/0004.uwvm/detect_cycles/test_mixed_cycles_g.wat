;; Mixed cycles test: Module G
;; Linear: F -> G -> H (no cycle)
(module
  ;; Module G imports from H
  (import "H" "func" (func $import_from_h))
  
  ;; Module G exports a function
  (func $func_g (export "func"))
  
  ;; Call H's function
  (func $call_h
    call $import_from_h
  )
) 