// This exists to satisfy Zig 0.16.0-dev's requirement for a root module
// when building freestanding targets.
pub fn panic(_: []const u8, _: ?*@import("std").builtin.StackTrace, _: ?usize) noreturn {
    while (true) {}
}
