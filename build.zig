const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .x86_64,
        .os_tag = .freestanding,
        .abi = .none,
    });

    const optimize = b.standardOptimizeOption(.{});

    const waks_cstd_dep = b.dependency("waks_cstd", .{
        .target = target,
        .optimize = optimize,
    });

    const kernel = b.addExecutable(.{
        .name = "kernel.bin",
        .root_module = b.createModule(.{
            .root_source_file = b.path("kernel/root.zig"),
            .target = target,
            .optimize = optimize,
        }),
    });

    kernel.root_module.linkLibrary(waks_cstd_dep.artifact("waks_cstd"));

    kernel.root_module.addCSourceFile(.{
        .file = b.path("kernel/main.c"),
        .flags = &.{ "-std=c11", "-Werror" },
    });

    kernel.root_module.addIncludePath(waks_cstd_dep.path("include"));

    kernel.root_module.red_zone = false;
    kernel.root_module.pic = false;
    kernel.bundle_compiler_rt = true;
    kernel.root_module.omit_frame_pointer = false;

    kernel.root_module.addCMacro("WAKS_FREESTANDING", "1");
    kernel.root_module.addAssemblyFile(b.path("boot/boot.s"));

    kernel.linkage = .static;
    kernel.setLinkerScript(b.path("kernel/linker.ld"));

    b.installArtifact(kernel);
}
