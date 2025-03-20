function windows_target()
    local use_llvm_toolchain = get_config("use-llvm")
    if use_llvm_toolchain then	
        set_toolchains("clang-cl")
        --add_cxflags("-std:c++latest")
    end

    if is_kind("binary") then
        set_extension(".exe")
    end

    if is_mode("debug") then
        add_cxflags("/guard:cf") -- enable Control Flow Guard
    end

    add_cxflags("-GR-") -- disable rtti

    local static_link = get_config("static")

    if is_mode("debug") then
        add_cxflags("-GS") -- enable buffer overflow check

        if static_link then	
            set_runtimes("MTd")
        else
            set_runtimes("MDd")
        end
    else
        set_fpmodels("fast")

        add_cxflags("-GS-") -- disable buffer overflow check
        add_cxflags("-GL") -- Whole Program Optimization
        --add_ldflags("-LTCG") -- Link Time Code Generation

        if static_link then	
            set_runtimes("MT")
        else
            set_runtimes("MD")
        end
    end

    local opt_name = get_config("winmin")
    if opt_name == "default" then	

    -- Windows NT (with win32 api)

    elseif 
        opt_name == "WS25" or 
        opt_name == "WIN11" or 
        opt_name == "WS22" or
        opt_name == "WS19" or
        opt_name == "WS16" or
        opt_name == "WIN10"
            then
        add_defines("_WIN32_WINNT=0x0A00")
    elseif 
        opt_name == "WS12R2" or
        opt_name == "WINBLUE"
            then
        add_defines("_WIN32_WINNT=0x0603")
    elseif 
        opt_name == "WS12" or 
        opt_name == "WIN8" 
            then
        add_defines("_WIN32_WINNT=0x0602")
    elseif 
        opt_name == "WS08R2" or
        opt_name == "WIN7"
            then
        add_defines("_WIN32_WINNT=0x0601")
    elseif 
        opt_name == "WS08" or
        opt_name == "VISTA" 
            then
        add_defines("_WIN32_WINNT=0x0600")
    elseif opt_name == "WS03R2" then
        add_defines("_WIN32_WINNT=0x0502")
    elseif 
        opt_name == "WS03" or
        opt_name == "WINXP"
    then
        add_defines("_WIN32_WINNT=0x0501")
    elseif 
        opt_name == "WS2K" or
        opt_name == "WIN2K"
    then
        add_defines("_WIN32_WINNT=0x0500")

    -- Windows 9x (with win32 api)

    elseif opt_name == "WINME" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WIN32_WINDOWS=0x0490")
    elseif opt_name == "WIN98" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WIN32_WINDOWS=0x0410")
    elseif opt_name == "WIN95" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WIN32_WINDOWS=0x0400")

    -- Windows NT (without win32 api)

    elseif opt_name == "NT400" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0400")
    elseif opt_name == "NT351" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0351")
    elseif opt_name == "NT350" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0350")    
    elseif opt_name == "NT310" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0310")
    else
        error("invalid value")
    end

    local march = get_config("march")
    if march == "no" or march == "default" then
    else
        error("windows (unknown-windows-msvc) does not support custom march!")
    end
end