#ifndef GODOT_API_BINDINGS_H
#define GODOT_API_BINDINGS_H

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/object.hpp>

#include <lua.h>
#include <lualib.h>

using namespace godot;

class GodotApiBindings {
public:
    // Setup all Godot API bindings in Luau
    static void setup_bindings(lua_State* L);
    
    // Conversion functions between Godot Variant and Lua values
    static void variant_to_lua(lua_State* L, const Variant& value);
    static Variant lua_to_variant(lua_State* L, int index);
    
    // Object handling
    static void push_object(lua_State* L, Object* obj);
    static Object* check_object(lua_State* L, int index);
    
    // Vector and math types
    static void setup_vector_types(lua_State* L);
    
    // Node and scene management
    static void setup_node_bindings(lua_State* L);
    
    // Input system
    static void setup_input_bindings(lua_State* L);
    
    // Resource system
    static void setup_resource_bindings(lua_State* L);

private:
    // Lua C function wrappers
    static int lua_print(lua_State* L);
    static int lua_get_node(lua_State* L);
    static int lua_create_node(lua_State* L);
    static int lua_connect_signal(lua_State* L);
    static int lua_emit_signal(lua_State* L);
    
    // Vector2 operations
    static int lua_vector2_new(lua_State* L);
    static int lua_vector2_add(lua_State* L);
    static int lua_vector2_length(lua_State* L);
    
    // Vector3 operations
    static int lua_vector3_new(lua_State* L);
    static int lua_vector3_add(lua_State* L);
    static int lua_vector3_length(lua_State* L);
    
    // Transform operations
    static int lua_transform2d_new(lua_State* L);
    static int lua_transform3d_new(lua_State* L);
    
    // Input functions
    static int lua_input_is_action_pressed(lua_State* L);
    static int lua_input_get_action_strength(lua_State* L);
    
    // Scene tree functions
    static int lua_get_tree(lua_State* L);
    static int lua_change_scene(lua_State* L);
    
    // Timer functions
    static int lua_create_timer(lua_State* L);
    
    // Resource functions
    static int lua_load_resource(lua_State* L);
    static int lua_save_resource(lua_State* L);
    
    // Global constants setup
    static void setup_global_constants(lua_State* L);
    
    // Helper functions
    static void register_class_methods(lua_State* L, const char* class_name);
    static void push_variant_as_table(lua_State* L, const Variant& value);
};

#endif // GODOT_API_BINDINGS_H
