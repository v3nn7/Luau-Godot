# API Reference - Godot Luau Extension

## Overview

This document provides a comprehensive reference for using Godot's API through Luau scripts.

## Script Structure

### Basic Script Template

```lua
-- Optional tool annotation
@tool

-- Base class (required)
extends = "Node2D"

-- Script variables
local health = 100
local speed = 200.0

-- Initialization
function _ready()
    print("Script initialized!")
end

-- Per-frame update
function _process(delta)
    -- Update logic here
end

-- Physics update
function _physics_process(delta)
    -- Physics logic here
end

-- Input handling
function _input(event)
    -- Input logic here
end
```

## Godot Lifecycle Methods

### Core Methods

```lua
function _ready()
    -- Called when node is ready (once)
end

function _process(delta)
    -- Called every frame
    -- delta: time since last frame (float)
end

function _physics_process(delta)
    -- Called every physics frame (usually 60 FPS)
    -- delta: physics timestep (float)
end

function _input(event)
    -- Called for input events
    -- event: InputEvent object
end

function _unhandled_input(event)
    -- Called for unhandled input events
end

function _draw()
    -- Called when node needs to be drawn (Control/CanvasItem nodes)
end
```

### Node-Specific Methods

```lua
-- For GUI nodes
function _gui_input(event)
    -- GUI-specific input handling
end

-- For collision detection
function _on_body_entered(body)
    -- Physics body entered area
end

function _on_area_entered(area)
    -- Area entered another area
end
```

## Node Management

### Getting Nodes

```lua
-- Get child node
local child = get_node("ChildNodeName")
local child = get_node("Path/To/Child")

-- Get node with NodePath
local node = get_node(NodePath("../SiblingNode"))

-- Get node or null (safe)
local maybe_node = get_node_or_null("MaybeExists")
if maybe_node then
    print("Node exists!")
end

-- Find child by name
local found = find_child("ChildName", true) -- recursive search

-- Get parent
local parent = get_parent()

-- Get scene tree
local tree = get_tree()
```

### Node Operations

```lua
-- Add child
local new_node = Node.new()
add_child(new_node)

-- Remove child
remove_child(child_node)

-- Queue for deletion
queue_free()

-- Duplicate node
local copy = duplicate()

-- Move child
move_child(child_node, 2) -- move to index 2

-- Get children
local children = get_children()
for i, child in ipairs(children) do
    print("Child " .. i .. ": " .. child.name)
end
```

## Properties and Variables

### Accessing Properties

```lua
-- Node properties
print(name)           -- node name
print(position.x)     -- position (Vector2/Vector3)
print(rotation)       -- rotation in radians
print(scale.x)        -- scale factor

-- Modifying properties
position = Vector2(100, 200)
rotation = math.rad(45) -- 45 degrees
scale = Vector2(2, 2)   -- double size

-- Visibility
visible = true
modulate = Color(1, 0, 0, 1) -- red tint
```

### Custom Properties

```lua
-- Export variables (visible in editor)
@export local player_speed = 300.0
@export local max_health = 100
@export local player_name = "Hero"

-- Groups
add_to_group("players")
remove_from_group("enemies")

if is_in_group("players") then
    print("This is a player!")
end
```

## Signals

### Connecting Signals

```lua
function _ready()
    -- Connect signal to function
    connect("signal_name", self, "on_signal_received")
    
    -- Connect with parameters
    connect("area_entered", self, "on_area_entered", [self])
    
    -- One-shot connection
    connect("timeout", self, "on_timeout", [], CONNECT_ONESHOT)
end

function on_signal_received()
    print("Signal received!")
end

function on_area_entered(area)
    print("Area entered: " .. area.name)
end
```

### Emitting Signals

```lua
-- Emit signal
emit_signal("player_died")

-- Emit with parameters
emit_signal("health_changed", health, max_health)

-- Check if signal exists
if has_signal("custom_signal") then
    emit_signal("custom_signal")
end
```

### Custom Signals

```lua
-- Define custom signals (at top of script)
signal player_died
signal health_changed(current_health, max_health)
signal item_collected(item_name, quantity)

function take_damage(amount)
    health = health - amount
    emit_signal("health_changed", health, max_health)
    
    if health <= 0 then
        emit_signal("player_died")
    end
end
```

## Input System

### Input Actions

```lua
function _input(event)
    -- Check action press
    if Input.is_action_just_pressed("jump") then
        jump()
    end
    
    -- Check action release
    if Input.is_action_just_released("fire") then
        stop_firing()
    end
    
    -- Check action held
    if Input.is_action_pressed("run") then
        run()
    end
    
    -- Get action strength (for analog inputs)
    local strength = Input.get_action_strength("accelerate")
end

function _physics_process(delta)
    -- Get input axis (returns -1, 0, or 1)
    local horizontal = Input.get_axis("move_left", "move_right")
    local vertical = Input.get_axis("move_up", "move_down")
    
    velocity.x = horizontal * speed
    velocity.y = vertical * speed
end
```

### Mouse and Touch

```lua
function _input(event)
    if event:is_action_pressed("click") then
        local mouse_pos = event.position
        print("Clicked at: " .. tostring(mouse_pos))
    end
    
    -- Mouse motion
    if event is InputEventMouseMotion then
        local delta_motion = event.relative
        camera_rotation += delta_motion.x * sensitivity
    end
end

-- Get mouse position
local mouse_pos = get_global_mouse_position()
local local_mouse_pos = get_local_mouse_position()
```

## Math and Vectors

### Vector2 Operations

```lua
-- Create vectors
local pos = Vector2(10, 20)
local zero = Vector2.ZERO
local up = Vector2.UP
local right = Vector2.RIGHT

-- Vector math
local sum = pos + Vector2(5, 5)
local scaled = pos * 2
local normalized = pos.normalized()
local length = pos.length()
local distance = pos.distance_to(Vector2(0, 0))

-- Dot and cross products
local dot = pos.dot(Vector2(1, 0))
local cross = pos.cross(Vector2(0, 1))

-- Rotation
local rotated = pos.rotated(math.rad(45))
```

### Vector3 Operations

```lua
-- Create 3D vectors
local pos3d = Vector3(10, 20, 30)
local forward = Vector3.FORWARD
local up = Vector3.UP
local right = Vector3.RIGHT

-- 3D math
local cross_product = pos3d.cross(Vector3.UP)
local reflected = pos3d.bounce(Vector3.UP)
```

### Transforms

```lua
-- 2D Transform
local transform = Transform2D()
transform.origin = Vector2(100, 100)
transform = transform.rotated(math.rad(45))

-- Apply transform to point
local transformed_point = transform * Vector2(10, 0)

-- 3D Transform
local transform3d = Transform3D()
transform3d.origin = Vector3(1, 2, 3)
transform3d = transform3d.rotated(Vector3.UP, math.rad(90))
```

## Physics

### RigidBody2D

```lua
extends = "RigidBody2D"

function _ready()
    -- Set physics properties
    mass = 10.0
    gravity_scale = 1.5
    
    -- Connect signals
    connect("body_entered", self, "on_collision")
end

function apply_force_to_center(force)
    -- Apply physics force
    apply_central_impulse(force)
end

function on_collision(body)
    print("Collided with: " .. body.name)
end
```

### CharacterBody2D

```lua
extends = "CharacterBody2D"

local speed = 300.0
local jump_velocity = -400.0
local gravity = 980.0

function _physics_process(delta)
    -- Apply gravity
    if not is_on_floor() then
        velocity.y += gravity * delta
    end
    
    -- Handle jump
    if Input.is_action_just_pressed("ui_accept") and is_on_floor() then
        velocity.y = jump_velocity
    end
    
    -- Handle movement
    local direction = Input.get_axis("ui_left", "ui_right")
    velocity.x = direction * speed
    
    -- Move and handle collisions
    move_and_slide()
    
    -- Check what we collided with
    for i = 1, get_slide_collision_count() do
        local collision = get_slide_collision(i - 1) -- Lua is 1-indexed, Godot is 0-indexed
        print("Hit: " .. collision.get_collider().name)
    end
end
```

## Scene Management

### Loading and Changing Scenes

```lua
-- Get scene tree
local tree = get_tree()

-- Change scene
tree.change_scene_to_file("res://levels/level2.tscn")

-- Reload current scene
tree.reload_current_scene()

-- Pause/unpause
tree.paused = true
tree.paused = false

-- Quit game
tree.quit()
```

### Instantiating Scenes

```lua
-- Load scene resource
local scene_resource = load("res://enemies/enemy.tscn")
local enemy = scene_resource.instantiate()

-- Add to scene
get_parent().add_child(enemy)
enemy.global_position = Vector2(100, 100)

-- Preload (compile-time loading)
local bullet_scene = preload("res://weapons/bullet.tscn")

function fire_bullet()
    local bullet = bullet_scene.instantiate()
    get_parent().add_child(bullet)
    bullet.global_position = global_position
end
```

## Resources

### Loading Resources

```lua
-- Load texture
local texture = load("res://textures/player.png")
sprite.texture = texture

-- Load audio
local audio_stream = load("res://audio/jump.ogg")
audio_player.stream = audio_stream
audio_player.play()

-- Load data files
local data = load("res://data/level_config.json")
```

### ResourceLoader

```lua
-- Check if resource exists
if ResourceLoader.exists("res://levels/secret_level.tscn") then
    print("Secret level found!")
end

-- Load with type hint
local script = ResourceLoader.load("res://scripts/player.gd", "GDScript")
```

## Timers and Tweens

### Timer Node

```lua
function _ready()
    -- Create timer
    local timer = Timer.new()
    add_child(timer)
    timer.wait_time = 2.0
    timer.one_shot = false
    timer.connect("timeout", self, "on_timer_timeout")
    timer.start()
end

function on_timer_timeout()
    print("Timer expired!")
end
```

### SceneTree Timer

```lua
function start_countdown()
    -- One-shot timer via scene tree
    local timer = get_tree().create_timer(3.0)
    timer.connect("timeout", self, "on_countdown_finished")
end

function on_countdown_finished()
    print("Countdown finished!")
end
```

## Error Handling

### Safe Operations

```lua
function safe_get_node(path)
    local node = get_node_or_null(path)
    if not node then
        print("Warning: Node not found at path: " .. path)
        return nil
    end
    return node
end

function safe_call_method(obj, method_name, ...)
    if obj and obj.has_method and obj:has_method(method_name) then
        return obj:call(method_name, ...)
    else
        print("Warning: Method " .. method_name .. " not found")
        return nil
    end
end
```

### Validation

```lua
function validate_player_data(data)
    assert(data.health > 0, "Player health must be positive")
    assert(data.speed > 0, "Player speed must be positive")
    assert(type(data.name) == "string", "Player name must be string")
end
```

## Performance Tips

### Efficient Node Access

```lua
-- Cache node references
local health_bar = nil

function _ready()
    health_bar = get_node("UI/HealthBar")
end

function update_health()
    -- Use cached reference instead of get_node() every time
    health_bar.value = current_health / max_health
end
```

### Minimize Allocations

```lua
-- Reuse vectors instead of creating new ones
local temp_vector = Vector2()

function update_position()
    temp_vector.x = target_position.x - position.x
    temp_vector.y = target_position.y - position.y
    temp_vector = temp_vector.normalized()
    
    position += temp_vector * speed * delta
end
```

This completes the API reference for using Godot through Luau scripts!
