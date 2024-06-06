const std = @import("std");
// const raySdk = @import("raylib/src/build.zig");

pub fn build(b: *std.Build) !void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "TEST",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    b.installArtifact(exe);

    // const raylib = try raySdk.addRaylib(b, target, optimize, .{});
    // exe.addIncludePath(b.path("raylib/src"));
    // exe.linkLibrary(raylib);

    const raylib_lib = b.path("include/raylib/zig-out/lib/libraylib.a");
    const raylib_include = b.path("include/raylib/zig-out/include/");
    const raygui_include = b.path("include/raygui/src");

    exe.linkLibC();
    exe.addObjectFile(raylib_lib.dupe(b));
    exe.addIncludePath(raylib_include.dupe(b));
    exe.addIncludePath(raygui_include.dupe(b));

    const run_cmd = b.addRunArtifact(exe);

    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
