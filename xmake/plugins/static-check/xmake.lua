task("static-check")

    set_category("plugin")

    on_run(function ()
        import("core.base.task")
        import("core.base.json")
        import("core.base.scheduler")
        import("async.runjobs")
		task.run("project", {kind = "compile_commands", outputdir = "build"})
        local files = {}
		for _, v in ipairs(json.loadfile("build/compile_commands.json")) do 
            table.insert(files, v["file"])
        end
        local results = {}
        runjobs("clang-tidy",function (index) 
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
            run(files[index])
        end, {total = #files, comax = 8})
        for filepath, content in pairs(results) do 
            printf("[clang-tidy] %s\n%s\n", filepath, content)
        end
    end)

    set_menu {usage = "xmake static-check", description = "Static check", options = {}} 