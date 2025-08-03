;; Mixed cycles test: Module H
;; Linear: F -> G -> H (no cycle)
(module
  ;; Module H has no imports (end of linear chain)
  
  ;; Module H exports a function
  (func $func_h (export "func"))
) 