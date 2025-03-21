function mingw_target()

    local use_llvm_toolchain = get_config("use-llvm")
    if use_llvm_toolchain then	
        set_toolchains("clang")
        add_ldflags("-fuse-ld=lld", {force = true})
        add_cxflags("-Wno-braced-scalar-init")
    end

    local sysroot_para = get_config("sysroot")
    if sysroot_para ~= "detect" and sysroot_para then
        local sysroot_cvt = "--sysroot=" .. sysroot_para
        add_cxflags(sysroot_cvt, {force = true})
        add_ldflags(sysroot_cvt, {force = true})
    end

    local target_para = get_config("target")
    if target_para ~= "detect" and target_para then
        local target_cvt = "--target=" .. target_para
        add_cxflags(target_cvt, {force = true})
        add_ldflags(target_cvt, {force = true})
    end

    local debug_strip = get_config("debug-strip")
    if debug_strip == "all" or is_mode("release", "releasedbg") then
        add_cxflags("-fno-ident") -- also strip ident data
    end

    if is_kind("binary") then
        set_extension(".exe")
    end

    local static_link = get_config("static")
    if static_link then	
        add_ldflags("-static", {force = true})
    end


    local opt_name = get_config("winmin")
    if opt_name == "default" then	
        add_syslinks("ntdll") -- link ntdll for default

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
        add_defines("WINVER=0x0A00")
        add_ldflags("-Wl,--major-subsystem-version=10", "-Wl,--minor-subsystem-version=0", {force = true})
        add_syslinks("ntdll")
    elseif 
        opt_name == "WS12R2" or
        opt_name == "WINBLUE"
            then
        add_defines("_WIN32_WINNT=0x0603")
        add_defines("WINVER=0x0603")
        add_ldflags("-Wl,--major-subsystem-version=6", "-Wl,--minor-subsystem-version=3", {force = true})
        add_syslinks("ntdll")
    elseif 
        opt_name == "WS12" or 
        opt_name == "WIN8" 
            then
        add_defines("_WIN32_WINNT=0x0602")
        add_defines("WINVER=0x0602")
        add_ldflags("-Wl,--major-subsystem-version=6", "-Wl,--minor-subsystem-version=2", {force = true})
        add_syslinks("ntdll")
    elseif 
        opt_name == "WS08R2" or
        opt_name == "WIN7"
            then
        add_defines("_WIN32_WINNT=0x0601")
        add_defines("WINVER=0x0601")
        add_ldflags("-Wl,--major-subsystem-version=6", "-Wl,--minor-subsystem-version=1", {force = true})
        add_syslinks("ntdll")
    elseif 
        opt_name == "WS08" or
        opt_name == "VISTA" 
            then
        add_defines("_WIN32_WINNT=0x0600")
        add_defines("WINVER=0x0600")
        add_ldflags("-Wl,--major-subsystem-version=6", "-Wl,--minor-subsystem-version=0", {force = true})
        add_syslinks("ntdll")
    elseif opt_name == "WS03R2" then
        add_defines("_WIN32_WINNT=0x0502")
        add_defines("WINVER=0x0502")
        add_ldflags("-Wl,--major-subsystem-version=5", "-Wl,--minor-subsystem-version=2", {force = true})
        add_syslinks("ntdll")
    elseif 
        opt_name == "WS03" or
        opt_name == "WINXP"
    then
        add_defines("_WIN32_WINNT=0x0501")
        add_defines("WINVER=0x0501")
        add_ldflags("-Wl,--major-subsystem-version=5", "-Wl,--minor-subsystem-version=1", {force = true})
        add_syslinks("ntdll")
    elseif 
        opt_name == "WS2K" or
        opt_name == "WIN2K"
    then
        add_defines("_WIN32_WINNT=0x0500")
        add_defines("WINVER=0x0500")
        add_ldflags("-Wl,--major-subsystem-version=5", "-Wl,--minor-subsystem-version=0", {force = true})
        add_syslinks("ntdll")

    -- Windows 9x (with win32 api)

    elseif opt_name == "WINME" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WIN32_WINDOWS=0x0490")
        add_defines("WINVER=0x0490")
        add_ldflags("-Wl,--major-subsystem-version=4", "-Wl,--minor-subsystem-version=90", {force = true})
        add_syslinks("msvcrt")
        add_ldflags("-static", {force = true}) -- Forced static linking on win9x
    elseif opt_name == "WIN98" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WIN32_WINDOWS=0x0410")
        add_defines("WINVER=0x0410")
        add_ldflags("-Wl,--major-subsystem-version=4", "-Wl,--minor-subsystem-version=10", {force = true})
        add_syslinks("msvcrt")
        add_ldflags("-static", {force = true}) -- Forced static linking on win9x
    elseif opt_name == "WIN95" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WIN32_WINDOWS=0x0400")
        add_defines("WINVER=0x0400")
        add_ldflags("-Wl,--major-subsystem-version=4", "-Wl,--minor-subsystem-version=0", {force = true})
        add_syslinks("msvcrt")
        add_ldflags("-static", {force = true}) -- Forced static linking on win9x

    -- Windows NT (without win32 api)

    elseif opt_name == "NT400" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0400")
        add_defines("WINVER=0x0400")
        add_ldflags("-Wl,--major-subsystem-version=4", "-Wl,--minor-subsystem-version=0", {force = true})
        add_syslinks("ntdll")
        add_ldflags("-static", {force = true}) -- Forced static linking on winnt (witout win32)
    elseif opt_name == "NT351" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0351")
        add_defines("WINVER=0x0351")
        add_ldflags("-Wl,--major-subsystem-version=3", "-Wl,--minor-subsystem-version=51", {force = true})
        add_syslinks("ntdll")
        add_ldflags("-static", {force = true}) -- Forced static linking on winnt (witout win32)
    elseif opt_name == "NT350" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0350")
        add_defines("WINVER=0x0350")
        add_ldflags("-Wl,--major-subsystem-version=3", "-Wl,--minor-subsystem-version=50", {force = true})
        add_syslinks("ntdll")
        add_ldflags("-static", {force = true}) -- Forced static linking on winnt (witout win32)
    elseif opt_name == "NT310" then
        add_undefines("_WIN32_WINNT")
        add_defines("_WINNT=0x0310")
        add_defines("WINVER=0x0310")
        add_ldflags("-Wl,--major-subsystem-version=3", "-Wl,--minor-subsystem-version=10", {force = true})
        add_syslinks("ntdll")
        add_ldflags("-static", {force = true}) -- Forced static linking on winnt (witout win32)

    -- Windows 1.0 2.0 3.0 are not supported because it only supports x86 (16-bit).

    else
        error("invalid value")
    end

    local march = get_config("march")
    if march == "no" or march == "default" then
    else
        error("windows (unknown-windows-msvc) does not support custom march!")
    end
end