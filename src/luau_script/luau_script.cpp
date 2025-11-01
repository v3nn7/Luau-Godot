#include "luau_script.h"
#include "../luau_script_language/luau_script_language.h"
#include "../bindings/godot_api_bindings.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/file_access.hpp>


#include <cstdlib>
#include <luacode.h>
#include <lualib.h>

// Compatibility for Luau reference helpers
#ifndef luau_ref
#define luau_ref lua_ref
#endif
#ifndef luau_unref
#define luau_unref lua_unref
#endif

LuauScript::LuauScript() {
    UtilityFunctions::print("[LUAU DEBUG] LuauScript constructor called");
    valid = false;
    tool = false;
    base_type = "Node";
    UtilityFunctions::print("[LUAU DEBUG] LuauScript constructor finished, this=", (uint64_t)this);
}

LuauScript::~LuauScript() {
    if (!path.is_empty()) {
        LuauScriptLanguage* lang = LuauScriptLanguage::get_singleton();
        if (lang) {
            lang->unregister_script(path);
        }
    }
}

void LuauScript::_bind_methods() { }

bool LuauScript::_editor_can_reload_from_file() {
    return true;
}

void LuauScript::_placeholder_erased(void *p_placeholder) {
    // Handle placeholder cleanup
}

bool LuauScript::_can_instantiate() const {
    return valid && !base_type.is_empty();
}

Ref<Script> LuauScript::_get_base_script() const {
    return Ref<Script>();
}

StringName LuauScript::_get_global_name() const {
    return StringName();
}

bool LuauScript::_inherits_script(const Ref<Script> &p_script) const {
    return false;
}

StringName LuauScript::_get_instance_base_type() const {
    return StringName(base_type);
}

void *LuauScript::_instance_create(Object *p_for_object) const {
    UtilityFunctions::print("[LUAU DEBUG] _instance_create called, valid: ", valid, ", path: '", path, "'");
    if (!valid) {
        UtilityFunctions::print("[LUAU DEBUG] _instance_create: script not valid, returning nullptr");
        return nullptr;
    }

    UtilityFunctions::print("[LUAU DEBUG] _instance_create: creating LuauScriptInstance");
    LuauScriptInstance* instance = memnew(LuauScriptInstance);
    UtilityFunctions::print("[LUAU DEBUG] _instance_create: calling init on instance");
    if (!instance->init(Ref<LuauScript>(this), p_for_object)) {
        UtilityFunctions::print("[LUAU DEBUG] _instance_create: init failed, deleting instance");
        memdelete(instance);
        return nullptr;
    }

    UtilityFunctions::print("[LUAU DEBUG] _instance_create: success, returning instance");
    return instance;
}

void *LuauScript::_placeholder_instance_create(Object *p_for_object) const {
    return _instance_create(p_for_object);
}

bool LuauScript::_instance_has(Object *p_object) const {
    // Check if object has this script instance
    return false; // Implementation depends on how instances are tracked
}

bool LuauScript::_has_source_code() const {
    return !source_code.is_empty();
}

String LuauScript::_get_source_code() const {
    // Debug: observe when editor requests source for saving
    UtilityFunctions::print("[LUAU DEBUG] _get_source_code called, length: ", source_code.length());
    // Also show the base Resource path the engine knows
    String res_path = Script::get_path();
    UtilityFunctions::print("[LUAU DEBUG] _get_source_code Resource::get_path(): '", res_path, "'");
    return source_code;
}

void LuauScript::_set_source_code(const String &p_code) {
    UtilityFunctions::print("[LUAU DEBUG] _set_source_code called, code length: ", p_code.length());
    source_code = p_code;

    // Avoid compiling during initial creation when script hasn't been saved yet.
    // Godot calls _set_source_code before assigning a resource path.
    String path = get_path();
    String res_path = Script::get_path();
    UtilityFunctions::print("[LUAU DEBUG] _set_source_code path: '", path, "'");
    UtilityFunctions::print("[LUAU DEBUG] _set_source_code Resource::get_path(): '", res_path, "'");

    // If our internal path is empty but the Resource has a path, sync it.
    if (path.is_empty() && !res_path.is_empty()) {
        this->set_path(res_path);
        path = res_path;
        UtilityFunctions::print("[LUAU DEBUG] _set_source_code: synced internal path from Resource: '", path, "'");
    }

    if (path.is_empty()) {
        UtilityFunctions::print("[LUAU DEBUG] _set_source_code: path is empty, but parsing anyway for new script");
        // Allow parsing even without path for new scripts
        _parse_script();
        return;
    }

    UtilityFunctions::print("[LUAU DEBUG] _set_source_code: calling _parse_script");
    _parse_script();
}

Error LuauScript::_reload(bool p_keep_state) {
    UtilityFunctions::print("[LUAU DEBUG] _reload called, path: '", path, "', keep_state=", p_keep_state ? 1 : 0);
    if (path.is_empty()) {
        // Built-in or unsaved script: parse from current source.
        UtilityFunctions::print("[LUAU DEBUG] _reload: path empty, parsing from source only");
        _parse_script();
        return valid ? OK : ERR_INVALID_DATA;
    }

    Ref<FileAccess> file = FileAccess::open(path, FileAccess::READ);
    if (file.is_null()) {
        UtilityFunctions::print("[LUAU DEBUG] _reload: failed to open file, returning ERR_FILE_CANT_OPEN");
        return ERR_FILE_CANT_OPEN;
    }

    source_code = file->get_as_text();
    file->close();

    _parse_script();
    return OK;
}

TypedArray<Dictionary> LuauScript::_get_documentation() const {
    return TypedArray<Dictionary>();
}

String LuauScript::_get_class_icon_path() const {
    return "res://Addons/Luau-by-v3nn7/icons/LuauScript.svg";
}

bool LuauScript::_has_method(const StringName &p_method) const {
    return methods.has(p_method) || _has_method_in_script(p_method);
}

bool LuauScript::_has_static_method(const StringName &p_method) const {
    return false; // Luau scripts don't support static methods in this implementation
}

Dictionary LuauScript::_get_method_info(const StringName &p_method) const {
    if (methods.has(p_method)) {
        return methods[p_method];
    }
    return Dictionary();
}

bool LuauScript::_is_tool() const {
    return tool;
}

bool LuauScript::_is_valid() const {
    return valid;
}

bool LuauScript::_is_abstract() const {
    return false;
}

ScriptLanguage *LuauScript::_get_language() const {
    return LuauScriptLanguage::get_singleton();
}

bool LuauScript::_has_script_signal(const StringName &p_signal) const {
    return signals.has(p_signal);
}

TypedArray<Dictionary> LuauScript::_get_script_signal_list() const {
    TypedArray<Dictionary> signal_list;
    for (HashMap<StringName, Dictionary>::ConstIterator it = signals.begin(); it != signals.end(); ++it) {
        signal_list.push_back(it->value);
    }
    return signal_list;
}

bool LuauScript::_has_property_default_value(const StringName &p_property) const {
    return properties.has(p_property);
}

Variant LuauScript::_get_property_default_value(const StringName &p_property) const {
    if (properties.has(p_property)) {
        Dictionary prop = properties[p_property];
        if (prop.has("default_value")) {
            return prop["default_value"];
        }
    }
    return Variant();
}

void LuauScript::_update_exports() {
    // Update exported properties
}

TypedArray<Dictionary> LuauScript::_get_script_method_list() const {
    TypedArray<Dictionary> method_list;
    for (HashMap<StringName, Dictionary>::ConstIterator it = methods.begin(); it != methods.end(); ++it) {
        method_list.push_back(it->value);
    }
    return method_list;
}

TypedArray<Dictionary> LuauScript::_get_script_property_list() const {
    TypedArray<Dictionary> property_list;
    for (HashMap<StringName, Dictionary>::ConstIterator it = properties.begin(); it != properties.end(); ++it) {
        property_list.push_back(it->value);
    }
    return property_list;
}

int32_t LuauScript::_get_member_line(const StringName &p_member) const {
    return 0; // TODO: Implement line tracking
}

Dictionary LuauScript::_get_constants() const {
    Dictionary const_dict;
    for (HashMap<StringName, Variant>::ConstIterator it = constants.begin(); it != constants.end(); ++it) {
        const_dict[it->key] = it->value;
    }
    return const_dict;
}

TypedArray<StringName> LuauScript::_get_members() const {
    TypedArray<StringName> members;
    
    for (HashMap<StringName, Dictionary>::ConstIterator it = methods.begin(); it != methods.end(); ++it) {
        members.push_back(it->key);
    }
    
    for (HashMap<StringName, Dictionary>::ConstIterator it = properties.begin(); it != properties.end(); ++it) {
        members.push_back(it->key);
    }
    
    for (HashMap<StringName, Variant>::ConstIterator it = constants.begin(); it != constants.end(); ++it) {
        members.push_back(it->key);
    }
    
    return members;
}

bool LuauScript::_is_placeholder_fallback_enabled() const {
    return false;
}

Variant LuauScript::_get_rpc_config() const {
    return Variant();
}

bool LuauScript::load_source_code(const String &p_path) {
    Ref<FileAccess> file = FileAccess::open(p_path, FileAccess::READ);
    if (file.is_null()) {
        return false;
    }

    source_code = file->get_as_text();
    path = p_path;
    file->close();

    _parse_script();
    
    // Register with language only if script is valid
    LuauScriptLanguage* lang = LuauScriptLanguage::get_singleton();
    if (lang && valid) {
        lang->register_script(path, Ref<LuauScript>(this));
    }

    return valid;
}

void LuauScript::_parse_script() {
    UtilityFunctions::print("[LUAU DEBUG] _parse_script called, path: '", path, "'");
    valid = false;
    methods.clear();
    properties.clear();
    constants.clear();
    signals.clear();
    
    if (source_code.is_empty()) {
        UtilityFunctions::print("[LUAU DEBUG] _parse_script: source_code is empty, returning");
        return;
    }

    // Allow parsing even if path is empty to support built-in scripts.
    if (path.is_empty()) {
        UtilityFunctions::print("[LUAU DEBUG] _parse_script: path is empty, continuing parse for built-in");
    }

    UtilityFunctions::print("[LUAU DEBUG] _parse_script: starting parsing, source length: ", source_code.length());

    // Basic parsing - look for extends declaration
    PackedStringArray lines = source_code.split("\n");
    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i].strip_edges();
        
        // Parse extends
        if (line.begins_with("extends = ")) {
            String type = line.substr(10).strip_edges();
            if (type.begins_with("\"") && type.ends_with("\"")) {
                base_type = type.substr(1, type.length() - 2);
            } else {
                base_type = type;
            }
        }
        
        // Parse tool declaration
        if (line == "@tool" || line.begins_with("-- @tool")) {
            tool = true;
        }
    }

    UtilityFunctions::print("[LUAU DEBUG] _parse_script: getting LuauScriptLanguage singleton");
    // Try to compile the script to validate syntax
    LuauScriptLanguage* lang = LuauScriptLanguage::get_singleton();
    if (!lang) {
        UtilityFunctions::print("LuauScriptLanguage singleton is null!");
        return;
    }

    UtilityFunctions::print("[LUAU DEBUG] _parse_script: calling luau_compile");
    CharString utf8 = source_code.utf8();
    size_t bytecodeSize = 0;
    char* bytecode = luau_compile(utf8.get_data(), utf8.length(), nullptr, &bytecodeSize);
    
    if (bytecode) {
        UtilityFunctions::print("[LUAU DEBUG] _parse_script: compilation successful");
        valid = true;
        ::free(bytecode);
        _extract_class_info();
    } else {
        UtilityFunctions::print(String("Luau script compilation failed: ") + path);
        valid = false;
    }
    UtilityFunctions::print("[LUAU DEBUG] _parse_script finished, valid: ", valid);
}

void LuauScript::_extract_class_info() {
    // Extract information about methods, properties, etc. from the script
    // This is a simplified implementation - a real one would parse the AST
    
    PackedStringArray lines = source_code.split("\n");
    for (int i = 0; i < lines.size(); i++) {
        String line = lines[i].strip_edges();
        
        // Look for function definitions
        if (line.begins_with("function ")) {
            String func_name = line.substr(9);
            int paren_pos = func_name.find("(");
            if (paren_pos > 0) {
                func_name = func_name.substr(0, paren_pos).strip_edges();
                
                Dictionary method_info;
                method_info["name"] = func_name;
                method_info["return_type"] = Variant::NIL;
                method_info["flags"] = 0;
                method_info["args"] = Array();
                
                methods[StringName(func_name)] = method_info;
            }
        }
        
        // Look for signal definitions (simplified)
        if (line.begins_with("signal ")) {
            String signal_name = line.substr(7).strip_edges();
            int paren_pos = signal_name.find("(");
            if (paren_pos > 0) {
                signal_name = signal_name.substr(0, paren_pos).strip_edges();
            }
            
            Dictionary signal_info;
            signal_info["name"] = signal_name;
            signal_info["args"] = Array();
            
            signals[StringName(signal_name)] = signal_info;
        }
    }
}

bool LuauScript::_has_method_in_script(const StringName& method_name) const {
    String method_str = String(method_name);
    return source_code.find("function " + method_str + "(") >= 0;
}

// LuauScriptInstance implementation

LuauScriptInstance::LuauScriptInstance() {
    owner = nullptr;
    L = nullptr;
    self_ref = LUA_NOREF;
}

LuauScriptInstance::~LuauScriptInstance() {
    if (L && self_ref != LUA_NOREF) {
        luau_unref(L, self_ref);
    }
}

bool LuauScriptInstance::init(Ref<LuauScript> p_script, Object* p_object) {
    if (p_script.is_null() || !p_object) {
        UtilityFunctions::print("[LUAU DEBUG] init: null script or object");
        return false;
    }

    script = p_script;
    owner = p_object;

    // Only proceed if script compiled; allow built-in scripts with empty path
    if (!script->_is_valid()) {
        UtilityFunctions::print("[LUAU DEBUG] init: script not valid");
        return false;
    }

    LuauScriptLanguage* lang = LuauScriptLanguage::get_singleton();
    if (!lang) {
        UtilityFunctions::print("[LUAU DEBUG] init: no language singleton");
        return false;
    }

    L = lang->get_lua_state();
    if (!L) {
        UtilityFunctions::print("[LUAU DEBUG] init: no lua state");
        return false;
    }

    // Create a table to represent this instance first
    lua_newtable(L);
    
    // Store reference to self before executing code
    self_ref = luau_ref(L, LUA_REGISTRYINDEX);
    
    // Setup owner reference in the instance table
    lua_rawgeti(L, LUA_REGISTRYINDEX, self_ref);
    GodotApiBindings::push_object(L, owner);
    lua_setfield(L, -2, "owner");
    lua_pop(L, 1);

    // Execute script code in a protected environment
    // Don't fail initialization if script execution fails - allow empty scripts
    String source_code = script->get_source_code();
    if (!source_code.is_empty()) {
        if (!lang->execute_luau_code(source_code, script->get_path())) {
            UtilityFunctions::print("[LUAU DEBUG] init: script execution failed, but continuing with empty instance");
            // Don't return false here - allow scripts with syntax errors to create instances
        }
    }

    UtilityFunctions::print("[LUAU DEBUG] init: success");
    return true;
}

void LuauScriptInstance::set_property(const StringName& p_name, const Variant& p_value) {
    properties[p_name] = p_value;
    
    if (L && self_ref != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, self_ref);
        GodotApiBindings::variant_to_lua(L, p_value);
        lua_setfield(L, -2, String(p_name).utf8().get_data());
        lua_pop(L, 1);
    }
}

Variant LuauScriptInstance::get_property(const StringName& p_name) {
    if (properties.has(p_name)) {
        return properties[p_name];
    }
    
    if (L && self_ref != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, self_ref);
        lua_getfield(L, -1, String(p_name).utf8().get_data());
        
        if (!lua_isnil(L, -1)) {
            Variant result = GodotApiBindings::lua_to_variant(L, -1);
            lua_pop(L, 2);
            return result;
        }
        lua_pop(L, 2);
    }
    
    return Variant();
}

bool LuauScriptInstance::has_method(const StringName& p_method) {
    return script.is_valid() && script->has_method(p_method);
}

Variant LuauScriptInstance::call_method(const StringName& p_method, const Variant** p_args, int p_argcount) {
    if (!L || self_ref == LUA_NOREF || !script.is_valid()) {
        UtilityFunctions::print("[LUAU DEBUG] call_method: invalid state");
        return Variant();
    }

    const char* method_cstr = String(p_method).utf8().get_data();
    
    // First check if the method exists in the global scope
    lua_getglobal(L, method_cstr);
    bool is_global_function = lua_isfunction(L, -1);
    
    if (!is_global_function) {
        lua_pop(L, 1); // Remove the nil/non-function value
        
        // Try to find the method in the instance table
        lua_rawgeti(L, LUA_REGISTRYINDEX, self_ref);
        if (lua_istable(L, -1)) {
            lua_getfield(L, -1, method_cstr);
            if (lua_isfunction(L, -1)) {
                // Found method in instance table
                lua_remove(L, -2); // Remove instance table, keep function
            } else {
                // Method not found anywhere
                lua_pop(L, 2); // Remove both values
                UtilityFunctions::print("[LUAU DEBUG] call_method: method '" + String(p_method) + "' not found");
                return Variant();
            }
        } else {
            lua_pop(L, 1); // Remove invalid instance table
            UtilityFunctions::print("[LUAU DEBUG] call_method: invalid instance table");
            return Variant();
        }
    }

    // At this point we have a function on the stack
    // Push self as first argument if it's not a global function
    if (!is_global_function) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, self_ref);
    }
    
    // Push other arguments
    for (int i = 0; i < p_argcount; i++) {
        if (p_args[i]) {
            GodotApiBindings::variant_to_lua(L, *p_args[i]);
        } else {
            lua_pushnil(L);
        }
    }

    // Call function with appropriate argument count
    int total_args = is_global_function ? p_argcount : p_argcount + 1;
    int result = lua_pcall(L, total_args, 1, 0);
    
    if (result != LUA_OK) {
        const char* err = lua_tostring(L, -1);
        UtilityFunctions::print(String("Luau error in ") + p_method + ": " + (err ? err : "unknown"));
        lua_pop(L, 1);
        return Variant();
    }

    // Convert return value
    Variant ret_value = Variant();
    if (!lua_isnil(L, -1)) {
        ret_value = GodotApiBindings::lua_to_variant(L, -1);
    }
    lua_pop(L, 1);
    
    return ret_value;
}

void LuauScriptInstance::call_ready() {
    const Variant** args = nullptr;
    call_method("_ready", args, 0);
}

void LuauScriptInstance::call_process(double delta) {
    Variant delta_var = delta;
    const Variant* args[] = { &delta_var };
    call_method("_process", args, 1);
}

void LuauScriptInstance::call_physics_process(double delta) {
    Variant delta_var = delta;
    const Variant* args[] = { &delta_var };
    call_method("_physics_process", args, 1);
}

void LuauScriptInstance::call_input(const Ref<class InputEvent>& event) {
    Variant event_var = event;
    const Variant* args[] = { &event_var };
    call_method("_input", args, 1);
}








