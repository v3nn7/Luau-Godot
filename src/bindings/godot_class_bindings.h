#ifndef GODOT_CLASS_BINDINGS_H
#define GODOT_CLASS_BINDINGS_H

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include <lua.h>
#include <lualib.h>

using namespace godot;

class GodotClassBindings {
public:
    // Main setup function for all Godot classes
    static void setup_class_bindings(lua_State* L);
    
    // Core system classes
    static void register_node_classes(lua_State* L);
    static void register_resource_classes(lua_State* L);
    static void register_input_classes(lua_State* L);
    static void register_physics_classes(lua_State* L);
    static void register_audio_classes(lua_State* L);
    static void register_rendering_classes(lua_State* L);
    static void register_animation_classes(lua_State* L);
    static void register_gui_classes(lua_State* L);
    static void register_networking_classes(lua_State* L);
    static void register_io_classes(lua_State* L);
    static void register_math_classes(lua_State* L);
    
    // Global singletons
    static void register_singletons(lua_State* L);
    
    // Dynamic method calling system
    static int lua_object_method_call(lua_State* L);
    static int lua_object_property_get(lua_State* L);
    static int lua_object_property_set(lua_State* L);
    static int lua_object_signal_connect(lua_State* L);
    static int lua_object_signal_emit(lua_State* L);
    
    // Class instantiation
    static int lua_class_new(lua_State* L);
    static int lua_class_free(lua_State* L);
    
    // Utility functions
    static void push_godot_object(lua_State* L, Object* obj, const String& class_name = "");
    static Object* get_godot_object(lua_State* L, int index);
    static void create_class_metatable(lua_State* L, const char* class_name, const char* parent_class = nullptr);
    static void register_class_methods(lua_State* L, const char* class_name, const Vector<String>& methods);
    static void register_class_properties(lua_State* L, const char* class_name, const Vector<String>& properties);
    static void register_class_signals(lua_State* L, const char* class_name, const Vector<String>& signals);
    static void register_class_constants(lua_State* L, const char* class_name, const Dictionary& constants);
    
    // Method call helpers
    static Variant call_godot_method(Object* obj, const String& method, const Array& args);
    static bool set_godot_property(Object* obj, const String& property, const Variant& value);
    static Variant get_godot_property(Object* obj, const String& property);
    
private:
    // Internal structure to hold class information
    struct ClassInfo {
        String class_name;
        String parent_class;
        Vector<String> methods;
        Vector<String> properties;
        Vector<String> signals;
        Dictionary constants;
    };
    
    static HashMap<String, ClassInfo> class_registry;
    static void register_class_info(const ClassInfo& info);
    static ClassInfo get_class_info(const String& class_name);
    
    // Auto-generated binding helpers
    static void setup_auto_bindings(lua_State* L);
    static void create_inheritance_chain(lua_State* L, const char* class_name);
};

// Macro for easier class registration
#define REGISTER_GODOT_CLASS(L, CLASS_NAME) \
    GodotClassBindings::create_class_metatable(L, #CLASS_NAME); \
    lua_pushcfunction(L, GodotClassBindings::lua_class_new, "new"); \
    lua_setfield(L, -2, "new"); \
    lua_setglobal(L, #CLASS_NAME);

#define REGISTER_GODOT_SINGLETON(L, SINGLETON_NAME, METHOD_NAME) \
    lua_pushcfunction(L, [](lua_State* L) -> int { \
        auto singleton = SINGLETON_NAME::get_singleton(); \
        GodotClassBindings::push_godot_object(L, singleton, #SINGLETON_NAME); \
        return 1; \
    }, #METHOD_NAME); \
    lua_setglobal(L, #METHOD_NAME);

#endif // GODOT_CLASS_BINDINGS_H
