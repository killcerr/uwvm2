;; Other module that imports from "main"
;; This creates a cycle: main -> other -> main
(module
  ;; Import from the main module
  (import "main" "func" (func $import_from_main))
  
  ;; Export a function
  (func $func_other (export "func"))
  
  ;; Call the imported function
  (func $call_main
    call $import_from_main
  )
) 