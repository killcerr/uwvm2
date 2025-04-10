import("core.cache.detectcache")
---@alias map_t table<string, string>

---Determine if a toolchain is clang
---@return boolean --Is the clang toolchain
function is_clang()
    return string.find(get_config("toolchain") or "", "clang", 1, true) ~= nil
end

---Cache keys used in this module
local cache_key = "toolchain.utility"

---Getting Cache Information
---@return table<string, any> --Cache Information Table
function get_cache()
    local cache_info = detectcache:get(cache_key)
    if not cache_info then
        cache_info = {}
        detectcache:set(cache_key, cache_info)
    end
    return cache_info
end

---Updating Cache Information
---@param cache_info table --Cache information to be saved
---@return nil
function update_cache(cache_info)
    detectcache:set(cache_key, cache_info)
    detectcache:save()
end
