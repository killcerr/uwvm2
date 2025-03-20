set_xmakever("2.8.2")

set_project("uwvm")

-- Version
set_version("2.0.0")
add_defines("UWVM_VERSION_X=2")
add_defines("UWVM_VERSION_Y=0")
add_defines("UWVM_VERSION_Z=0")
add_defines("UWVM_VERSION_S=0")

includes("xmake/impl.lua")
includes("xmake/platform/impl.lua")

function def_build()

    set_languages("c23", "cxx26")

	set_policy("build.c++.modules", true)

	local disable_cpp_exceptions = get_config("fno-exceptions")
	if disable_cpp_exceptions then
		set_exceptions("no-cxx")
	end

    local enable_debug_timer = get_config("debug-timer")
	if enable_debug_timer then
		add_defines("UWVM_TIMER")
	end

    local disable_int = get_config("disable-int")
	if disable_int then
		add_defines("UWVM_DISABLE_INT")
	end

    local disable_jit = get_config("disable-jit")
	if disable_jit then
		add_defines("UWVM_DISABLE_JIT")
	end

    if is_plat("windows") then
        windows_target()
    elseif is_plat("mingw") then
        mingw_target()
    else
        error("Unknown Target!")
    end
end

target("uwvm")
	set_kind("binary")
	def_build()

	add_files("src/uwvm/crtmain/main.cc")
    add_files("src/uwvm/crtmain/uwvm.cppm")
target_end()
