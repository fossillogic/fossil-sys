dir = include_directories('.')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'c')
add_project_arguments('-D_POSIX_C_SOURCE=200112L', language: 'cpp')
cc = meson.get_compiler('c')

libdl = cc.find_library('dl', required : false)

# ------------------------------
# Platform-specific dependencies
# ------------------------------
if host_machine.system() == 'windows'
    winsock_dep = cc.find_library('ws2_32', required: true)
    bcrypt_dep  = cc.find_library('bcrypt', required: true)
else
    winsock_dep = []
    bcrypt_dep  = []
endif

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
    dependencies: [libdl, winsock_dep, bcrypt_dep,dependency('threads')],
    include_directories: dir)

fossil_sys_dep = declare_dependency(
    link_with: [fossil_sys_lib],
    dependencies: [dependency('threads')],
    include_directories: dir)

meson.override_dependency('fossil-sys', fossil_sys_dep)