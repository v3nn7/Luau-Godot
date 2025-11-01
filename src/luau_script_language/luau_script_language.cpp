#include "luau_script_language.h"
#include "../luau_script/luau_script.h"
#include "../bindings/godot_api_bindings.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/project_settings.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>

#include <lua.h>
#include <lualib.h>
#include <luacode.h>

using namespace godot;

LuauScriptLanguage *LuauScriptLanguage::singleton = nullptr;

LuauScriptLanguage::LuauScriptLanguage() {
    singleton = this;
    initialized = false;
    L = luaL_newstate();
    if (L) {
        luaL_openlibs(L);
        _setup_sandboxing(L);
        _setup_godot_api(L);
        initialized = true;
    }
}

LuauScriptLanguage::~LuauScriptLanguage() {
    if (L) {
        lua_close(L);
        L = nullptr;
    }
    if (singleton == this) singleton = nullptr;
}

void LuauScriptLanguage::_bind_methods() {
}

String LuauScriptLanguage::_get_name() const { return "Luau"; }
void LuauScriptLanguage::_init() {}
String LuauScriptLanguage::_get_type() const { return "LuauScript"; }
String LuauScriptLanguage::_get_extension() const { return "luau"; }
PackedStringArray LuauScriptLanguage::_get_recognized_extensions() const { return PackedStringArray(Array::make("luau")); }
void LuauScriptLanguage::_finish() {}
PackedStringArray LuauScriptLanguage::_get_reserved_words() const { return PackedStringArray(); }
bool LuauScriptLanguage::_is_control_flow_keyword(const String &keyword) const { return false; }
PackedStringArray LuauScriptLanguage::_get_comment_delimiters() const { return PackedStringArray(Array::make("--")); }
PackedStringArray LuauScriptLanguage::_get_string_delimiters() const { return PackedStringArray(Array::make("\"")); }
PackedStringArray LuauScriptLanguage::_get_doc_comment_delimiters() const { return PackedStringArray(); }
bool LuauScriptLanguage::_supports_builtin_mode() const { return true; }
bool LuauScriptLanguage::_supports_documentation() const { return false; }
bool LuauScriptLanguage::_can_inherit_from_file() const { return true; }
int32_t LuauScriptLanguage::_find_function(const String &p_function, const String &p_code) const { return -1; }

String LuauScriptLanguage::_make_function(const String &p_class_name, const String &p_function_name, const PackedStringArray &p_function_args) const {
    String args;
    for (int i = 0; i < p_function_args.size(); i++) {
        if (i > 0) args += ", ";
        args += p_function_args[i];
    }
    return String("function ") + p_function_name + "(" + args + ")\n    -- TODO\nend\n";
}

Ref<Script> LuauScriptLanguage::_make_template(const String &p_template, const String &p_class_name, const String &p_base_class_name) const {
    Ref<LuauScript> script;
    script.instantiate();
    String tmpl;
    if (p_template.is_empty()) {
        tmpl = String("-- Luau Script\nreturn {}\n");
    } else {
        tmpl = p_template;
    }
    script->_set_source_code(tmpl);
    if (!p_base_class_name.is_empty()) {
        script->set_base_type(p_base_class_name);
    }
    return script;
}

TypedArray<Dictionary> LuauScriptLanguage::_get_built_in_templates(const StringName &p_object) const {
    // Provide a minimal default template entry
    TypedArray<Dictionary> arr;
    Dictionary d;
    // Unique identifier required by Godot's ScriptLanguageExtension
    d["id"] = "default";
    // Origin of this template: 0 = builtin
    d["origin"] = 0;
    d["name"] = "Default";
    d["description"] = "Minimal Luau script";
    // Godot expects an "inherit" key to be present.
    d["inherit"] = "Node";
    // And a "content" key containing the template code.
    d["content"] = String("-- Luau Script\nreturn {}\n");
    arr.push_back(d);
    return arr;
}

bool LuauScriptLanguage::_is_using_templates() { return true; }

Dictionary LuauScriptLanguage::_validate(const String &p_script, const String &p_path, bool p_validate_functions, bool p_validate_errors, bool p_validate_warnings, bool p_validate_safe_lines) const {
    Dictionary result;
    result["errors"] = Array();
    result["valid"] = true;
    // Try to compile to check basic syntax
    if (!p_script.is_empty()) {
        CharString utf8 = p_script.utf8();
        size_t bytecodeSize = 0;
        char *bytecode = luau_compile(utf8.get_data(), utf8.length(), nullptr, &bytecodeSize);
        if (!bytecode) {
            Array errors;
            Dictionary err;
            err["message"] = "Luau compilation failed";
            err["line"] = 0;
            err["column"] = 0;
            errors.push_back(err);
            result["errors"] = errors;
            result["valid"] = false;
        } else {
            ::free(bytecode);
            result["valid"] = true;
        }
    }
    return result;
}

String LuauScriptLanguage::_validate_path(const String &p_path) const {
    UtilityFunctions::print("[LUAU DEBUG] _validate_path called with: ", p_path);
    if (!p_path.ends_with(".luau")) {
        return "Path must end with .luau extension";
    }
    // Ensure directory exists so ResourceSaver can write the file.
    String dir = p_path.get_base_dir();
    if (!dir.is_empty() && p_path.begins_with("res://")) {
        String abs_dir = ProjectSettings::get_singleton()->globalize_path(dir);
        String abs_file = ProjectSettings::get_singleton()->globalize_path(p_path);
        Error mkerr = DirAccess::make_dir_recursive_absolute(abs_dir);
        if (mkerr != OK) {
            UtilityFunctions::print("[LUAU DEBUG] _validate_path: make_dir_recursive failed for ", dir, ", err=", (int)mkerr);
        } else {
            UtilityFunctions::print("[LUAU DEBUG] _validate_path: ensured directory exists: ", dir);
            // Check existence via res:// and absolute path to detect cache inconsistencies.
            bool exists_res = FileAccess::file_exists(p_path);
            bool exists_abs = FileAccess::file_exists(abs_file);
            UtilityFunctions::print("[LUAU DEBUG] _validate_path: exists(res)=", exists_res, ", exists(abs)=", exists_abs, ", abs_file: ", abs_file);
            // Do not create files during validation; saving happens on confirmation.
        }
    }
    return "";
}

Object *LuauScriptLanguage::_create_script() const {
    UtilityFunctions::print("[LUAU DEBUG] _create_script() called");
    LuauScript *script = memnew(LuauScript);
    UtilityFunctions::print("[LUAU DEBUG] _create_script() created LuauScript at: ", (uint64_t)script);
    return script;
}

bool LuauScriptLanguage::_has_named_classes() const { return false; }

Dictionary LuauScriptLanguage::_complete_code(const String &p_code, const String &p_path, Object *p_owner) const {
    Dictionary result;
    // Godot requires 'result' to exist even if empty.
    result["result"] = Array();
    // 'force' (not 'forced') jest wymagany przez ScriptLanguageExtension.
    result["force"] = false;
    return result;
}

Dictionary LuauScriptLanguage::_lookup_code(const String &p_code, const String &p_symbol, const String &p_path, Object *p_owner) const {
    Dictionary result;
    // Godot expects a 'result' key indicating the lookup outcome.
    // 0 = LOOKUP_RESULT_SCRIPT_LOCATION
    result["result"] = 0;
    // Additional optional fields.
    result["type"] = 0; // LOOKUP_RESULT_SCRIPT_LOCATION
    result["location"] = -1;
    return result;
}

String LuauScriptLanguage::_auto_indent_code(const String &p_code, int32_t p_from_line, int32_t p_to_line) const {
    return p_code;
}

void LuauScriptLanguage::_add_global_constant(const StringName &p_name, const Variant &p_value) {
    if (!L) return;
    GodotApiBindings::variant_to_lua(L, p_value);
    lua_setglobal(L, String(p_name).utf8().get_data());
}

void LuauScriptLanguage::_add_named_global_constant(const StringName &p_name, const Variant &p_value) { _add_global_constant(p_name, p_value); }

void LuauScriptLanguage::_remove_named_global_constant(const StringName &p_name) {
    if (!L) return;
    lua_pushnil(L);
    lua_setglobal(L, String(p_name).utf8().get_data());
}

void LuauScriptLanguage::_thread_enter() {}
void LuauScriptLanguage::_thread_exit() {}

String LuauScriptLanguage::_debug_get_error() const { return L ? String() : String(); }
int32_t LuauScriptLanguage::_debug_get_stack_level_count() const { return 0; }
int32_t LuauScriptLanguage::_debug_get_stack_level_line(int32_t p_level) const { return 0; }
String LuauScriptLanguage::_debug_get_stack_level_function(int32_t p_level) const { return String(); }
String LuauScriptLanguage::_debug_get_stack_level_source(int32_t p_level) const { return String(); }
Dictionary LuauScriptLanguage::_debug_get_stack_level_locals(int32_t p_level, int32_t p_max_subitems, int32_t p_max_depth) { return Dictionary(); }
Dictionary LuauScriptLanguage::_debug_get_stack_level_members(int32_t p_level, int32_t p_max_subitems, int32_t p_max_depth) { return Dictionary(); }
void *LuauScriptLanguage::_debug_get_stack_level_instance(int32_t p_level) { return nullptr; }
Dictionary LuauScriptLanguage::_debug_get_globals(int32_t p_max_subitems, int32_t p_max_depth) { return Dictionary(); }
String LuauScriptLanguage::_debug_parse_stack_level_expression(int32_t p_level, const String &p_expression, int32_t p_max_subitems, int32_t p_max_depth) { return String(); }
TypedArray<Dictionary> LuauScriptLanguage::_debug_get_current_stack_info() { return TypedArray<Dictionary>(); }

void LuauScriptLanguage::_reload_all_scripts() {
    for (HashMap<String, Ref<LuauScript>>::Iterator it = scripts.begin(); it != scripts.end(); ++it) {
        if (it->value.is_valid()) {
            it->value->reload(false);
        }
    }
}

void LuauScriptLanguage::_reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) {
    if (p_script.is_valid()) {
        p_script->reload(p_soft_reload);
    }
}

TypedArray<Dictionary> LuauScriptLanguage::_get_public_functions() const { return TypedArray<Dictionary>(); }
Dictionary LuauScriptLanguage::_get_public_constants() const { return Dictionary(); }
TypedArray<Dictionary> LuauScriptLanguage::_get_public_annotations() const { return TypedArray<Dictionary>(); }

void LuauScriptLanguage::_profiling_start() {}
void LuauScriptLanguage::_profiling_stop() {}
int32_t LuauScriptLanguage::_profiling_get_accumulated_data(ScriptLanguageExtensionProfilingInfo *p_info_array, int32_t p_info_max) { return 0; }
int32_t LuauScriptLanguage::_profiling_get_frame_data(ScriptLanguageExtensionProfilingInfo *p_info_array, int32_t p_info_max) { return 0; }
void LuauScriptLanguage::_frame() {}

bool LuauScriptLanguage::_handles_global_class_type(const String &p_type) const { 
    // We don't expose global classes; let the editor handle base types normally.
    return false; 
}

Dictionary LuauScriptLanguage::_get_global_class_name(const String &p_path) const {
    Dictionary result;
    result["name"] = "";
    result["base_type"] = "";
    result["icon_path"] = "";
    return result;
}

bool LuauScriptLanguage::execute_luau_code(const String &code, const String &path) {
    if (!L || !initialized) {
        UtilityFunctions::print("Luau VM not initialized");
        return false;
    }
    // Use a synthetic chunk name for built-in scripts or unsaved resources.
    String chunk_name = path.is_empty() ? String("(luau_builtin)") : path;
    CharString utf8 = code.utf8();
    size_t bytecodeSize = 0;
    char *bytecode = luau_compile(utf8.get_data(), utf8.length(), nullptr, &bytecodeSize);
    if (!bytecode) {
        UtilityFunctions::print(String("Luau compilation failed for: ") + chunk_name);
        return false;
    }
    int result = luau_load(L, chunk_name.utf8().get_data(), bytecode, bytecodeSize, 0);
    ::free(bytecode);
    if (result != LUA_OK) {
        String error = lua_tostring(L, -1);
        lua_pop(L, 1);
        UtilityFunctions::print(String("Luau load error: ") + error);
        return false;
    }
    result = lua_pcall(L, 0, 0, 0);
    if (result != LUA_OK) {
        String error = lua_tostring(L, -1);
        lua_pop(L, 1);
        UtilityFunctions::print(String("Luau execution error: ") + error);
        return false;
    }
    return true;
}

void LuauScriptLanguage::register_script(const String &path, Ref<LuauScript> script) { scripts[path] = script; }
void LuauScriptLanguage::unregister_script(const String &path) { scripts.erase(path); }

void LuauScriptLanguage::_setup_godot_api(lua_State *Lstate) { GodotApiBindings::setup_bindings(Lstate); }

void LuauScriptLanguage::generate_type_definitions() {
    // Stub: no-op generator
    UtilityFunctions::print("Luau type definitions generation stub");
}

String LuauScriptLanguage::get_autocomplete_data() const {
    // Stub: return empty data
    return String();
}

void LuauScriptLanguage::_setup_sandboxing(lua_State *Lstate) {
    lua_pushnil(Lstate);
    lua_setglobal(Lstate, "loadfile");
    lua_pushnil(Lstate);
    lua_setglobal(Lstate, "dofile");
    lua_pushnil(Lstate);
    lua_setglobal(Lstate, "require");
}