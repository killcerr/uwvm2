;; Mixed cycles test: Module I
;; Isolated: I (no dependencies)
(module
  ;; Module I has no imports (isolated module)
  
  ;; Module I exports a function
  (func $func_i (export "func"))
) 