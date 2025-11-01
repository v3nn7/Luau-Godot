-- Player Controller Script
-- extends defines the base class this script extends from
extends = "CharacterBody2D"

-- Script properties (will be exposed in the editor)
local speed = 300.0
local jump_velocity = -400.0

-- Godot's gravity constant
local gravity = ProjectSettings.get_setting("physics/2d/default_gravity")

-- Called when the node is ready
function _ready()
    print("Player Controller initialized!")
end

-- Called every physics frame
function _physics_process(delta)
    -- Add gravity
    if not is_on_floor() then
        velocity.y = velocity.y + gravity * delta
    end

    -- Handle jump
    if Input.is_action_just_pressed("ui_accept") and is_on_floor() then
        velocity.y = jump_velocity
    end

    -- Handle horizontal movement
    local direction = Input.get_axis("ui_left", "ui_right")
    if direction ~= 0 then
        velocity.x = direction * speed
    else
        velocity.x = move_toward(velocity.x, 0, speed)
    end

    move_and_slide()
end

-- Called every frame
function _process(delta)
    -- Handle non-physics related updates here
    update_animation()
end

-- Handle input events
function _input(event)
    if Input.is_action_just_pressed("ui_select") then
        print("Player selected!")
    end
end

-- Custom function for animation updates
function update_animation()
    local animated_sprite = get_node("AnimatedSprite2D")
    
    if animated_sprite then
        if velocity.x > 0 then
            animated_sprite.flip_h = false
            animated_sprite.play("run")
        elseif velocity.x < 0 then
            animated_sprite.flip_h = true
            animated_sprite.play("run")
        else
            animated_sprite.play("idle")
        end
        
        if not is_on_floor() then
            animated_sprite.play("jump")
        end
    end
end

-- Signal handler example
function on_area_entered(area)
    print("Player entered area: " .. area.name)
    
    if area.name == "Goal" then
        print("Level completed!")
        get_tree().change_scene_to_file("res://levels/next_level.tscn")
    end
end
