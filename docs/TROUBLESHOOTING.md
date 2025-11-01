# Troubleshooting - Godot Luau Extension

## Installation Issues

### Extension Not Loading

**Problem**: Godot doesn't recognize the Luau extension
```
Failed to load extension: luau
```

**Solutions**:
1. Check that `luau.gdextension` is in your project root
2. Verify the library path in `.gdextension` matches your compiled binaries
3. Ensure the binary was compiled for the correct platform/architecture
4. Check Godot version compatibility (requires 4.6+)

### Missing Dependencies

**Problem**: DLL/shared library errors on startup
```
The specified module could not be found
```

**Solutions**:
1. **Windows**: Install Visual C++ Redistributable 2019+
2. **Linux**: Install missing system libraries:
   ```bash
   sudo apt install libc6-dev libstdc++6
   ```
3. **macOS**: Update macOS to compatible version

### Architecture Mismatch

**Problem**: Extension fails to load with architecture error

**Solutions**:
1. Check your Godot architecture:
   ```bash
   godot --version
   ```
2. Recompile for matching architecture:
   ```bash
   # For 64-bit Godot
   scons arch=x86_64
   
   # For Apple Silicon
   scons arch=arm64 platform=osx
   ```

## Compilation Issues

### SCons Not Found

**Problem**: `'scons' is not recognized as an internal or external command`

**Solutions**:
```bash
# Install SCons
pip install scons

# Or use Python module
python -m pip install scons
python -m SCons

# Verify installation
scons --version
```

### Compiler Issues

**Problem**: Compiler not found or incompatible

**Windows Solutions**:
```bash
# Use Visual Studio Developer Command Prompt
# or set environment variables:
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

# Alternative: Install Build Tools only
# Download "Build Tools for Visual Studio"
```

**Linux Solutions**:
```bash
# Install GCC/G++
sudo apt install build-essential

# Or install clang
sudo apt install clang

# Use specific compiler
scons CXX=clang++
```

### Submodule Issues

**Problem**: Missing godot-cpp or Luau sources
```
fatal error: 'godot_cpp/core/defs.hpp' file not found
```

**Solutions**:
```bash
# Initialize submodules
git submodule update --init --recursive

# Force update if corrupted
git submodule foreach --recursive git clean -xfd
git submodule foreach --recursive git reset --hard
git submodule update --force --recursive
```

## Runtime Issues

### Script Not Recognized

**Problem**: `.luau` files not recognized as scripts in Godot

**Solutions**:
1. Ensure extension is properly loaded (check project settings)
2. Restart Godot after installing extension
3. Check that `LuauScriptLanguage` is registered:
   ```
   Go to Project Settings -> Languages -> Scripting
   ```

### Compilation Errors

**Problem**: Luau syntax errors
```
Luau compilation failed: [string "script.luau"]:5: syntax error near 'end'
```

**Solutions**:
1. Check Luau syntax (similar to Lua but with differences)
2. Common mistakes:
   ```lua
   -- Wrong: using 'then' without 'if'
   local x = condition and value1 or value2
   
   -- Correct: proper conditional
   local x
   if condition then
       x = value1
   else
       x = value2
   end
   ```

### Runtime Errors

**Problem**: Script execution fails
```
Luau execution error: attempt to index a nil value
```

**Common Solutions**:
1. **Nil checking**:
   ```lua
   local node = get_node_or_null("Path/To/Node")
   if node then
       node.visible = false
   end
   ```

2. **Method existence checking**:
   ```lua
   if obj.has_method and obj:has_method("custom_method") then
       obj:custom_method()
   end
   ```

3. **Safe property access**:
   ```lua
   local pos = node and node.position or Vector2.ZERO
   ```

### Performance Issues

**Problem**: Poor frame rate with Luau scripts

**Solutions**:
1. **Profile your code**:
   - Use Godot's built-in profiler
   - Identify bottlenecks in `_process` functions

2. **Optimize heavy operations**:
   ```lua
   -- Bad: Heavy operations in _process
   function _process(delta)
       for i = 1, 1000 do
           complex_calculation()
       end
   end
   
   -- Good: Spread over multiple frames
   local calculation_index = 1
   function _process(delta)
       -- Do only small chunk per frame
       for i = 1, 10 do
           if calculation_index <= 1000 then
               complex_calculation(calculation_index)
               calculation_index = calculation_index + 1
           end
       end
   end
   ```

3. **Cache expensive lookups**:
   ```lua
   -- Bad: get_node() every frame
   function _process(delta)
       get_node("UI/HealthBar").value = health
   end
   
   -- Good: Cache reference
   local health_bar
   function _ready()
       health_bar = get_node("UI/HealthBar")
   end
   
   function _process(delta)
       health_bar.value = health
   end
   ```

## API Issues

### Missing Godot API

**Problem**: Godot methods not available in Luau
```
attempt to call a nil value (global 'get_tree')
```

**Solutions**:
1. Check if method is bound in `godot_api_bindings.cpp`
2. Use alternative approach:
   ```lua
   -- If get_tree() not available, access through node
   local tree = get_parent().get_tree()
   ```

3. Add missing bindings (for developers):
   ```cpp
   // In godot_api_bindings.cpp
   lua_pushcfunction(L, lua_get_tree, "get_tree");
   lua_setglobal(L, "get_tree");
   ```

### Type Conversion Issues

**Problem**: Data not converting properly between Luau and Godot

**Solutions**:
1. **Vector conversion**:
   ```lua
   -- Ensure proper Vector2 usage
   local pos = Vector2(x, y)  -- Not {x = x, y = y}
   ```

2. **Array/Dictionary conversion**:
   ```lua
   -- Use proper Godot types
   local array = Array()
   array.append("item1")
   array.append("item2")
   
   local dict = Dictionary()
   dict["key"] = "value"
   ```

### Signal Connection Issues

**Problem**: Signals not connecting or firing

**Solutions**:
1. **Check signal existence**:
   ```lua
   if has_signal("custom_signal") then
       connect("custom_signal", self, "on_custom_signal")
   end
   ```

2. **Verify method existence**:
   ```lua
   function _ready()
       if has_method("on_timeout") then
           timer.connect("timeout", self, "on_timeout")
       end
   end
   ```

3. **One-shot connections**:
   ```lua
   -- For signals that should only fire once
   connect("player_died", self, "on_death", [], CONNECT_ONESHOT)
   ```

## Memory Issues

### Memory Leaks

**Problem**: Memory usage grows over time

**Solutions**:
1. **Proper cleanup**:
   ```lua
   function _exit_tree()
       -- Disconnect signals
       if signal_connected then
           disconnect("signal_name", self, "handler")
       end
       
       -- Free resources
       if texture_resource then
           texture_resource.queue_free()
       end
   end
   ```

2. **Avoid circular references**:
   ```lua
   -- Bad: circular reference
   node_a.reference_to_b = node_b
   node_b.reference_to_a = node_a
   
   -- Good: use weak references or break cycles
   node_a.reference_to_b = weakref(node_b)
   ```

### Resource Management

**Problem**: Resources not being freed

**Solutions**:
```lua
-- Preload vs load
local preloaded = preload("res://texture.png")  -- Loaded at compile time
local loaded = load("res://texture.png")        -- Loaded at runtime

-- Free when done
function cleanup()
    if dynamic_texture then
        dynamic_texture.queue_free()
        dynamic_texture = null
    end
end
```

## Debugging Tips

### Enable Verbose Logging

```lua
function _ready()
    print("Script starting...")
    
    -- Log all method calls
    local original_process = _process
    _process = function(delta)
        print("_process called with delta: " .. delta)
        original_process(delta)
    end
end
```

### Use Assertions

```lua
function move_player(direction)
    assert(direction, "Direction cannot be nil")
    assert(type(direction) == "table", "Direction must be Vector2")
    assert(direction.x and direction.y, "Direction must have x and y components")
    
    position += direction * speed
end
```

### Breakpoint Alternative

```lua
function debug_pause()
    get_tree().paused = true
    print("PAUSED - Press F6 to continue")
    
    -- Wait for input in _input
end

function _input(event)
    if event.is_action_pressed("debug_continue") then
        get_tree().paused = false
    end
end
```

## Platform-Specific Issues

### Windows

**Problem**: Antivirus blocking compilation
- **Solution**: Add project folder to antivirus exceptions

**Problem**: Long path issues
- **Solution**: Use shorter project paths or enable long path support in Windows

### Linux

**Problem**: Permission denied errors
- **Solution**: Check file permissions:
  ```bash
  chmod +x godot
  chmod -R 755 project_folder/
  ```

**Problem**: Missing X11 libraries
- **Solution**: Install X11 development packages:
  ```bash
  sudo apt install libx11-dev libxrandr-dev libxi-dev
  ```

### macOS

**Problem**: Code signing issues
- **Solution**: Disable Gatekeeper temporarily:
  ```bash
  sudo spctl --master-disable
  # Re-enable after testing
  sudo spctl --master-enable
  ```

**Problem**: Apple Silicon compatibility
- **Solution**: Use Universal Binary or compile for specific architecture:
  ```bash
  scons arch=arm64 platform=osx  # For M1/M2
  scons arch=x86_64 platform=osx # For Intel
  ```

## Getting Help

### Before Reporting Issues

1. **Check logs**: Look in Godot's output console
2. **Minimal reproduction**: Create smallest possible test case
3. **Version information**: Include Godot version, OS, extension version
4. **Build information**: Include compilation flags and environment

### Where to Get Help

- **GitHub Issues**: For bugs and feature requests
- **GitHub Discussions**: For questions and general help
- **Godot Community**: Discord, Reddit, Forums
- **Documentation**: Check API reference and examples

### Reporting Bugs

Include this information:
```
**Environment:**
- OS: Windows 10 / Ubuntu 20.04 / macOS 12
- Godot Version: 4.6.0
- Extension Version: 1.0.0
- Architecture: x86_64

**Build Information:**
- Compiler: MSVC 2022 / GCC 11 / Clang 14
- SCons Version: 4.0.0
- Build Command: scons target=template_debug platform=windows

**Issue Description:**
[Describe the problem clearly]

**Reproduction Steps:**
1. Step one
2. Step two
3. Expected result vs actual result

**Code Sample:**
[Minimal Luau script that reproduces the issue]

**Logs:**
[Include relevant console output]
```
