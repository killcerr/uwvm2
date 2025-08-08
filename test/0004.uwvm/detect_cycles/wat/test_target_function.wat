;; Target module for type mismatch test - exports function instead of global
;; This module exports a function (type 0), not a global (type 3)
(module
  ;; Export a function (type 0) instead of a global (type 3)
  (func $func_target (export "exported_func"))
) 