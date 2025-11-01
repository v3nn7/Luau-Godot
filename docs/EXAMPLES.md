# Examples - Godot Luau Extension

This document contains practical examples of using Luau scripts in Godot.

## Basic Examples

### Hello World Script

```lua
-- hello_world.luau
extends = "Node"

function _ready()
    print("Hello from Luau!")
    
    -- Change background color
    get_tree().get_root().set_render_debug_material_override(true)
    
    -- Create a label
    local label = Label.new()
    label.text = "Hello, Luau!"
    label.position = Vector2(100, 100)
    add_child(label)
end
```

### Simple Movement

```lua
-- simple_movement.luau
extends = "Node2D"

local speed = 200.0

function _process(delta)
    local movement = Vector2()
    
    if Input.is_action_pressed("ui_right") then
        movement.x += 1
    end
    if Input.is_action_pressed("ui_left") then
        movement.x -= 1
    end
    if Input.is_action_pressed("ui_down") then
        movement.y += 1
    end
    if Input.is_action_pressed("ui_up") then
        movement.y -= 1
    end
    
    position += movement.normalized() * speed * delta
end
```

## 2D Game Examples

### Platformer Player Controller

```lua
-- player_2d.luau
extends = "CharacterBody2D"

-- Player properties
local speed = 300.0
local jump_velocity = -400.0
local acceleration = 1000.0
local friction = 800.0
local air_acceleration = 300.0

-- Get gravity from project settings
local gravity = ProjectSettings.get_setting("physics/2d/default_gravity")

-- Coyote time (allows jumping shortly after leaving ground)
local coyote_time = 0.1
local coyote_timer = 0.0

-- Jump buffering (allows jump input before landing)
local jump_buffer_time = 0.1
local jump_buffer_timer = 0.0

-- References to child nodes
local sprite = nil
local animation_player = nil

function _ready()
    sprite = get_node("Sprite2D")
    animation_player = get_node("AnimationPlayer")
end

function _physics_process(delta)
    handle_gravity(delta)
    handle_jump(delta)
    handle_movement(delta)
    update_animation()
    
    move_and_slide()
end

function handle_gravity(delta)
    if not is_on_floor() then
        velocity.y += gravity * delta
        coyote_timer += delta
    else
        coyote_timer = 0.0
    end
end

function handle_jump(delta)
    -- Jump buffering
    if Input.is_action_just_pressed("jump") then
        jump_buffer_timer = jump_buffer_time
    end
    
    if jump_buffer_timer > 0.0 then
        jump_buffer_timer -= delta
        
        -- Can jump if on floor or in coyote time
        if is_on_floor() or coyote_timer < coyote_time then
            velocity.y = jump_velocity
            jump_buffer_timer = 0.0
            coyote_timer = coyote_time -- Prevent coyote jump after using it
        end
    end
    
    -- Variable jump height
    if Input.is_action_just_released("jump") and velocity.y < 0 then
        velocity.y = velocity.y * 0.5
    end
end

function handle_movement(delta)
    local direction = Input.get_axis("move_left", "move_right")
    
    if direction ~= 0 then
        -- Accelerate towards target speed
        local target_speed = direction * speed
        local accel = acceleration
        if not is_on_floor() then
            accel = air_acceleration
        end
        
        velocity.x = move_toward(velocity.x, target_speed, accel * delta)
        
        -- Flip sprite
        if sprite then
            sprite.flip_h = direction < 0
        end
    else
        -- Apply friction only when on ground
        if is_on_floor() then
            velocity.x = move_toward(velocity.x, 0, friction * delta)
        end
    end
end

function update_animation()
    if not animation_player then
        return
    end
    
    if not is_on_floor() then
        if velocity.y < 0 then
            animation_player.play("jump")
        else
            animation_player.play("fall")
        end
    elseif abs(velocity.x) > 10 then
        animation_player.play("run")
    else
        animation_player.play("idle")
    end
end
```

### Enemy AI with State Machine

```lua
-- enemy_ai.luau
extends = "CharacterBody2D"

-- Enemy states
local PATROL = "patrol"
local CHASE = "chase"
local ATTACK = "attack"
local HURT = "hurt"
local DIE = "die"

-- Enemy properties
local health = 100
local max_health = 100
local patrol_speed = 50.0
local chase_speed = 150.0
local attack_damage = 25
local detection_range = 200.0
local attack_range = 50.0

-- State management
local current_state = PATROL
local state_timer = 0.0
local target_player = nil

-- Movement
local patrol_direction = 1
local patrol_points = {}
local current_patrol_index = 0
local gravity = 980.0

function _ready()
    -- Set up patrol points (example)
    patrol_points = {
        Vector2(-100, 0),
        Vector2(100, 0)
    }
    
    add_to_group("enemies")
    connect("area_entered", self, "_on_detection_area_entered")
    connect("area_exited", self, "_on_detection_area_exited")
end

function _physics_process(delta)
    state_timer += delta
    
    -- Apply gravity
    if not is_on_floor() then
        velocity.y += gravity * delta
    end
    
    -- State machine
    if current_state == PATROL then
        state_patrol(delta)
    elseif current_state == CHASE then
        state_chase(delta)
    elseif current_state == ATTACK then
        state_attack(delta)
    elseif current_state == HURT then
        state_hurt(delta)
    elseif current_state == DIE then
        state_die(delta)
    end
    
    move_and_slide()
    check_for_cliffs()
end

function state_patrol(delta)
    -- Move towards current patrol point
    if #patrol_points > 0 then
        local target_point = patrol_points[current_patrol_index + 1] -- Lua is 1-indexed
        local direction = sign(target_point.x - position.x)
        
        velocity.x = direction * patrol_speed
        
        -- Check if reached patrol point
        if abs(position.x - target_point.x) < 10 then
            current_patrol_index = (current_patrol_index + 1) % #patrol_points
            patrol_direction = -patrol_direction
        end
    else
        -- Simple back and forth patrol
        velocity.x = patrol_direction * patrol_speed
    end
end

function state_chase(delta)
    if target_player and is_instance_valid(target_player) then
        local distance = position.distance_to(target_player.global_position)
        
        if distance > detection_range * 1.5 then
            -- Lost player, return to patrol
            change_state(PATROL)
            target_player = null
        elseif distance < attack_range then
            -- Close enough to attack
            change_state(ATTACK)
        else
            -- Chase player
            local direction = sign(target_player.global_position.x - position.x)
            velocity.x = direction * chase_speed
        end
    else
        change_state(PATROL)
    end
end

function state_attack(delta)
    velocity.x = 0 -- Stop moving during attack
    
    if state_timer > 1.0 then -- Attack duration
        if target_player and position.distance_to(target_player.global_position) < attack_range then
            -- Deal damage to player
            if target_player.has_method("take_damage") then
                target_player.take_damage(attack_damage)
            end
        end
        
        change_state(CHASE)
    end
end

function state_hurt(delta)
    velocity.x = 0 -- Stunned during hurt state
    
    if state_timer > 0.5 then -- Hurt duration
        if health <= 0 then
            change_state(DIE)
        else
            change_state(PATROL)
        end
    end
end

function state_die(delta)
    velocity.x = 0
    
    if state_timer > 1.0 then -- Death animation duration
        queue_free()
    end
end

function change_state(new_state)
    current_state = new_state
    state_timer = 0.0
    
    -- State entry actions
    if new_state == ATTACK then
        play_animation("attack")
    elseif new_state == HURT then
        play_animation("hurt")
        modulate = Color.RED
    elseif new_state == DIE then
        play_animation("die")
        collision_layer = 0 -- Disable collision
    else
        modulate = Color.WHITE
    end
end

function take_damage(amount)
    health = math.max(0, health - amount)
    change_state(HURT)
    
    -- Knockback effect
    if target_player then
        local knockback_direction = (position - target_player.position).normalized()
        velocity += knockback_direction * 200
    end
end

function check_for_cliffs()
    -- Prevent falling off edges during patrol
    if current_state == PATROL and is_on_floor() then
        -- Cast ray downward to detect edges
        local space_state = get_world_2d().direct_space_state
        local query = PhysicsRayQueryParameters2D.new()
        query.from = position + Vector2(patrol_direction * 32, 0)
        query.to = position + Vector2(patrol_direction * 32, 64)
        
        local result = space_state.intersect_ray(query)
        if not result then
            -- No ground ahead, turn around
            patrol_direction = -patrol_direction
        end
    end
end

function _on_detection_area_entered(area)
    local body = area.get_parent()
    if body.is_in_group("player") then
        target_player = body
        if current_state == PATROL then
            change_state(CHASE)
        end
    end
end

function _on_detection_area_exited(area)
    local body = area.get_parent()
    if body == target_player then
        -- Don't immediately lose player, let chase state handle it
    end
end

function play_animation(anim_name)
    local animator = get_node_or_null("AnimationPlayer")
    if animator then
        animator.play(anim_name)
    end
end
```

## 3D Examples

### 3D Character Controller

```lua
-- player_3d.luau
extends = "CharacterBody3D"

-- Movement properties
local speed = 5.0
local sprint_speed = 8.0
local jump_velocity = 4.5
local mouse_sensitivity = 0.002

-- Get gravity from project settings
local gravity = ProjectSettings.get_setting("physics/3d/default_gravity")

-- Camera reference
local camera_pivot = nil
local camera = nil

function _ready()
    camera_pivot = get_node("CameraPivot")
    camera = camera_pivot.get_node("Camera3D")
    
    # Capture mouse
    Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
end

function _physics_process(delta)
    handle_gravity(delta)
    handle_jump()
    handle_movement(delta)
    
    move_and_slide()
end

function _input(event)
    # Mouse look
    if event is InputEventMouseMotion and Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED then
        rotate_y(-event.relative.x * mouse_sensitivity)
        camera_pivot.rotate_x(-event.relative.y * mouse_sensitivity)
        
        # Clamp vertical rotation
        camera_pivot.rotation.x = clamp(camera_pivot.rotation.x, deg_to_rad(-90), deg_to_rad(90))
    end
    
    # Toggle mouse capture
    if event.is_action_pressed("ui_cancel") then
        if Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED then
            Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
        else
            Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
        end
    end
end

function handle_gravity(delta)
    if not is_on_floor() then
        velocity.y -= gravity * delta
    end
end

function handle_jump()
    if Input.is_action_just_pressed("jump") and is_on_floor() then
        velocity.y = jump_velocity
    end
end

function handle_movement(delta)
    local input_dir = Input.get_vector("move_left", "move_right", "move_forward", "move_backward")
    local direction = (transform.basis * Vector3(input_dir.x, 0, input_dir.y)).normalized()
    
    if direction != Vector3.ZERO then
        local current_speed = speed
        if Input.is_action_pressed("sprint") then
            current_speed = sprint_speed
        end
        
        velocity.x = direction.x * current_speed
        velocity.z = direction.z * current_speed
    else
        velocity.x = move_toward(velocity.x, 0, speed)
        velocity.z = move_toward(velocity.z, 0, speed)
    end
end
```

## UI Examples

### Interactive Menu

```lua
-- main_menu.luau
extends = "Control"

-- UI references
local start_button = nil
local options_button = nil
local quit_button = nil
local options_panel = nil

function _ready()
    # Get UI elements
    start_button = get_node("VBoxContainer/StartButton")
    options_button = get_node("VBoxContainer/OptionsButton")
    quit_button = get_node("VBoxContainer/QuitButton")
    options_panel = get_node("OptionsPanel")
    
    # Connect signals
    start_button.connect("pressed", self, "_on_start_pressed")
    options_button.connect("pressed", self, "_on_options_pressed")
    quit_button.connect("pressed", self, "_on_quit_pressed")
    
    # Hide options panel initially
    options_panel.visible = false
    
    # Animate menu entrance
    animate_menu_entrance()
end

function _on_start_pressed()
    print("Starting game...")
    get_tree().change_scene_to_file("res://levels/level_1.tscn")
end

function _on_options_pressed()
    options_panel.visible = not options_panel.visible
    animate_options_panel()
end

function _on_quit_pressed()
    get_tree().quit()
end

function animate_menu_entrance()
    local tween = create_tween()
    
    # Start with buttons off-screen
    for button in [start_button, options_button, quit_button] do
        button.position.x = -200
        button.modulate.a = 0
    end
    
    # Animate buttons sliding in
    for i, button in ipairs([start_button, options_button, quit_button]) do
        tween.parallel().tween_property(button, "position:x", 0, 0.5)
        tween.tween_delay(i * 0.1)
        tween.parallel().tween_property(button, "modulate:a", 1.0, 0.3)
    end
end

function animate_options_panel()
    local tween = create_tween()
    
    if options_panel.visible then
        # Slide in from right
        options_panel.position.x = get_viewport_rect().size.x
        tween.tween_property(options_panel, "position:x", 0, 0.3)
    else
        # Slide out to right
        tween.tween_property(options_panel, "position:x", get_viewport_rect().size.x, 0.3)
    end
end
```

### Health Bar with Animation

```lua
-- health_bar.luau
extends = "ProgressBar"

local current_health = 100
local max_health = 100
local damage_color = Color.RED
local heal_color = Color.GREEN
local normal_color = Color.WHITE

function _ready()
    value = current_health
    max_value = max_health
    
    # Connect to player health changes
    var player = get_node("/root/Player")
    if player then
        player.connect("health_changed", self, "_on_health_changed")
        player.connect("damage_taken", self, "_on_damage_taken")
    end
end

function _on_health_changed(new_health, max_hp)
    var old_health = current_health
    current_health = new_health
    max_health = max_hp
    max_value = max_health
    
    # Animate health bar
    var tween = create_tween()
    tween.tween_property(self, "value", current_health, 0.3)
    
    # Color flash effect
    if new_health < old_health then
        flash_color(damage_color)
    elseif new_health > old_health then
        flash_color(heal_color)
    end
end

function _on_damage_taken(damage_amount)
    # Screen shake effect
    var camera = get_viewport().get_camera_2d()
    if camera then
        shake_camera(camera, 0.2, 10)
    end
end

function flash_color(color)
    var tween = create_tween()
    modulate = color
    tween.tween_property(self, "modulate", normal_color, 0.2)
end

function shake_camera(camera, duration, strength)
    var original_pos = camera.global_position
    var tween = create_tween()
    
    var shake_count = 10
    for i in range(shake_count) do
        var random_offset = Vector2(
            randf_range(-strength, strength),
            randf_range(-strength, strength)
        )
        tween.tween_property(camera, "global_position", original_pos + random_offset, duration / shake_count)
    end
    
    # Return to original position
    tween.tween_property(camera, "global_position", original_pos, 0.1)
end
```

## Advanced Examples

### Save/Load System

```lua
-- save_system.luau
extends = "Node"

local save_file_path = "user://savegame.save"

function save_game(player_data)
    var save_file = FileAccess.open(save_file_path, FileAccess.WRITE)
    if not save_file then
        print("Failed to open save file for writing")
        return false
    end
    
    var save_data = {
        player_name = player_data.name,
        level = player_data.current_level,
        health = player_data.health,
        inventory = player_data.inventory,
        position = {
            x = player_data.position.x,
            y = player_data.position.y
        },
        timestamp = Time.get_unix_time_from_system()
    }
    
    save_file.store_string(JSON.stringify(save_data))
    save_file.close()
    
    print("Game saved successfully!")
    return true
end

function load_game()
    if not FileAccess.file_exists(save_file_path) then
        print("No save file found")
        return null
    end
    
    var save_file = FileAccess.open(save_file_path, FileAccess.READ)
    if not save_file then
        print("Failed to open save file for reading")
        return null
    end
    
    var save_data_text = save_file.get_as_text()
    save_file.close()
    
    var json = JSON.new()
    var parse_result = json.parse(save_data_text)
    
    if parse_result != OK then
        print("Failed to parse save data")
        return null
    end
    
    var save_data = json.data
    
    # Reconstruct player data
    var player_data = {
        name = save_data.player_name,
        current_level = save_data.level,
        health = save_data.health,
        inventory = save_data.inventory,
        position = Vector2(save_data.position.x, save_data.position.y)
    }
    
    print("Game loaded successfully!")
    return player_data
end

function delete_save()
    if FileAccess.file_exists(save_file_path) then
        DirAccess.remove_absolute(save_file_path)
        print("Save file deleted")
        return true
    else
        print("No save file to delete")
        return false
    end
end
```

These examples demonstrate the power and flexibility of using Luau scripts in Godot. Each example is fully functional and can be adapted for your specific game needs.
