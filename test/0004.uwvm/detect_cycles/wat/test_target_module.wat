;; Target module for missing export test
;; This module only exports "func", not "nonexistent_func"
(module
  ;; Export a function
  (func $func_target (export "func"))
  
  ;; This function is not exported, so it should cause a missing export error
  (func $internal_func)
) 