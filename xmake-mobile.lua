target("raylib-game-web")
    set_kind("binary")
    set_languages("cxx17")
    set_toolchains("emcc")

    add_defines("PLATFORM_WEB")
    add_files("cpp/*.cc", "cpp/*.c", "cpp/*.cpp")
    add_includedirs("raylib-5.5_webassembly/include", {public=true})
    add_linkdirs("raylib-5.5_webassembly/lib")
    add_links("raylib")

    -- Create the build directory using xmake's path functions
    before_build(function (target)
        import("core.base.option")
        import("lib.detect.find_tool")
        
        -- Create build/web directory
        local buildir = path.join("build", "web")
        if not os.exists(buildir) then
            os.mkdir(buildir)
        end
    end)

    add_ldflags(
        "-s USE_GLFW=3",
        "-s ASYNCIFY",
        "-s WASM=1",
        "-s ALLOW_MEMORY_GROWTH=1",
        "-s FORCE_FILESYSTEM=1",
        "--shell-file=./shell.html",
        "--preload-file=assets",
        "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','HEAPF32']",

        "-s ASSERTIONS=2",
        "-gsource-map",       

        "-o", "build/web/index.html",
        {force=true}
    )