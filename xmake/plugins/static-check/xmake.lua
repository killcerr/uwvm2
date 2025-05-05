task("static-check")

    set_category("plugin")

    on_run(function ()
        import("lib.detect.find_tool")
        import("core.base.task")
        import("core.base.json")
        import("core.base.process")
        import("core.base.scheduler")
        local tool = find_tool("clang-tidy")
		task.run("project", {kind = "compile_commands", outputdir = "build"})
        files = {}
		for _, v in ipairs(json.loadfile("build/compile_commands.json")) do 
            table.insert(files, v["file"])
        end
        results = {}
        coro = {}
        for _, filepath in ipairs(files) do 
            function run(filepath) 
                try
                {
                    function ()
                        local tmp = os.tmpfile()
                        os.execv("clang-tidy", {"-p", "build", "-header-filter=.*", filepath}, {stdout = tmp, stderr = tmp})
                        scheduler.co_lock("static-check")
                        results[filepath] = io.readfile(tmp)
                        scheduler.co_unlock("static-check")
                        os.rm(tmp)
                    end,
                    catch
                    {
                        function (errors)
                            print(errors)
                        end
                    }
                }
            end
            while scheduler:co_count() > 5 do
            end
            table.insert(coro, scheduler.co_start(run,filepath))
        end
        for _, co in ipairs(coro) do 
            while not co==dead do
            end
        end
    end)

    set_menu {usage = "xmake static-check [options]", description = "Static check", options = {}} 