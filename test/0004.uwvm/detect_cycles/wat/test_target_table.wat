;; Target module for type mismatch test - exports table instead of func
;; This module exports a table, not a function
(module
  ;; Export a table (type 1) instead of a function (type 0)
  (table (export "exported_table") 1 1 funcref)
) 