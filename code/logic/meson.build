dir = include_directories('.')

subdir('fossil')

fossil_sys_lib = library('fossil_sys',
    files(
        'memory.c',
        'hostinfo.c',
        'syscall.c',
        'dynamic.c',
        'process.c',
        'bitwise.c',
        'event.c',
        'env.c'),
    install: true,
    dependencies: [platform_deps, dependency('threads')],
    include_directories: dir)

fossil_sys_dep = declare_dependency(
    link_with: [fossil_sys_lib],
    include_directories: dir)

meson.override_dependency('fossil-sys', fossil_sys_dep)