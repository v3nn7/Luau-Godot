#include "godot_api_bindings.h"
#include "godot_class_bindings.h"

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <cmath>
#include <godot_cpp/variant/utility_functions.hpp>

void GodotApiBindings::setup_bindings(lua_State* L) {
    // Setup complete Godot class bindings first
    GodotClassBindings::setup_class_bindings(L);
    
    // Setup global Godot functions
    lua_pushcfunction(L, lua_print, "print");
    lua_setglobal(L, "print");
    
    lua_pushcfunction(L, lua_get_node, "get_node");
    lua_setglobal(L, "get_node");
    
    lua_pushcfunction(L, lua_create_node, "create_node");
    lua_setglobal(L, "create_node");
    
    lua_pushcfunction(L, lua_connect_signal, "connect_signal");
    lua_setglobal(L, "connect_signal");
    
    lua_pushcfunction(L, lua_emit_signal, "emit_signal");
    lua_setglobal(L, "emit_signal");
    
    // Setup vector and math types
    setup_vector_types(L);
    
    // Setup node bindings
    setup_node_bindings(L);
    
    // Setup input bindings
    setup_input_bindings(L);
    
    // Setup resource bindings
    setup_resource_bindings(L);
    
    // Create Godot global table with constants
    lua_newtable(L);
    
    // Add common Godot constants
    lua_pushinteger(L, (int)Node::PROCESS_MODE_INHERIT);
    lua_setfield(L, -2, "PROCESS_MODE_INHERIT");
    
    lua_pushinteger(L, (int)Node::PROCESS_MODE_PAUSABLE);
    lua_setfield(L, -2, "PROCESS_MODE_PAUSABLE");
    
    lua_pushinteger(L, (int)Node::PROCESS_MODE_WHEN_PAUSED);
    lua_setfield(L, -2, "PROCESS_MODE_WHEN_PAUSED");
    
    lua_pushinteger(L, (int)Node::PROCESS_MODE_DISABLED);
    lua_setfield(L, -2, "PROCESS_MODE_DISABLED");
    
    // Add more global constants from various classes
    setup_global_constants(L);
    
    lua_setglobal(L, "Godot");
}

void GodotApiBindings::variant_to_lua(lua_State* L, const Variant& value) {
    switch (value.get_type()) {
        case Variant::NIL:
            lua_pushnil(L);
            break;
        case Variant::BOOL:
            lua_pushboolean(L, (bool)value);
            break;
        case Variant::INT:
            lua_pushinteger(L, (int64_t)value);
            break;
        case Variant::FLOAT:
            lua_pushnumber(L, (double)value);
            break;
        case Variant::STRING:
            {
                String str = value;
                lua_pushstring(L, str.utf8().get_data());
            }
            break;
        case Variant::VECTOR2:
            {
                Vector2 vec = value;
                lua_newtable(L);
                lua_pushnumber(L, vec.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, vec.y);
                lua_setfield(L, -2, "y");
                
                // Add metatable for vector operations
                luaL_getmetatable(L, "Vector2");
                if (lua_isnil(L, -1)) {
                    lua_pop(L, 1);
                    luaL_newmetatable(L, "Vector2");
                    lua_pushcfunction(L, lua_vector2_add, "__add");
                    lua_setfield(L, -2, "__add");
                }
                lua_setmetatable(L, -2);
            }
            break;
        case Variant::VECTOR3:
            {
                Vector3 vec = value;
                lua_newtable(L);
                lua_pushnumber(L, vec.x);
                lua_setfield(L, -2, "x");
                lua_pushnumber(L, vec.y);
                lua_setfield(L, -2, "y");
                lua_pushnumber(L, vec.z);
                lua_setfield(L, -2, "z");
                
                luaL_getmetatable(L, "Vector3");
                if (lua_isnil(L, -1)) {
                    lua_pop(L, 1);
                    luaL_newmetatable(L, "Vector3");
                    lua_pushcfunction(L, lua_vector3_add, "__add");
                    lua_setfield(L, -2, "__add");
                }
                lua_setmetatable(L, -2);
            }
            break;
        case Variant::OBJECT:
            {
                Object* obj = value;
                push_object(L, obj);
            }
            break;
        case Variant::ARRAY:
            {
                Array arr = value;
                lua_newtable(L);
                for (int i = 0; i < arr.size(); i++) {
                    variant_to_lua(L, arr[i]);
                    lua_rawseti(L, -2, i + 1); // Lua arrays are 1-indexed
                }
            }
            break;
        case Variant::DICTIONARY:
            {
                Dictionary dict = value;
                lua_newtable(L);
                Array keys = dict.keys();
                for (int i = 0; i < keys.size(); i++) {
                    variant_to_lua(L, dict[keys[i]]);
                    variant_to_lua(L, keys[i]);
                    lua_settable(L, -3);
                }
            }
            break;
        default:
            push_variant_as_table(L, value);
            break;
    }
}

Variant GodotApiBindings::lua_to_variant(lua_State* L, int index) {
    int type = lua_type(L, index);
    auto is_integer_compat = [&](int idx) -> bool {
        if (!lua_isnumber(L, idx)) return false;
        double n = lua_tonumber(L, idx);
        return std::floor(n) == n;
    };
    
    switch (type) {
        case LUA_TNIL:
            return Variant();
        case LUA_TBOOLEAN:
            return Variant(lua_toboolean(L, index) != 0);
        case LUA_TNUMBER:
            if (is_integer_compat(index)) {
                return Variant((int64_t)lua_tointeger(L, index));
            } else {
                return Variant(lua_tonumber(L, index));
            }
        case LUA_TSTRING:
            return Variant(String(lua_tostring(L, index)));
        case LUA_TTABLE:
            {
                // Check if it's a Vector2/Vector3/other special type
                lua_getfield(L, index, "x");
                if (!lua_isnil(L, -1)) {
                    lua_getfield(L, index, "y");
                    if (!lua_isnil(L, -1)) {
                        lua_getfield(L, index, "z");
                        if (!lua_isnil(L, -1)) {
                            // Vector3
                            double x = lua_tonumber(L, -3);
                            double y = lua_tonumber(L, -2);
                            double z = lua_tonumber(L, -1);
                            lua_pop(L, 3);
                            return Variant(Vector3(x, y, z));
                        } else {
                            // Vector2
                            lua_pop(L, 1);
                            double x = lua_tonumber(L, -2);
                            double y = lua_tonumber(L, -1);
                            lua_pop(L, 2);
                            return Variant(Vector2(x, y));
                        }
                    }
                }
                lua_pop(L, 1);
                
                // Check if it's an array (all integer keys)
                bool is_array = true;
                lua_pushnil(L);
                while (lua_next(L, index) != 0) {
                    if (!is_integer_compat(-2)) {
                        is_array = false;
                        lua_pop(L, 2);
                        break;
                    }
                    lua_pop(L, 1);
                }
                
                if (is_array) {
                    Array arr;
                    lua_pushnil(L);
                    while (lua_next(L, index) != 0) {
                        arr.append(lua_to_variant(L, -1));
                        lua_pop(L, 1);
                    }
                    return Variant(arr);
                } else {
                    Dictionary dict;
                    lua_pushnil(L);
                    while (lua_next(L, index) != 0) {
                        Variant key = lua_to_variant(L, -2);
                        Variant value = lua_to_variant(L, -1);
                        dict[key] = value;
                        lua_pop(L, 1);
                    }
                    return Variant(dict);
                }
            }
        case LUA_TUSERDATA:
            {
                // Check if it's a wrapped Godot object by verifying metatable
                if (lua_getmetatable(L, index)) {
                    // Check if this userdata has a Godot object metatable
                    // We can verify this by checking if it has the expected size
                    size_t size = lua_objlen(L, index);
                    if (size == sizeof(Object*)) {
                        Object** obj_ptr = (Object**)lua_touserdata(L, index);
                        if (obj_ptr && *obj_ptr) {
                            lua_pop(L, 1); // Remove metatable
                            return Variant(*obj_ptr);
                        }
                    }
                    lua_pop(L, 1); // Remove metatable
                }
            }
            break;
    }
    
    return Variant();
}

void GodotApiBindings::push_object(lua_State* L, Object* obj) {
    if (!obj) {
        lua_pushnil(L);
        return;
    }
    
    // Create userdata to hold the object pointer
    Object** obj_ptr = (Object**)lua_newuserdata(L, sizeof(Object*));
    *obj_ptr = obj;
    
    // Set metatable for object methods
    String class_name = obj->get_class();
    luaL_getmetatable(L, class_name.utf8().get_data());
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        luaL_newmetatable(L, class_name.utf8().get_data());
        register_class_methods(L, class_name.utf8().get_data());
    }
    lua_setmetatable(L, -2);
}

Object* GodotApiBindings::check_object(lua_State* L, int index) {
    if (!lua_isuserdata(L, index)) {
        return nullptr;
    }
    
    // Verify it's a Godot object by checking metatable and size
    if (lua_getmetatable(L, index)) {
        size_t size = lua_objlen(L, index);
        if (size == sizeof(Object*)) {
            Object** obj_ptr = (Object**)lua_touserdata(L, index);
            lua_pop(L, 1); // Remove metatable
            return (obj_ptr && *obj_ptr) ? *obj_ptr : nullptr;
        }
        lua_pop(L, 1); // Remove metatable
    }
    
    return nullptr;
}

void GodotApiBindings::setup_vector_types(lua_State* L) {
    // Vector2 constructor
    lua_pushcfunction(L, lua_vector2_new, "Vector2");
    lua_setglobal(L, "Vector2");
    
    // Vector3 constructor  
    lua_pushcfunction(L, lua_vector3_new, "Vector3");
    lua_setglobal(L, "Vector3");
    
    // Transform2D constructor
    lua_pushcfunction(L, lua_transform2d_new, "Transform2D");
    lua_setglobal(L, "Transform2D");
    
    // Transform3D constructor
    lua_pushcfunction(L, lua_transform3d_new, "Transform3D");
    lua_setglobal(L, "Transform3D");
}

void GodotApiBindings::setup_node_bindings(lua_State* L) {
    lua_pushcfunction(L, lua_get_tree, "get_tree");
    lua_setglobal(L, "get_tree");
    
    lua_pushcfunction(L, lua_change_scene, "change_scene");
    lua_setglobal(L, "change_scene");
    
    lua_pushcfunction(L, lua_create_timer, "create_timer");
    lua_setglobal(L, "create_timer");
}

void GodotApiBindings::setup_input_bindings(lua_State* L) {
    lua_pushcfunction(L, lua_input_is_action_pressed, "is_action_pressed");
    lua_setglobal(L, "is_action_pressed");
    
    lua_pushcfunction(L, lua_input_get_action_strength, "get_action_strength");
    lua_setglobal(L, "get_action_strength");
}

void GodotApiBindings::setup_resource_bindings(lua_State* L) {
    lua_pushcfunction(L, lua_load_resource, "load_resource");
    lua_setglobal(L, "load_resource");
    
    lua_pushcfunction(L, lua_save_resource, "save_resource");
    lua_setglobal(L, "save_resource");
}

// Lua C function implementations

int GodotApiBindings::lua_print(lua_State* L) {
    int n = lua_gettop(L);
    String message;
    
    for (int i = 1; i <= n; i++) {
        if (i > 1) message += "\t";
        
        if (lua_isstring(L, i)) {
            message += lua_tostring(L, i);
        } else {
            // Convert to string representation
            lua_getglobal(L, "tostring");
            lua_pushvalue(L, i);
            lua_call(L, 1, 1);
            message += lua_tostring(L, -1);
            lua_pop(L, 1);
        }
    }
    
    UtilityFunctions::print(message);
    return 0;
}

int GodotApiBindings::lua_get_node(lua_State* L) {
    if (lua_gettop(L) < 2) {
        lua_pushnil(L);
        return 1;
    }
    
    Object* base_obj = check_object(L, 1);
    const char* path = lua_tostring(L, 2);
    
    Node* base_node = Object::cast_to<Node>(base_obj);
    if (!base_node || !path) {
        lua_pushnil(L);
        return 1;
    }
    
    Node* found_node = base_node->get_node_or_null(NodePath(path));
    push_object(L, found_node);
    
    return 1;
}

int GodotApiBindings::lua_vector2_new(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0.0);
    double y = luaL_optnumber(L, 2, 0.0);
    
    variant_to_lua(L, Variant(Vector2(x, y)));
    return 1;
}

int GodotApiBindings::lua_vector2_add(lua_State* L) {
    // Get x,y from first vector
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    double x1 = lua_tonumber(L, -2);
    double y1 = lua_tonumber(L, -1);
    lua_pop(L, 2);
    
    // Get x,y from second vector
    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    double x2 = lua_tonumber(L, -2);
    double y2 = lua_tonumber(L, -1);
    lua_pop(L, 2);
    
    variant_to_lua(L, Variant(Vector2(x1 + x2, y1 + y2)));
    return 1;
}

int GodotApiBindings::lua_vector3_new(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0.0);
    double y = luaL_optnumber(L, 2, 0.0);
    double z = luaL_optnumber(L, 3, 0.0);
    
    variant_to_lua(L, Variant(Vector3(x, y, z)));
    return 1;
}

int GodotApiBindings::lua_vector3_add(lua_State* L) {
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    lua_getfield(L, 1, "z");
    double x1 = lua_tonumber(L, -3);
    double y1 = lua_tonumber(L, -2);
    double z1 = lua_tonumber(L, -1);
    lua_pop(L, 3);
    
    lua_getfield(L, 2, "x");
    lua_getfield(L, 2, "y");
    lua_getfield(L, 2, "z");
    double x2 = lua_tonumber(L, -3);
    double y2 = lua_tonumber(L, -2);
    double z2 = lua_tonumber(L, -1);
    lua_pop(L, 3);
    
    variant_to_lua(L, Variant(Vector3(x1 + x2, y1 + y2, z1 + z2)));
    return 1;
}

int GodotApiBindings::lua_input_is_action_pressed(lua_State* L) {
    const char* action = lua_tostring(L, 1);
    if (!action) {
        lua_pushboolean(L, false);
        return 1;
    }
    
    Input* input = Input::get_singleton();
    bool pressed = input && input->is_action_pressed(StringName(action));
    lua_pushboolean(L, pressed);
    
    return 1;
}

int GodotApiBindings::lua_create_node(lua_State* L) {
    const char* class_name = lua_tostring(L, 1);
    if (!class_name) {
        lua_pushnil(L);
        return 1;
    }
    
    Object* obj = ClassDB::instantiate(StringName(class_name));
    push_object(L, obj);
    
    return 1;
}

// Stub implementations for remaining functions
int GodotApiBindings::lua_connect_signal(lua_State* L) { return 0; }
int GodotApiBindings::lua_emit_signal(lua_State* L) { return 0; }
int GodotApiBindings::lua_vector2_length(lua_State* L) {
    // expects a table with x, y
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    double x = lua_tonumber(L, -2);
    double y = lua_tonumber(L, -1);
    lua_pop(L, 2);
    double len = std::sqrt(x * x + y * y);
    lua_pushnumber(L, len);
    return 1;
}
int GodotApiBindings::lua_vector3_length(lua_State* L) {
    // expects a table with x, y, z
    lua_getfield(L, 1, "x");
    lua_getfield(L, 1, "y");
    lua_getfield(L, 1, "z");
    double x = lua_tonumber(L, -3);
    double y = lua_tonumber(L, -2);
    double z = lua_tonumber(L, -1);
    lua_pop(L, 3);
    double len = std::sqrt(x * x + y * y + z * z);
    lua_pushnumber(L, len);
    return 1;
}
int GodotApiBindings::lua_transform2d_new(lua_State* L) { return 0; }
int GodotApiBindings::lua_transform3d_new(lua_State* L) { return 0; }
int GodotApiBindings::lua_input_get_action_strength(lua_State* L) {
    const char* action = lua_tostring(L, 1);
    if (!action) {
        lua_pushnumber(L, 0.0);
        return 1;
    }
    Input* input = Input::get_singleton();
    float strength = input ? input->get_action_strength(StringName(action)) : 0.0f;
    lua_pushnumber(L, strength);
    return 1;
}
int GodotApiBindings::lua_get_tree(lua_State* L) {
    SceneTree* tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    push_object(L, tree);
    return 1;
}
int GodotApiBindings::lua_change_scene(lua_State* L) {
    SceneTree* tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    const char* path = lua_tostring(L, 1);
    if (!tree || !path) { lua_pushboolean(L, false); return 1; }
    Error err = tree->change_scene_to_file(String(path));
    lua_pushboolean(L, err == OK);
    return 1;
}
int GodotApiBindings::lua_create_timer(lua_State* L) {
    double seconds = lua_gettop(L) >= 1 ? lua_tonumber(L, 1) : 0.0;
    Timer* timer = memnew(Timer);
    timer->set_wait_time(seconds);
    timer->set_one_shot(false);
    push_object(L, timer);
    return 1;
}
int GodotApiBindings::lua_load_resource(lua_State* L) {
    const char* path = lua_tostring(L, 1);
    if (!path) { lua_pushnil(L); return 1; }
    Ref<Resource> res = ResourceLoader::get_singleton()->load(String(path));
    if (res.is_null()) { lua_pushnil(L); return 1; }
    GodotApiBindings::push_object(L, static_cast<Object*>(res.ptr()));
    return 1;
}
int GodotApiBindings::lua_save_resource(lua_State* L) {
    Object* obj = check_object(L, 1);
    const char* path = lua_tostring(L, 2);
    Resource* res = Object::cast_to<Resource>(obj);
    if (!res || !path) { lua_pushboolean(L, false); return 1; }
    Error err = ResourceSaver::get_singleton()->save(Ref<Resource>(res), String(path));
    lua_pushboolean(L, err == OK);
    return 1;
}

void GodotApiBindings::register_class_methods(lua_State* L, const char* class_name) {
    // Register common methods available on all objects
    // This is a simplified implementation
}

void GodotApiBindings::setup_global_constants(lua_State* L) {
    // Input constants
    lua_pushinteger(L, (int)Input::MOUSE_MODE_VISIBLE);
    lua_setfield(L, -2, "MOUSE_MODE_VISIBLE");
    
    lua_pushinteger(L, (int)Input::MOUSE_MODE_HIDDEN);
    lua_setfield(L, -2, "MOUSE_MODE_HIDDEN");
    
    lua_pushinteger(L, (int)Input::MOUSE_MODE_CAPTURED);
    lua_setfield(L, -2, "MOUSE_MODE_CAPTURED");
    
    lua_pushinteger(L, (int)Input::MOUSE_MODE_CONFINED);
    lua_setfield(L, -2, "MOUSE_MODE_CONFINED");
    
    // FileAccess constants
    lua_pushinteger(L, (int)FileAccess::ModeFlags::READ);
    lua_setfield(L, -2, "FILE_READ");
    
    lua_pushinteger(L, (int)FileAccess::ModeFlags::WRITE);
    lua_setfield(L, -2, "FILE_WRITE");
    
    lua_pushinteger(L, (int)FileAccess::ModeFlags::READ_WRITE);
    lua_setfield(L, -2, "FILE_READ_WRITE");
    
    // Error constants
    lua_pushinteger(L, (int)OK);
    lua_setfield(L, -2, "OK");
    
    lua_pushinteger(L, (int)ERR_FILE_NOT_FOUND);
    lua_setfield(L, -2, "ERR_FILE_NOT_FOUND");
    
    lua_pushinteger(L, (int)ERR_FILE_CANT_OPEN);
    lua_setfield(L, -2, "ERR_FILE_CANT_OPEN");
    
    // Common math constants
    lua_pushnumber(L, Math_PI);
    lua_setfield(L, -2, "PI");
    
    lua_pushnumber(L, Math_TAU);
    lua_setfield(L, -2, "TAU");
    
    lua_pushnumber(L, Math_E);
    lua_setfield(L, -2, "E");
    
    lua_pushnumber(L, Math_SQRT2);
    lua_setfield(L, -2, "SQRT2");
}

void GodotApiBindings::push_variant_as_table(lua_State* L, const Variant& value) {
    lua_newtable(L);
    lua_pushstring(L, Variant::get_type_name(value.get_type()).utf8().get_data());
    lua_setfield(L, -2, "_type");
}
