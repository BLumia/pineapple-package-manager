project('ppkg', ['cpp'], default_options: ['cpp_std=c++17'])

libzip = dependency('libzip', static:true)

executable('ppkg', 'main.cpp', dependencies: libzip, cpp_args: ['-DZIP_STATIC'], link_args: ['-static'])
