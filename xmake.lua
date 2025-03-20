set_xmakever("2.8.2")

set_project("uwvm")

set_languages("c23", "cxx26")

-- Version
set_version("2.0.0")
add_defines("UWVM_VERSION_X=2")
add_defines("UWVM_VERSION_Y=0")
add_defines("UWVM_VERSION_Z=0")
add_defines("UWVM_VERSION_S=0")

includes("xmake/impl.lua")