const std = @import("std");
const Builder = std.build.Builder;

pub fn build(b: *Builder) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});
    
    const exe = b.addExecutable("EngineTest", "src/main.cpp");
    exe.setTarget(target);
    exe.setBuildMode(optimize);
    
    exe.addCSourceFile("src/main.cpp", &[_][]const u8{
        "-std=c++20",
        "-Wall",
        "-Wextra",
        // Add other compiler options here if needed
    });

    exe.addCSourceFile("src/editor.cpp", &[_][]const u8{
        "-std=c++20",
        "-Wall",
        "-Wextra",
        // Add other compiler options here if needed
    });

    const include_dir = b.path("include");
    exe.addIncludeDir(include_dir);

    const raylib_version = "5.0";
    const raygui_version = "4.0";

    const raylib_repo = "https://github.com/raysan5/raylib/archive/refs/tags/" ++ raylib_version ++ ".tar.gz";
    const raygui_repo = "https://github.com/raysan5/raygui/archive/refs/tags/" ++ raygui_version ++ ".tar.gz";

    const raylib = b.addSystemLibrary("raylib", raylib_repo, .{});
    const raygui = b.addSystemLibrary("raygui", raygui_repo, .{});

    exe.linkLibrary(raylib);
    exe.linkLibrary(raygui);

    exe.install();
}