#ifndef LUAU_SCRIPT_LANGUAGE_H
#define LUAU_SCRIPT_LANGUAGE_H

#include <godot_cpp/classes/script_language_extension.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/hash_set.hpp>

#include <lua.h>
#include <lualib.h>

using namespace godot;

class LuauScript;

class LuauScriptLanguage : public ScriptLanguageExtension {
    GDCLASS(LuauScriptLanguage, ScriptLanguageExtension);

private:
    static LuauScriptLanguage *singleton;
    HashMap<String, Ref<LuauScript>> scripts;
    
    lua_State* L;
    bool initialized;

    void _setup_godot_api(lua_State* L);
    void _setup_sandboxing(lua_State* L);

public:
    static LuauScriptLanguage* get_singleton() { return singleton; }

    LuauScriptLanguage();
    ~LuauScriptLanguage();

    // ScriptLanguageExtension interface
    virtual String _get_name() const override;
    virtual void _init() override;
    virtual String _get_type() const override;
    virtual String _get_extension() const override;
    virtual PackedStringArray _get_recognized_extensions() const override;
    virtual void _finish() override;
    virtual PackedStringArray _get_reserved_words() const override;
    virtual bool _is_control_flow_keyword(const String &keyword) const override;
    virtual PackedStringArray _get_comment_delimiters() const override;
    virtual PackedStringArray _get_string_delimiters() const override;
    virtual PackedStringArray _get_doc_comment_delimiters() const override;
    virtual bool _supports_builtin_mode() const override;
    virtual bool _supports_documentation() const override;
    virtual bool _can_inherit_from_file() const override;
    virtual int32_t _find_function(const String &p_function, const String &p_code) const override;
    virtual String _make_function(const String &p_class_name, const String &p_function_name, const PackedStringArray &p_function_args) const override;
    virtual Ref<Script> _make_template(const String &p_template, const String &p_class_name, const String &p_base_class_name) const override;
    virtual TypedArray<Dictionary> _get_built_in_templates(const StringName &p_object) const override;
    virtual bool _is_using_templates() override;
    virtual Dictionary _validate(const String &p_script, const String &p_path, bool p_validate_functions, bool p_validate_errors, bool p_validate_warnings, bool p_validate_safe_lines) const override;
    virtual String _validate_path(const String &p_path) const override;
    virtual Object *_create_script() const override;
    virtual bool _has_named_classes() const override;
    virtual Dictionary _complete_code(const String &p_code, const String &p_path, Object *p_owner) const override;
    virtual Dictionary _lookup_code(const String &p_code, const String &p_symbol, const String &p_path, Object *p_owner) const override;
    virtual String _auto_indent_code(const String &p_code, int32_t p_from_line, int32_t p_to_line) const override;
    virtual void _add_global_constant(const StringName &p_name, const Variant &p_value) override;
    virtual void _add_named_global_constant(const StringName &p_name, const Variant &p_value) override;
    virtual void _remove_named_global_constant(const StringName &p_name) override;
    virtual void _thread_enter() override;
    virtual void _thread_exit() override;
    virtual String _debug_get_error() const override;
    virtual int32_t _debug_get_stack_level_count() const override;
    virtual int32_t _debug_get_stack_level_line(int32_t p_level) const override;
    virtual String _debug_get_stack_level_function(int32_t p_level) const override;
    virtual String _debug_get_stack_level_source(int32_t p_level) const override;
    virtual Dictionary _debug_get_stack_level_locals(int32_t p_level, int32_t p_max_subitems, int32_t p_max_depth) override;
    virtual Dictionary _debug_get_stack_level_members(int32_t p_level, int32_t p_max_subitems, int32_t p_max_depth) override;
    virtual void *_debug_get_stack_level_instance(int32_t p_level) override;
    virtual Dictionary _debug_get_globals(int32_t p_max_subitems, int32_t p_max_depth) override;
    virtual String _debug_parse_stack_level_expression(int32_t p_level, const String &p_expression, int32_t p_max_subitems, int32_t p_max_depth) override;
    virtual TypedArray<Dictionary> _debug_get_current_stack_info() override;
    virtual void _reload_all_scripts() override;
    virtual void _reload_tool_script(const Ref<Script> &p_script, bool p_soft_reload) override;
    virtual TypedArray<Dictionary> _get_public_functions() const override;
    virtual Dictionary _get_public_constants() const override;
    virtual TypedArray<Dictionary> _get_public_annotations() const override;
    virtual void _profiling_start() override;
    virtual void _profiling_stop() override;
    virtual int32_t _profiling_get_accumulated_data(ScriptLanguageExtensionProfilingInfo *p_info_array, int32_t p_info_max) override;
    virtual int32_t _profiling_get_frame_data(ScriptLanguageExtensionProfilingInfo *p_info_array, int32_t p_info_max) override;
    virtual void _frame() override;
    virtual bool _handles_global_class_type(const String &p_type) const override;
    virtual Dictionary _get_global_class_name(const String &p_path) const override;

    // Luau-specific methods
    lua_State* get_lua_state() const { return L; }
    bool execute_luau_code(const String& code, const String& path = "");
    void register_script(const String& path, Ref<LuauScript> script);
    void unregister_script(const String& path);
    
    // Type definitions for code hints
    void generate_type_definitions();
    String get_autocomplete_data() const;

protected:
    static void _bind_methods();
};

#endif // LUAU_SCRIPT_LANGUAGE_H
