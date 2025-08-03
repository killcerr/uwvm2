;; Target module for type mismatch test - exports memory instead of table
;; This module exports memory (type 2), not a table (type 1)
(module
  ;; Export memory (type 2) instead of a table (type 1)
  (memory (export "exported_mem") 1 1)
) 