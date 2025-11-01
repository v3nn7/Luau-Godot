#!/usr/bin/env python
import os
import subprocess

opts = Variables([], ARGUMENTS)

# Define our options
opts.Add(EnumVariable('target', "Compilation target", 'template_debug', ['editor', 'template_debug', 'template_release']))
opts.Add(EnumVariable('platform', "Compilation platform", '', ['', 'windows', 'linux', 'osx']))
opts.Add(EnumVariable('arch', "Compilation Architecture", '', ['', 'x86_32', 'x86_64', 'arm32', 'arm64']))
opts.Add(BoolVariable('compiledb', "Generate compilation DB (`compile_commands.json`) for external tools", False))
opts.Add(PathVariable('custom_api_file', "Path to a custom JSON API file", None, PathVariable.PathIsFile))
opts.Add(BoolVariable('generate_bindings', "Generate bindings for the Godot API", False))
opts.Add(BoolVariable('use_static_cpp', "Link the C++ library statically", True))
opts.Add(BoolVariable('use_llvm', "Use the LLVM compiler", False))

# Local dependency paths, adapt them to your setup
godot_headers_path = "extern/godot-cpp/"
cpp_bindings_path = "extern/godot-cpp/"
cpp_library = "libgodot-cpp"

target = ARGUMENTS.get('target', 'template_debug')
platform = ARGUMENTS.get('platform', '')
arch = ARGUMENTS.get('arch', '')

# Updates the environment with the option variables.
env = DefaultEnvironment()

# Update environment with options FIRST
opts.Update(env)

Help(opts.GenerateHelpText(env))

host_platform = env['PLATFORM']

# For the reference:
# - GDExtensions are written in C++. It's required to link it against the Godot cpp bindings.

# Check if we have a custom API file
if env.get('custom_api_file'):
    env.Append(CPPDEFINES=['CUSTOM_API_FILE=\\"' + env['custom_api_file'] + '\\"'])

# Platform-specific configuration
is_64bits = env['arch'] == 'x86_64'
if env['arch'] == '':
    import platform
    is_64bits = platform.machine().endswith('64')
    # Default arch string for library naming and godot-cpp build
    env['arch'] = 'x86_64' if is_64bits else 'x86_32'
    arch = env['arch']

if env['platform'] == '':
    if host_platform == 'win32':
        env['platform'] = 'windows'
    elif host_platform == 'posix':
        env['platform'] = 'linux'
    elif host_platform == 'darwin':
        env['platform'] = 'osx'

# Platform-specific flags
if env['platform'] == 'windows':
    env['CC'] = 'cl'
    env['CXX'] = 'cl'
    
    if host_platform == 'win32' or host_platform == 'win64':
        env.Append(CPPDEFINES=['WIN32', '_WIN32', '_WINDOWS', '_CRT_SECURE_NO_WARNINGS'])
        # C++17 required for Luau (string_view, optional)
        env.Append(CXXFLAGS=['/std:c++17'])
        if env['target'] in ('debug', 'template_debug'):
            env.Append(CCFLAGS=['/Zi', '/FS', '/Od', '/EHsc'])
            env.Append(LINKFLAGS=['/DEBUG'])
        else:
            env.Append(CCFLAGS=['/O2', '/EHsc'])

elif env['platform'] in ('linux', 'freebsd'):
    env['CC'] = 'gcc'
    env['CXX'] = 'g++'
    
    env.Append(CCFLAGS=['-fPIC'])
    env.Append(CXXFLAGS=['-std=c++17'])
    if env['target'] in ('debug', 'template_debug'):
        env.Append(CCFLAGS=['-g3', '-Og'])
    else:
        env.Append(CCFLAGS=['-O3'])

elif env['platform'] == 'osx':
    env['CC'] = 'clang'
    env['CXX'] = 'clang++'
    
    env.Append(CCFLAGS=['-arch', 'x86_64'])
    env.Append(CXXFLAGS=['-std=c++17'])
    env.Append(LINKFLAGS=['-arch', 'x86_64'])
    if env['target'] in ('debug', 'template_debug'):
        env.Append(CCFLAGS=['-g', '-O0'])
    else:
        env.Append(CCFLAGS=['-O3'])

# Add Luau source files
luau_sources = []

# VM sources  
luau_vm_path = "extern/luau/VM/src/"
luau_vm_sources = [
    'lapi.cpp', 'laux.cpp', 'lbaselib.cpp', 'lbitlib.cpp', 'lbuffer.cpp', 'lbuflib.cpp',
    'lbuiltins.cpp', 'lcorolib.cpp', 'ldblib.cpp', 'ldebug.cpp', 'ldo.cpp', 'lfunc.cpp',
    'lgc.cpp', 'lgcdebug.cpp', 'linit.cpp', 'lmathlib.cpp', 'lmem.cpp', 'lnumprint.cpp',
    'lobject.cpp', 'loslib.cpp', 'lperf.cpp', 'lstate.cpp', 'lstring.cpp', 'lstrlib.cpp',
    'ltable.cpp', 'ltablib.cpp', 'ltm.cpp', 'ludata.cpp', 'lutf8lib.cpp', 'lveclib.cpp',
    'lvmexecute.cpp', 'lvmload.cpp', 'lvmutils.cpp'
]

for src in luau_vm_sources:
    luau_sources.append(luau_vm_path + src)

# Compiler sources
luau_compiler_path = "extern/luau/Compiler/src/"
luau_compiler_sources = [
    'BuiltinFolding.cpp', 'Builtins.cpp', 'BytecodeBuilder.cpp', 'Compiler.cpp', 
    'ConstantFolding.cpp', 'CostModel.cpp', 'lcode.cpp', 'TableShape.cpp', 
    'Types.cpp', 'ValueTracking.cpp'
]

for src in luau_compiler_sources:
    luau_sources.append(luau_compiler_path + src)

# AST sources
luau_ast_path = "extern/luau/Ast/src/"
luau_ast_sources = [
    'Ast.cpp', 'Confusables.cpp', 'Lexer.cpp', 'Location.cpp', 'Parser.cpp', 'StringUtils.cpp', 'TimeTrace.cpp', 'Allocator.cpp', 'Cst.cpp', 'PrettyPrinter.cpp'
]

for src in luau_ast_sources:
    luau_sources.append(luau_ast_path + src)

# Analysis sources - use Glob to auto-detect all cpp files
luau_analysis_path = "extern/luau/Analysis/src/"
luau_analysis_sources = Glob(luau_analysis_path + "*.cpp")
luau_sources.extend(luau_analysis_sources)

# EqSat sources (needed by Analysis/EqSatSimplification)
luau_eqsat_path = "extern/luau/EqSat/src/"
luau_eqsat_sources = [
    'Id.cpp',
    'UnionFind.cpp',
]
for src in luau_eqsat_sources:
    luau_sources.append(luau_eqsat_path + src)

# Config sources
luau_config_path = "extern/luau/Config/src/"
luau_config_sources = Glob(luau_config_path + "*.cpp")
luau_sources.extend(luau_config_sources)

# Add include paths
env.Append(CPPPATH=[
    "src/",
    "extern/luau/VM/include/",
    "extern/luau/VM/src/",
    "extern/luau/Compiler/include/",
    "extern/luau/Ast/include/",
    "extern/luau/Analysis/include/",
    "extern/luau/Config/include/",
    "extern/luau/EqSat/include/",
    "extern/luau/Common/include/",
    godot_headers_path + "gdextension/",
    godot_headers_path + "include/",
    godot_headers_path + "gen/include/",
    godot_headers_path + "include/core/",
    godot_headers_path + "include/variant/",
])

# Set up the suffix for the shared library
if env['platform'] == 'windows':
    env['SHLIBSUFFIX'] = '.dll'
elif env['platform'] == 'linux':
    env['SHLIBSUFFIX'] = '.so'
elif env['platform'] == 'osx':
    env['SHLIBSUFFIX'] = '.dylib'

# Get the library name
library_name = "libluau-{}.{}.{}{}".format(
    env['platform'], env['target'], env['arch'], env['SHLIBSUFFIX']
)

# Build godot-cpp first - need to call it via subprocess to avoid conflicts
import os.path

# Determine godot-cpp library path based on platform
if env['platform'] == 'windows':
    godot_cpp_lib_name = "libgodot-cpp.windows.{}.{}.lib".format(env['target'], env['arch'])
elif env['platform'] == 'linux':
    godot_cpp_lib_name = "libgodot-cpp.linux.{}.{}.a".format(env['target'], env['arch'])
elif env['platform'] == 'osx':
    godot_cpp_lib_name = "libgodot-cpp.osx.{}.a".format(env['target'])
else:
    godot_cpp_lib_name = "libgodot-cpp.a"

godot_cpp_lib_path = godot_headers_path + "bin/" + godot_cpp_lib_name
force_godot_cpp_build = env.get('generate_bindings', False) or bool(env.get('custom_api_file'))

# Check if godot-cpp is already built; rebuild if forced
if force_godot_cpp_build or not os.path.exists(godot_cpp_lib_path):
    print("Building godot-cpp...")
    import subprocess
    godot_cpp_cmd = [
        "scons",
        "target=" + target,
        "platform=" + env['platform'],
        "arch=" + arch,
        "-C", godot_headers_path
    ]
    # Forward API binding options to godot-cpp so its generated bindings
    # match the Godot version used in the editor.
    if env.get('generate_bindings', False):
        godot_cpp_cmd.append("generate_bindings=yes")
    if env.get('custom_api_file'):
        godot_cpp_cmd.append("custom_api_file=" + env['custom_api_file'])
    result = subprocess.call(godot_cpp_cmd)
    if result != 0:
        print("Warning: godot-cpp build failed, continuing anyway...")

# Link with godot-cpp
if os.path.exists(godot_cpp_lib_path):
    env.Append(LIBS=[File(godot_cpp_lib_path)])
    env.Append(LIBPATH=[godot_headers_path + "bin"])
else:
    print("Warning: godot-cpp library not found, linking may fail")

# Add our source files
sources = luau_sources + Glob("src/*.cpp") + Glob("src/*/*.cpp") + Glob("src/*/*/*.cpp")

# Make the shared library
library = env.SharedLibrary(
    "bin/" + library_name,
    sources
)

Default(library)

if env.get('compiledb', False):
    try:
        env.Tool('compilation_db')
        env.CompilationDatabase()
    except Exception as e:
        print("Warning: Could not enable compilation database:", e)


