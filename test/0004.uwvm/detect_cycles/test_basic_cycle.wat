;; Basic cycle test module
;; This module imports from "other" and exports "func"
(module
  ;; Import from another module
  (import "other" "func" (func $import_from_other))
  
  ;; Export a function
  (func $func_main (export "func"))
  
  ;; Call the imported function
  (func $call_other
    call $import_from_other
  )
) 