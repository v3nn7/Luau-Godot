# Godot Luau Extension

A complete Luau scripting language integration for Godot Engine 4.6+ using GDExtension.

## Features

- ✅ **Complete Integration** - Luau as a first-class scripting language in Godot
- ✅ **`.luau` File Support** - Attach scripts to nodes in the editor
- ✅ **Godot API Access** - Full access to Godot methods, properties, and signals
- ✅ **Lifecycle Methods** - Support for `_ready`, `_process`, `_physics_process`, `_input`
- ✅ **Inheritance** - Support for `extends = "Node"` and other base classes
- ✅ **Sandboxing** - Safe script execution
- ✅ **Error Handling** - Integration with Godot's error system
- ✅ **Cross-Platform** - Windows, Linux, macOS

## Requirements

- **Godot 4.6** or newer
- **SCons** - Build system
- **Python 3.6+** - For SCons
- **C++17 Compiler**:
  - Windows: Visual Studio 2019+ or clang
  - Linux: GCC 7+ or clang
  - macOS: Xcode 10+ or clang

## Quick Start

### 1. Download & Compile

```bash
git clone --recursive https://github.com/your-username/godot-luau-extension.git
cd godot-luau-extension

# Build for your platform
scons target=template_debug platform=windows arch=x86_64  # Windows
scons target=template_debug platform=linux arch=x86_64    # Linux
scons target=template_debug platform=osx                  # macOS
```

### 2. Install in Godot Project

1. Copy `luau.gdextension` to your project root
2. Copy the `bin/` folder with compiled libraries
3. Copy `icons/` folder (optional)
4. Launch Godot - the extension will auto-load

### 3. Create Your First Luau Script

Create a `.luau` file:

```lua
-- player.luau
extends = "CharacterBody2D"

local speed = 300.0
local jump_velocity = -400.0

function _ready()
    print("Player ready!")
end

function _physics_process(delta)
    -- Handle movement
    local direction = Input.get_axis("ui_left", "ui_right")
    if direction ~= 0 then
        velocity.x = direction * speed
    else
        velocity.x = 0
    end
    
    -- Handle jump
    if Input.is_action_just_pressed("ui_accept") and is_on_floor() then
        velocity.y = jump_velocity
    end
    
    -- Apply gravity
    if not is_on_floor() then
        velocity.y = velocity.y + get_gravity().y * delta
    end
    
    move_and_slide()
end
```

## Godot API in Luau

The extension provides access to most Godot APIs:

```lua
-- Nodes and scene management
local node = get_node("Player")
local tree = get_tree()
tree.change_scene_to_file("res://levels/level2.tscn")

-- Vectors and math
local pos = Vector2(100, 200)
local vel = Vector3(1, 0, 0)

-- Input handling
if Input.is_action_pressed("ui_right") then
    position.x = position.x + speed * delta
end

-- Signals
connect("area_entered", self, "on_area_entered")

function on_area_entered(area)
    print("Entered area: " .. area.name)
end

-- Timers and resources
local timer = create_timer(2.0)
timer.connect("timeout", self, "on_timeout")

local texture = load_resource("res://icons/player.png")
```

## Script Structure

```lua
-- Tool script comment (optional)
@tool

-- Base class definition
extends = "Node2D"

-- Script variables
local health = 100
local speed = 200.0

-- Godot lifecycle functions
function _ready()
    -- Initialization
end

function _process(delta)
    -- Per-frame update
end

function _physics_process(delta)
    -- Physics per-frame update
end

function _input(event)
    -- Input handling
end

-- Custom functions
function custom_function()
    -- Your logic here
end
```

## Project Structure

```
godot-luau-extension/
├── src/                          # C++ source code
│   ├── register_types.cpp        # GDExtension entry point
│   ├── luau_script_language/     # Script language class
│   ├── luau_script/              # Script class
│   └── bindings/                 # Godot API bindings
├── extern/                       # External libraries
│   ├── luau/                     # Luau sources
│   └── godot-cpp/                # Godot C++ bindings
├── examples/                     # Examples
│   ├── scripts/                  # Sample .luau scripts
│   └── scenes/                   # Sample scenes
├── icons/                        # Editor icons
├── bin/                          # Compiled libraries
├── docs/                         # Documentation
├── luau.gdextension             # Extension configuration
└── SConstruct                   # Build script
```

## Documentation

- **[Build Instructions](BUILD.md)** - Detailed compilation guide
- **[API Reference](API.md)** - Complete API documentation
- **[Examples](../examples/)** - Sample scripts and scenes
- **[Troubleshooting](TROUBLESHOOTING.md)** - Common issues and solutions

## Examples

In the `examples/` folder you'll find:

- **player_controller.luau** - 2D player controller
- **simple_enemy.luau** - Simple AI enemy
- **test_scene.tscn** - Test scene

## Debugging

### Luau Compilation Errors

Syntax errors are displayed in Godot's console:

```
Luau compilation failed: [string "player.luau"]:5: syntax error near 'end'
```

### Runtime Errors

Runtime errors are caught and logged:

```
Luau execution error: attempt to call method 'invalid_method' (a nil value)
```

### Performance

- Check script performance using Godot's profiler
- Avoid heavy operations in `_process` and `_physics_process`
- Use `_ready` for one-time initialization

## Limitations

- **No Step Debugger** - Currently no step-by-step debugging
- **Limited Metadata** - Less type information than GDScript
- **No Built-in Editor** - Use external editor for .luau files

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

## Support

- **Issues** - Report bugs and feature requests
- **Discussions** - Ask questions and share ideas
- **Wiki** - Additional documentation and tutorials

## Acknowledgments

- **Roblox** - For creating the Luau language
- **Godot Engine** - For the amazing game engine
- **Community** - For support and feedback
