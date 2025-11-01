#ifndef LUAU_SCRIPT_H
#define LUAU_SCRIPT_H

#include <godot_cpp/classes/script_extension.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/templates/hash_map.hpp>

#include <lua.h>

using namespace godot;

class LuauScriptLanguage;

class LuauScript : public ScriptExtension {
    GDCLASS(LuauScript, ScriptExtension);

private:
    String source_code;
    String path;
    String base_type;
    bool valid;
    bool tool;
    HashMap<StringName, Variant> constants;
    HashMap<StringName, Dictionary> methods;
    HashMap<StringName, Dictionary> properties;
    HashMap<StringName, Dictionary> signals;

    void _parse_script();
    void _extract_class_info();
    bool _has_method_in_script(const StringName& method_name) const;

public:
    LuauScript();
    ~LuauScript();

    // ScriptExtension interface
    virtual bool _editor_can_reload_from_file() override;
    virtual void _placeholder_erased(void *p_placeholder) override;
    virtual bool _can_instantiate() const override;
    virtual Ref<Script> _get_base_script() const override;
    virtual StringName _get_global_name() const override;
    virtual bool _inherits_script(const Ref<Script> &p_script) const override;
    virtual StringName _get_instance_base_type() const override;
    virtual void *_instance_create(Object *p_for_object) const override;
    virtual void *_placeholder_instance_create(Object *p_for_object) const override;
    virtual bool _instance_has(Object *p_object) const override;
    virtual bool _has_source_code() const override;
    virtual String _get_source_code() const override;
    virtual void _set_source_code(const String &p_code) override;
    virtual Error _reload(bool p_keep_state = false) override;
    virtual TypedArray<Dictionary> _get_documentation() const override;
    virtual String _get_class_icon_path() const override;
    virtual bool _has_method(const StringName &p_method) const override;
    virtual bool _has_static_method(const StringName &p_method) const override;
    virtual Dictionary _get_method_info(const StringName &p_method) const override;
    virtual bool _is_tool() const override;
    virtual bool _is_valid() const override;
    virtual bool _is_abstract() const override;
    virtual ScriptLanguage *_get_language() const override;
    virtual bool _has_script_signal(const StringName &p_signal) const override;
    virtual TypedArray<Dictionary> _get_script_signal_list() const override;
    virtual bool _has_property_default_value(const StringName &p_property) const override;
    virtual Variant _get_property_default_value(const StringName &p_property) const override;
    virtual void _update_exports() override;
    virtual TypedArray<Dictionary> _get_script_method_list() const override;
    virtual TypedArray<Dictionary> _get_script_property_list() const override;
    virtual int32_t _get_member_line(const StringName &p_member) const override;
    virtual Dictionary _get_constants() const override;
    virtual TypedArray<StringName> _get_members() const override;
    virtual bool _is_placeholder_fallback_enabled() const override;
    virtual Variant _get_rpc_config() const override;

    // Luau-specific methods
    bool load_source_code(const String &p_path);
    void set_path(const String &p_path) { path = p_path; }
    String get_path() const { return path; }
    void set_base_type(const String &p_base_type) { base_type = p_base_type; }
    String get_base_type() const { return base_type; }

protected:
    static void _bind_methods();
};

class LuauScriptInstance {
private:
    Object* owner;
    Ref<LuauScript> script;
    lua_State* L;
    int self_ref;
    HashMap<StringName, Variant> properties;

public:
    LuauScriptInstance();
    ~LuauScriptInstance();

    bool init(Ref<LuauScript> p_script, Object* p_object);
    void set_property(const StringName& p_name, const Variant& p_value);
    Variant get_property(const StringName& p_name);
    bool has_method(const StringName& p_method);
    Variant call_method(const StringName& p_method, const Variant** p_args, int p_argcount);
    
    // Godot lifecycle methods
    void call_ready();
    void call_process(double delta);
    void call_physics_process(double delta);
    void call_input(const Ref<class InputEvent>& event);
    
    Object* get_owner() const { return owner; }
    Ref<LuauScript> get_script() const { return script; }
};

#endif // LUAU_SCRIPT_H
