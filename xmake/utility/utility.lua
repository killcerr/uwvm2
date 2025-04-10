import("common")

---Deriving target and modifier from arch and plat
---@param target string --Target platform
---@param toolchain string --Toolchain name
---@return string --Target platform
---@return modifier_t --adjustment function
function get_target_modifier(target, toolchain)
    ---@type modifier_table_t
    local target_list = import("target", { anonymous = true }).get_target_list()
    if target ~= "target" then
        return target, target_list[target]
    end

    ---@type table<string, any>
    local cache_info = common.get_cache()
    ---@type string, modifier_t
    local target, modifier = table.unpack(cache_info["target"] or {})
    if target and modifier then -- Already detected, returns target and modifier directly.
        return target, modifier
    end

    ---@type string | nil
    local arch = get_config("arch")
    ---@type string | nil
    local plat = get_config("plat")
    ---@type string
    local target_os = get_config("target_os") or "none"
    local message = [[Unsupported %s "%s". Please select a specific toolchain.]]

    ---Mapping xmake style arch to triplet style
    ---@type map_t
    local arch_table = {
        x86 = "i686",
        i386 = "i686",
        i686 = "i686",
        x64 = "x86_64",
        x86_64 = "x86_64",
        loong64 = "loongarch64",
        riscv64 = "riscv64",
        arm = "arm",
        armv7 = "arm",
        armv7s = "arm",
        ["arm64-v8a"] = "aarch64",
        arm64 = "aarch64",
        arm64ec = "aarch64"
    }
    local old_arch = arch
    arch = arch_table[arch]
    assert(arch, format(message, "arch", old_arch))

    if plat == "windows" and toolchain == "gcc" then
        plat = "mingw" -- gcc doesn't support msvc targets, but clang does!
    end
    ---@type map_t
    local plat_table = {
        mingw = "w64",
        msys = "w64",
        linux = "linux",
        windows = "windows",
        cross = target_os
    }
    local old_plat = plat
    plat = plat_table[plat]
    assert(plat, format(message, "plat", old_plat))

    ---@type map_t
    local x86_abi_table = {
        windows = "msvc",
        w64 = "mingw32",
        linux = "gnu",
        none = "elf"
    }
    ---@type map_t
    local linux_abi_table = { linux = "gnu" }
    ---@type table<string, map_t>
    local abi_table = {
        i686 = x86_abi_table,
        x86_64 = x86_abi_table,
        arm = {
            linux = "gnueabihf",
            none = "eabi"
        },
        aarch64 = linux_abi_table,
        riscv64 = linux_abi_table,
        loongarch64 = linux_abi_table
    }
    ---@type string
    local abi = (abi_table[arch] or {})[plat] or "unknown"

    ---@type string[]
    local field = { arch, plat, abi }
    -- Special treatment for arch-elf
    if plat == "none" and abi == "elf" then
        table.remove(field, 2)
    end
    target = table.concat(field, "-")

    modifier = target_list[target]
    cprint("detecting for target .. " .. (modifier and "${color.success}" or "${color.failure}") .. target)
    assert(modifier, format(message, "target", target))

    cache_info["target"] = { target, modifier }
    common.update_cache(cache_info)

    return target, modifier
end

---Get a list of sysroot options based on options or probing results
---@return table<string, string | string[]> | nil --Options list
function get_sysroot_option()
    local cache_info = common.get_cache()
    ---sysroot cache
    ---@type string | nil
    local sysroot = cache_info["sysroot"]
    ---Get a list of options based on sysroot
    ---@return table<string, string | string[]> --Options list
    local function get_option_list()
        local sysroot_option = "--sysroot=" .. sysroot
        -- Determine if it is libc++
        local is_libcxx = (get_config("runtimes") or ""):startswith("c++")
        local libcxx_option = is_libcxx and "-isystem" .. path.join(sysroot, "include", "c++", "v1") or nil
        return { cxflags = { sysroot_option, libcxx_option }, ldflags = sysroot_option, shflags = sysroot_option }
    end
    if sysroot == "" then
        return nil               -- Detected, no sysroot available.
    elseif sysroot then
        return get_option_list() -- Already probed, using cached sysroot
    end

    -- Check for a given sysroot or auto-detect sysroot.
    sysroot = get_config("sysroot")
    local detect = sysroot == "detect"
    -- The specified sysroot if sysroot is not “no” or “detect”.
    sysroot = (sysroot ~= "no" and not detect) and sysroot or nil
    -- If the clang toolchain is used and sysroot is not specified, then autoprobe is attempted.
    detect = detect and common.is_clang()
    cache_info["sysroot_set_by_user"] = sysroot and true or false
    if sysroot then    -- Check legitimacy if sysroot is specified
        assert(os.isdir(sysroot), string.format([[The sysroot "%s" is not a directory.]], sysroot))
    elseif detect then -- Attempted detection
        ---@type string | nil
        local prefix
        if get_config("bin") then
            prefix = path.join(string.trim(get_config("bin")), "..")
        else
            prefix = try { function() return os.iorunv("llvm-config", { "--prefix" }) end }
            prefix = prefix and string.trim(prefix)
        end
        if prefix then
            -- Try the following directories: 1. prefix/sysroot 2. prefix/... /sysroot Prefer a more localized directory
            for _, v in ipairs({ "sysroot", "../sysroot" }) do
                local dir = path.join(prefix, v)
                if os.isdir(dir) then
                    sysroot = path.normalize(dir)
                    cprint("detecting for sysroot ... ${color.success}%s", sysroot)
                    break
                end
            end
        end
    end

    -- Updating the cache
    cache_info["sysroot"] = sysroot or ""
    common.update_cache(cache_info)

    if sysroot then
        return get_option_list()
    else
        if detect then
            cprint("detecting for sysroot ... ${color.failure}no")
        end
        return nil
    end
end

---Get march options
---@param target string --Target platform
---@param toolchain string --Type of toolchain
---@note Check option legitimacy only if target and toolchain exist.
---@return string | nil --march option
function get_march_option(target, toolchain)
    local cache_info = common.get_cache()
    local option = cache_info["march"]
    if option == "" then
        return nil    -- Already detected, -march is not supported
    elseif option then
        return option -- Already probed to support the -march option
    end

    ---Detect if march is supported
    ---@type string
    local arch = get_config("march")
    if arch ~= "no" then
        local march = (arch ~= "default" and arch or "native")
        option = { "-march=" .. march }
        -- Check option legitimacy only if target and toolchain exist.
        if target and toolchain then
            import("core.tool.compiler")
            if toolchain == "clang" and target ~= "native" then
                table.insert(option, "--target=" .. target)
            end
            ---@type boolean
            local support = compiler.has_flags("cxx", table.concat(option, " "))
            local message = "checking for march ... "
            cprint(message .. (support and "${color.success}" or "${color.failure}") .. march)
            if not support then
                if arch ~= "default" then
                    raise(string.format([[The toolchain doesn't support the arch "%s"]], march))
                end
                option = ""
            else
                option = option[1]
            end
        else
            option = nil -- 未探测，设置为nil在下次进行探测
        end
    else
        option = ""
    end

    -- Updating the cache
    cache_info["march"] = option
    common.update_cache(cache_info)
    return option
end

---Getting the rtlib option
---@return string | nil --rtlib option
function get_rtlib_option()
    local config = get_config("rtlib")
    return (common.is_clang() and config ~= "default") and "-rtlib=" .. config or nil
end

---Getting the unwindlib option
---@return string | nil --unwindlib option
function get_unwindlib_option()
    local config = get_config("unwindlib")
    local force = config:startswith("force")
    local lib = force and string.sub(config, 7, #config) or config
    local option = config ~= "default" and "-unwindlib=" .. lib or nil
    return (common.is_clang() and (force or get_config("rtlib") == "compiler-rt")) and option or nil
end

---Mapping mode to cmake style
---@param mode string --xmake style compilation mode
---@return string | nil --cmake style compilation mode
function get_cmake_mode(mode)
    ---@type map_t
    local table = { debug = "Debug", release = "Release", minsizerel = "MinSizeRel", releasedbg = "RelWithDebInfo" }
    return table[mode]
end
