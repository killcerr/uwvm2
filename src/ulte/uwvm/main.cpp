﻿/*************************************************************
 * Ultimate WebAssembly Virtual Machine (Version 2)          *
 * Copyright (c) 2025-present UlteSoft. All rights reserved. *
 * Licensed under the APL-2 License (see LICENSE file).      *
 *************************************************************/

/**
 * @file        main.cc
 * @brief       main function for crt startup
 * @details     This function does nothing but call the cpp boot method of the other module
 * @author      MacroModel
 * @version     2.0.0
 * @date        2025-03-20
 * @copyright   APL-2 License
 */

/****************************************
 *  _   _ __        ____     __ __  __  *
 * | | | |\ \      / /\ \   / /|  \/  | *
 * | | | | \ \ /\ / /  \ \ / / | |\/| | *
 * | |_| |  \ V  V /    \ V /  | |  | | *
 *  \___/    \_/\_/      \_/   |_|  |_| *
 *                                      *
 ****************************************/

#ifdef UWVM_MODULE
import ulte.uwvm.crtmain;
#else
// import
# include "crtmain/impl.h"
#endif

/// @brief      (crt) main func. The main function is a special function, but there are still meaningless [[gnu::used]].
/// @param      argc Argument Count
/// @param      argv Argument Vector
/// @return     exit(3)
/// @see        ::ulte::uwvm::uwvm_main()

#if __has_cpp_attribute(__gnu__::__used__)
[[__gnu__::__used__]]
#endif
int main(int argc, char** argv)
{
    return ::ulte::uwvm::uwvm_main(argc, argv);
}
