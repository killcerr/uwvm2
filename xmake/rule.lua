includes("option.lua")

---@type string | nil
local debug_strip = get_config("debug-strip")

if debug_strip == "no" then -- not strip
    debug_strip = nil
end

---@type boolean
local enable_lto = get_config("enable-lto")

rule("debug", function()
    on_load(function(target)
        target:set("symbols", "debug")
        target:set("optimize", "none")
        target:add("defines", "DEBUG", "_DEBUG")
        target:set("strip", debug_strip)
    end)
end)

rule("release", function()
    on_load(function(target)
        target:add("defines", "NDEBUG")
        target:set("optimize", "fastest")
        target:set("strip", "all")
        target:set("policy", "build.optimization.lto", enable_lto)
    end)
end)

rule("minsizerel", function()
    on_load(function(target)
        target:add("defines", "NDEBUG")
        target:set("optimize", "smallest")
        target:set("strip", "all")
        target:set("policy", "build.optimization.lto", enable_lto)
    end)
end)

rule("releasedbg", function()
    on_load(function(target)
        target:set("optimize", "fastest")
        target:set("symbols", "debug")
        target:set("policy", "build.optimization.lto", enable_lto)
        target:set("strip", debug_strip)
    end)
end)


---rule tables
---@type string[]
support_rules_table = { "debug", "release", "minsizerel", "releasedbg" }