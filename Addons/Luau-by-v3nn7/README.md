# Luau for Godot

This Godot addon integrates the Luau scripting language into the Godot engine.

## Recent Changes

The plugin has been updated to prevent crashes when attaching scripts. The following changes were made:

1. Added automatic bin directory creation if it doesn't exist
2. Added a script handler that safely handles script attachment
3. Added better error messages for missing dependencies
4. Created placeholder libraries to prevent crashes during development

## Usage

To use Luau scripts in your Godot project:

1. Enable the plugin in Project Settings
2. Create a new script with the .luau extension
3. Use the LuauScriptHandler to safely attach scripts to nodes

## Creating Luau Scripts

```lua
-- Example Luau script (example.luau)
extends = "Node2D"

-- Your code goes here
function _ready()
    print("Hello from Luau!")
end
```

## Safe Script Attachment

To safely attach a Luau script to a node, use the LuauScriptHandler:

```gdscript
# In your GDScript code:
var handler = $LuauScriptHandler
handler.attach_script_safely(your_node, "res://path/to/script.luau")
```
