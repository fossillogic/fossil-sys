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
    dxgi_dep    = cc.find_library('dxgi', required: true)   # <-- added DXGI
elif host_machine.system() == 'linux'
    winsock_dep = []
    bcrypt_dep  = []
    dxgi_dep    = []
endif

if host_machine.system() == 'darwin'
    winsock_dep = []
    bcrypt_dep  = []
    dxgi_dep    = []
    iokit_dep = dependency('appleframeworks', modules: ['IOKit', 'CoreGraphics', 'Foundation'])
else
    iokit_dep = []
endif