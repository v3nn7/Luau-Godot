#include "godot_class_bindings.h"
#include "godot_api_bindings.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/reference_rect.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/os.hpp>
#include <godot_cpp/classes/time.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/display_server.hpp>
#include <godot_cpp/classes/audio_server.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/physics_server2d.hpp>
#include <godot_cpp/classes/physics_server3d.hpp>
#include <godot_cpp/classes/navigation_server2d.hpp>
#include <godot_cpp/classes/navigation_server3d.hpp>
#include <godot_cpp/classes/xr_server.hpp>
#include <godot_cpp/classes/camera_server.hpp>

// UI Classes
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/text_edit.hpp>
#include <godot_cpp/classes/rich_text_label.hpp>
#include <godot_cpp/classes/progress_bar.hpp>
#include <godot_cpp/classes/slider.hpp>
#include <godot_cpp/classes/spin_box.hpp>
#include <godot_cpp/classes/check_box.hpp>
#include <godot_cpp/classes/option_button.hpp>
#include <godot_cpp/classes/menu_button.hpp>
#include <godot_cpp/classes/popup_menu.hpp>
#include <godot_cpp/classes/tree.hpp>
#include <godot_cpp/classes/item_list.hpp>
#include <godot_cpp/classes/tab_container.hpp>
#include <godot_cpp/classes/split_container.hpp>
#include <godot_cpp/classes/scroll_container.hpp>
#include <godot_cpp/classes/panel.hpp>
#include <godot_cpp/classes/panel_container.hpp>

// 2D Classes
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/rigid_body2d.hpp>
#include <godot_cpp/classes/static_body2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
#include <godot_cpp/classes/collision_polygon2d.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>
#include <godot_cpp/classes/camera2d.hpp>
#include <godot_cpp/classes/parallax_background.hpp>
#include <godot_cpp/classes/parallax_layer.hpp>
#include <godot_cpp/classes/tile_map.hpp>

// 3D Classes  
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/area3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/ray_cast3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/light3d.hpp>
#include <godot_cpp/classes/directional_light3d.hpp>
#include <godot_cpp/classes/omni_light3d.hpp>
#include <godot_cpp/classes/spot_light3d.hpp>

// Audio Classes
#include <godot_cpp/classes/audio_stream_player.hpp>
#include <godot_cpp/classes/audio_stream_player2d.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>

// Animation Classes
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/animation_tree.hpp>
#include <godot_cpp/classes/tween.hpp>

// Timer
#include <godot_cpp/classes/timer.hpp>

// Containers
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#include <godot_cpp/classes/grid_container.hpp>
#include <godot_cpp/classes/center_container.hpp>
#include <godot_cpp/classes/margin_container.hpp>

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/core/class_db.hpp>

HashMap<String, GodotClassBindings::ClassInfo> GodotClassBindings::class_registry;

void GodotClassBindings::setup_class_bindings(lua_State* L) {
    // Register all major Godot class categories
    register_singletons(L);
    register_node_classes(L);
    register_resource_classes(L);
    register_input_classes(L);
    register_physics_classes(L);
    register_audio_classes(L);
    register_rendering_classes(L);
    register_animation_classes(L);
    register_gui_classes(L);
    register_networking_classes(L);
    register_io_classes(L);
    register_math_classes(L);
    
    // Setup automatic binding system
    setup_auto_bindings(L);
}

void GodotClassBindings::register_singletons(lua_State* L) {
    // Core singletons available globally
    REGISTER_GODOT_SINGLETON(L, Engine, Engine);
    REGISTER_GODOT_SINGLETON(L, Input, Input);
    REGISTER_GODOT_SINGLETON(L, OS, OS);
    REGISTER_GODOT_SINGLETON(L, Time, Time);
    REGISTER_GODOT_SINGLETON(L, ProjectSettings, ProjectSettings);
    REGISTER_GODOT_SINGLETON(L, ResourceLoader, ResourceLoader);
    REGISTER_GODOT_SINGLETON(L, ResourceSaver, ResourceSaver);
    REGISTER_GODOT_SINGLETON(L, DisplayServer, DisplayServer);
    REGISTER_GODOT_SINGLETON(L, AudioServer, AudioServer);
    REGISTER_GODOT_SINGLETON(L, RenderingServer, RenderingServer);
    REGISTER_GODOT_SINGLETON(L, PhysicsServer2D, PhysicsServer2D);
    REGISTER_GODOT_SINGLETON(L, PhysicsServer3D, PhysicsServer3D);
    REGISTER_GODOT_SINGLETON(L, NavigationServer2D, NavigationServer2D);
    REGISTER_GODOT_SINGLETON(L, NavigationServer3D, NavigationServer3D);
    REGISTER_GODOT_SINGLETON(L, XRServer, XRServer);
    REGISTER_GODOT_SINGLETON(L, CameraServer, CameraServer);
}

void GodotClassBindings::register_node_classes(lua_State* L) {
    // Base Node classes
    REGISTER_GODOT_CLASS(L, Node);
    REGISTER_GODOT_CLASS(L, Node2D);
    REGISTER_GODOT_CLASS(L, Node3D);
    REGISTER_GODOT_CLASS(L, CanvasItem);
    REGISTER_GODOT_CLASS(L, Control);
    
    // 2D Nodes
    REGISTER_GODOT_CLASS(L, Sprite2D);
    REGISTER_GODOT_CLASS(L, AnimatedSprite2D);
    REGISTER_GODOT_CLASS(L, CharacterBody2D);
    REGISTER_GODOT_CLASS(L, RigidBody2D);
    REGISTER_GODOT_CLASS(L, StaticBody2D);
    REGISTER_GODOT_CLASS(L, Area2D);
    REGISTER_GODOT_CLASS(L, CollisionShape2D);
    REGISTER_GODOT_CLASS(L, CollisionPolygon2D);
    REGISTER_GODOT_CLASS(L, RayCast2D);
    REGISTER_GODOT_CLASS(L, Camera2D);
    REGISTER_GODOT_CLASS(L, ParallaxBackground);
    REGISTER_GODOT_CLASS(L, ParallaxLayer);
    REGISTER_GODOT_CLASS(L, TileMap);
    REGISTER_GODOT_CLASS(L, YSort);
    
    // 3D Nodes
    REGISTER_GODOT_CLASS(L, MeshInstance3D);
    REGISTER_GODOT_CLASS(L, CharacterBody3D);
    REGISTER_GODOT_CLASS(L, RigidBody3D);
    REGISTER_GODOT_CLASS(L, StaticBody3D);
    REGISTER_GODOT_CLASS(L, Area3D);
    REGISTER_GODOT_CLASS(L, CollisionShape3D);
    REGISTER_GODOT_CLASS(L, RayCast3D);
    REGISTER_GODOT_CLASS(L, Camera3D);
    REGISTER_GODOT_CLASS(L, Light3D);
    REGISTER_GODOT_CLASS(L, DirectionalLight3D);
    REGISTER_GODOT_CLASS(L, OmniLight3D);
    REGISTER_GODOT_CLASS(L, SpotLight3D);
    
    // Utility Nodes
    REGISTER_GODOT_CLASS(L, Timer);
    REGISTER_GODOT_CLASS(L, AnimationPlayer);
    REGISTER_GODOT_CLASS(L, AnimationTree);
    REGISTER_GODOT_CLASS(L, Tween);
}

void GodotClassBindings::register_gui_classes(lua_State* L) {
    // Basic UI Controls
    REGISTER_GODOT_CLASS(L, Label);
    REGISTER_GODOT_CLASS(L, Button);
    REGISTER_GODOT_CLASS(L, LineEdit);
    REGISTER_GODOT_CLASS(L, TextEdit);
    REGISTER_GODOT_CLASS(L, RichTextLabel);
    
    // Interactive Controls
    REGISTER_GODOT_CLASS(L, ProgressBar);
    REGISTER_GODOT_CLASS(L, Slider);
    REGISTER_GODOT_CLASS(L, SpinBox);
    REGISTER_GODOT_CLASS(L, CheckBox);
    REGISTER_GODOT_CLASS(L, OptionButton);
    REGISTER_GODOT_CLASS(L, MenuButton);
    REGISTER_GODOT_CLASS(L, PopupMenu);
    REGISTER_GODOT_CLASS(L, Tree);
    REGISTER_GODOT_CLASS(L, ItemList);
    REGISTER_GODOT_CLASS(L, TabContainer);
    
    // Containers
    REGISTER_GODOT_CLASS(L, Panel);
    REGISTER_GODOT_CLASS(L, PanelContainer);
    REGISTER_GODOT_CLASS(L, VBoxContainer);
    REGISTER_GODOT_CLASS(L, HBoxContainer);
    REGISTER_GODOT_CLASS(L, GridContainer);
    REGISTER_GODOT_CLASS(L, CenterContainer);
    REGISTER_GODOT_CLASS(L, MarginContainer);
    REGISTER_GODOT_CLASS(L, SplitContainer);
    REGISTER_GODOT_CLASS(L, ScrollContainer);
}

void GodotClassBindings::register_audio_classes(lua_State* L) {
    REGISTER_GODOT_CLASS(L, AudioStreamPlayer);
    REGISTER_GODOT_CLASS(L, AudioStreamPlayer2D);
    REGISTER_GODOT_CLASS(L, AudioStreamPlayer3D);
}

void GodotClassBindings::register_resource_classes(lua_State* L) {
    REGISTER_GODOT_CLASS(L, Resource);
}

void GodotClassBindings::register_input_classes(lua_State* L) {
    // Input is a singleton, already registered
}

void GodotClassBindings::register_physics_classes(lua_State* L) {
    // Physics classes are mostly singletons or already covered in nodes
}

void GodotClassBindings::register_rendering_classes(lua_State* L) {
    // Rendering classes are mostly singletons
}

void GodotClassBindings::register_animation_classes(lua_State* L) {
    // Already covered in register_node_classes
}

void GodotClassBindings::register_networking_classes(lua_State* L) {
    // TODO: Add networking classes
}

void GodotClassBindings::register_io_classes(lua_State* L) {
    // File I/O functions
    lua_pushcfunction(L, [](lua_State* L) -> int {
        const char* path = luaL_checkstring(L, 1);
        int mode = luaL_optinteger(L, 2, FileAccess::READ);
        
        Ref<FileAccess> file = FileAccess::open(String(path), (FileAccess::ModeFlags)mode);
        if (file.is_null()) {
            lua_pushnil(L);
            lua_pushstring(L, "Failed to open file");
            return 2;
        }
        
        push_godot_object(L, file.ptr(), "FileAccess");
        return 1;
    }, "file_open");
    lua_setglobal(L, "file_open");
    
    lua_pushcfunction(L, [](lua_State* L) -> int {
        const char* path = luaL_checkstring(L, 1);
        
        Ref<DirAccess> dir = DirAccess::open(String(path));
        if (dir.is_null()) {
            lua_pushnil(L);
            lua_pushstring(L, "Failed to open directory");
            return 2;
        }
        
        push_godot_object(L, dir.ptr(), "DirAccess");
        return 1;
    }, "dir_open");
    lua_setglobal(L, "dir_open");
}

void GodotClassBindings::register_math_classes(lua_State* L) {
    // Math utilities are already covered in godot_api_bindings
}

void GodotClassBindings::setup_auto_bindings(lua_State* L) {
    // Create global function for dynamic method calling
    lua_pushcfunction(L, lua_object_method_call, "call_method");
    lua_setglobal(L, "call_method");
    
    lua_pushcfunction(L, lua_object_property_get, "get_property");
    lua_setglobal(L, "get_property");
    
    lua_pushcfunction(L, lua_object_property_set, "set_property");
    lua_setglobal(L, "set_property");
    
    lua_pushcfunction(L, lua_object_signal_connect, "connect_signal");
    lua_setglobal(L, "connect_signal");
    
    lua_pushcfunction(L, lua_object_signal_emit, "emit_signal");
    lua_setglobal(L, "emit_signal");
}

int GodotClassBindings::lua_class_new(lua_State* L) {
    const char* class_name = lua_tostring(L, lua_upvalueindex(1));
    if (!class_name) {
        // Get class name from metatable
        if (lua_getmetatable(L, 1)) {
            lua_getfield(L, -1, "__class_name");
            class_name = lua_tostring(L, -1);
            lua_pop(L, 2);
        }
    }
    
    if (!class_name) {
        luaL_error(L, "Cannot determine class name for instantiation");
        return 0;
    }
    
    // Create new Godot object
    Object* obj = ClassDB::instantiate(StringName(class_name));
    if (!obj) {
        luaL_error(L, "Failed to instantiate class: %s", class_name);
        return 0;
    }
    
    push_godot_object(L, obj, String(class_name));
    return 1;
}

int GodotClassBindings::lua_object_method_call(lua_State* L) {
    Object* obj = get_godot_object(L, 1);
    const char* method_name = luaL_checkstring(L, 2);
    
    if (!obj) {
        luaL_error(L, "Invalid object for method call");
        return 0;
    }
    
    // Collect arguments
    int arg_count = lua_gettop(L) - 2;
    Array args;
    for (int i = 3; i <= lua_gettop(L); i++) {
        args.append(GodotApiBindings::lua_to_variant(L, i));
    }
    
    // Call method
    Variant result = call_godot_method(obj, String(method_name), args);
    GodotApiBindings::variant_to_lua(L, result);
    
    return 1;
}

int GodotClassBindings::lua_object_property_get(lua_State* L) {
    Object* obj = get_godot_object(L, 1);
    const char* property_name = luaL_checkstring(L, 2);
    
    if (!obj) {
        luaL_error(L, "Invalid object for property access");
        return 0;
    }
    
    Variant result = get_godot_property(obj, String(property_name));
    GodotApiBindings::variant_to_lua(L, result);
    
    return 1;
}

int GodotClassBindings::lua_object_property_set(lua_State* L) {
    Object* obj = get_godot_object(L, 1);
    const char* property_name = luaL_checkstring(L, 2);
    Variant value = GodotApiBindings::lua_to_variant(L, 3);
    
    if (!obj) {
        luaL_error(L, "Invalid object for property assignment");
        return 0;
    }
    
    bool success = set_godot_property(obj, String(property_name), value);
    lua_pushboolean(L, success);
    
    return 1;
}

int GodotClassBindings::lua_object_signal_connect(lua_State* L) {
    Object* obj = get_godot_object(L, 1);
    const char* signal_name = luaL_checkstring(L, 2);
    Object* target = get_godot_object(L, 3);
    const char* method_name = luaL_checkstring(L, 4);
    
    if (!obj || !target) {
        luaL_error(L, "Invalid objects for signal connection");
        return 0;
    }
    
    Error err = obj->connect(StringName(signal_name), Callable(target, StringName(method_name)));
    lua_pushboolean(L, err == OK);
    
    return 1;
}

int GodotClassBindings::lua_object_signal_emit(lua_State* L) {
    Object* obj = get_godot_object(L, 1);
    const char* signal_name = luaL_checkstring(L, 2);
    
    if (!obj) {
        luaL_error(L, "Invalid object for signal emission");
        return 0;
    }
    
    // Collect arguments
    int arg_count = lua_gettop(L) - 2;
    Array args;
    for (int i = 3; i <= lua_gettop(L); i++) {
        args.append(GodotApiBindings::lua_to_variant(L, i));
    }
    
    // Emit signal with arguments
    obj->emit_signal(StringName(signal_name), args);
    
    return 0;
}

void GodotClassBindings::push_godot_object(lua_State* L, Object* obj, const String& class_name) {
    if (!obj) {
        lua_pushnil(L);
        return;
    }
    
    // Create userdata to hold object pointer
    Object** obj_ptr = (Object**)lua_newuserdata(L, sizeof(Object*));
    *obj_ptr = obj;
    
    // Get or create metatable for class
    String actual_class = class_name.is_empty() ? obj->get_class() : class_name;
    luaL_getmetatable(L, actual_class.utf8().get_data());
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        create_class_metatable(L, actual_class.utf8().get_data());
    }
    lua_setmetatable(L, -2);
}

Object* GodotClassBindings::get_godot_object(lua_State* L, int index) {
    if (!lua_isuserdata(L, index)) {
        return nullptr;
    }
    
    Object** obj_ptr = (Object**)lua_touserdata(L, index);
    return (obj_ptr && *obj_ptr) ? *obj_ptr : nullptr;
}

void GodotClassBindings::create_class_metatable(lua_State* L, const char* class_name, const char* parent_class) {
    luaL_newmetatable(L, class_name);
    
    // Store class name
    lua_pushstring(L, class_name);
    lua_setfield(L, -2, "__class_name");
    
    // Method call metamethod
    lua_pushcfunction(L, [](lua_State* L) -> int {
        Object* obj = get_godot_object(L, 1);
        const char* key = lua_tostring(L, 2);
        
        if (!obj || !key) {
            lua_pushnil(L);
            return 1;
        }
        
        // Check if it's a method
        if (obj->has_method(StringName(key))) {
            // Return a bound method
            lua_pushstring(L, key);
            lua_pushcclosure(L, [](lua_State* L) -> int {
                Object* obj = get_godot_object(L, 1);
                const char* method = lua_tostring(L, lua_upvalueindex(1));
                
                // Collect arguments (skip self)
                int arg_count = lua_gettop(L) - 1;
                Array args;
                for (int i = 2; i <= lua_gettop(L); i++) {
                    args.append(GodotApiBindings::lua_to_variant(L, i));
                }
                
                Variant result = call_godot_method(obj, String(method), args);
                GodotApiBindings::variant_to_lua(L, result);
                return 1;
            }, "bound_method", 1);
            return 1;
        }
        
        // Check if it's a property
        Variant prop_value = get_godot_property(obj, String(key));
        if (prop_value.get_type() != Variant::NIL) {
            GodotApiBindings::variant_to_lua(L, prop_value);
            return 1;
        }
        
        lua_pushnil(L);
        return 1;
    }, "__index");
    lua_setfield(L, -2, "__index");
    
    // Property assignment metamethod
    lua_pushcfunction(L, [](lua_State* L) -> int {
        Object* obj = get_godot_object(L, 1);
        const char* key = lua_tostring(L, 2);
        Variant value = GodotApiBindings::lua_to_variant(L, 3);
        
        if (obj && key) {
            set_godot_property(obj, String(key), value);
        }
        
        return 0;
    }, "__newindex");
    lua_setfield(L, -2, "__newindex");
    
    // Garbage collection
    lua_pushcfunction(L, [](lua_State* L) -> int {
        // Note: We don't delete Godot objects here as they have their own lifecycle
        return 0;
    }, "__gc");
    lua_setfield(L, -2, "__gc");
    
    // Set inheritance if parent class is specified
    if (parent_class) {
        luaL_getmetatable(L, parent_class);
        if (!lua_isnil(L, -1)) {
            lua_setfield(L, -2, "__parent");
        } else {
            lua_pop(L, 1);
        }
    }
}

Variant GodotClassBindings::call_godot_method(Object* obj, const String& method, const Array& args) {
    if (!obj || !obj->has_method(StringName(method))) {
        return Variant();
    }
    
    // Convert Array to Variant arguments
    Vector<Variant> variant_args;
    for (int i = 0; i < args.size(); i++) {
        variant_args.push_back(args[i]);
    }
    
    // Call method
    const Variant** arg_ptrs = nullptr;
    if (variant_args.size() > 0) {
        arg_ptrs = (const Variant**)alloca(variant_args.size() * sizeof(Variant*));
        for (int i = 0; i < variant_args.size(); i++) {
            arg_ptrs[i] = &variant_args[i];
        }
    }
    
    Variant result = obj->callv(StringName(method), args);
    
    return result;
}

bool GodotClassBindings::set_godot_property(Object* obj, const String& property, const Variant& value) {
    if (!obj) {
        return false;
    }
    
    bool valid = false;
    obj->set(StringName(property), value);
    return true;
}

Variant GodotClassBindings::get_godot_property(Object* obj, const String& property) {
    if (!obj) {
        return Variant();
    }
    
    bool valid = false;
    Variant result = obj->get(StringName(property));
    return result;
}

