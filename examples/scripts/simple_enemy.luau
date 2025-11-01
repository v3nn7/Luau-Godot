-- Simple Enemy Script
extends = "RigidBody2D"

-- Enemy properties
local health = 100
local damage = 25
local patrol_speed = 50.0
local chase_speed = 150.0
local detection_range = 200.0

-- State management
local state = "patrol"
local player = nil
local patrol_direction = 1

function _ready()
    print("Enemy spawned with " .. health .. " health")
    
    -- Connect to signals
    connect("body_entered", self, "on_body_entered")
    
    -- Set collision detection
    gravity_scale = 1.0
end

function _physics_process(delta)
    if state == "patrol" then
        patrol_behavior(delta)
    elseif state == "chase" then
        chase_behavior(delta)
    elseif state == "attack" then
        attack_behavior(delta)
    end
    
    check_for_player()
end

function patrol_behavior(delta)
    -- Simple back and forth patrol
    local velocity = linear_velocity
    velocity.x = patrol_direction * patrol_speed
    linear_velocity = velocity
    
    -- Change direction at edges (simplified)
    if is_on_wall() then
        patrol_direction = -patrol_direction
        flip_sprite()
    end
end

function chase_behavior(delta)
    if player and player.is_valid() then
        local direction = (player.global_position - global_position).normalized()
        local velocity = linear_velocity
        velocity.x = direction.x * chase_speed
        linear_velocity = velocity
        
        -- Check if close enough to attack
        local distance = global_position.distance_to(player.global_position)
        if distance < 50.0 then
            state = "attack"
        end
    else
        state = "patrol"
        player = nil
    end
end

function attack_behavior(delta)
    if player and player.is_valid() then
        local distance = global_position.distance_to(player.global_position)
        if distance > 80.0 then
            state = "chase"
        else
            -- Perform attack
            deal_damage_to_player()
        end
    else
        state = "patrol"
        player = nil
    end
end

function check_for_player()
    -- Raycast or area detection for player
    local space_state = get_world_2d().direct_space_state
    if space_state then
        local query = PhysicsRayQueryParameters2D.new()
        query.from = global_position
        query.to = global_position + Vector2(detection_range * patrol_direction, 0)
        query.collision_mask = 1  -- Player collision layer
        
        local result = space_state.intersect_ray(query)
        if result and result.collider and result.collider.is_in_group("player") then
            if state == "patrol" then
                state = "chase"
                player = result.collider
                print("Player detected! Switching to chase mode.")
            end
        end
    end
end

function deal_damage_to_player()
    if player and player.has_method("take_damage") then
        player.take_damage(damage)
        print("Enemy dealt " .. damage .. " damage to player")
    end
end

function take_damage(amount)
    health = health - amount
    print("Enemy took " .. amount .. " damage, health: " .. health)
    
    if health <= 0 then
        die()
    else
        -- Knockback effect
        apply_impulse(Vector2(0, 0), Vector2(100, -50))
    end
end

function die()
    print("Enemy died!")
    
    -- Drop items or trigger events
    spawn_pickup()
    
    -- Remove from scene
    queue_free()
end

function spawn_pickup()
    local pickup_scene = preload("res://items/health_pickup.tscn")
    if pickup_scene then
        local pickup = pickup_scene.instantiate()
        get_parent().add_child(pickup)
        pickup.global_position = global_position
    end
end

function flip_sprite()
    local sprite = get_node("Sprite2D")
    if sprite then
        sprite.flip_h = patrol_direction < 0
    end
end

function on_body_entered(body)
    if body.is_in_group("player") then
        player = body
        if state == "patrol" then
            state = "chase"
            print("Player entered detection area!")
        end
    end
end

-- Signal connections for area detection
function _on_detection_area_body_entered(body)
    on_body_entered(body)
end

function _on_detection_area_body_exited(body)
    if body == player then
        state = "patrol"
        player = nil
        print("Player left detection area, returning to patrol")
    end
end
