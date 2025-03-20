/********************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)     *
 * Copyright (c) 2025 MacroModel. All rights reserved.  *
 * Licensed under the APL-2 License (see LICENSE file). *
 ********************************************************/

/**
 * @file main.cc
 * @brief main function for crt startup
 * @details This function does nothing but call the cpp boot method of the other module
 * @author MacroModel
 * @version 2.0.0
 * @date 2025-03-20
 * @copyright APL-2 License
 */

 /**
 * @brief import uwvm module
 */
import uwvm;

/**
 * @brief main func
 * @param argc Argument Count
 * @param argv Argument Vector
 * @return exit(3)
 * @see ::uwvm::uwvm_main()
 */
extern "C"
#if __has_cpp_attribute(__gnu__::__used__)
    [[__gnu__::__used__]]
#endif
    int
    main(int argc, char** argv)
{
    return ::uwvm::uwvm_main(argc, argv);
}

