#include "luau_resource_loader.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include "luau_script_language/luau_script_language.h"
#include "luau_script/luau_script.h"

namespace godot {

PackedStringArray LuauResourceLoader::_get_recognized_extensions() const {
    PackedStringArray exts;
    exts.push_back("luau");
    return exts;
}

bool LuauResourceLoader::_recognize_path(const String &p_path, const StringName &p_type) const {
    // Wystarczy rozpoznać po rozszerzeniu; typ bywa pusty lub różny.
    return p_path.ends_with(".luau");
}

bool LuauResourceLoader::_handles_type(const StringName &p_type) const {
    return p_type == StringName("LuauScript") || p_type == StringName("Script");
}

String LuauResourceLoader::_get_resource_type(const String &p_path) const {
    if (p_path.ends_with(".luau")) {
        return "LuauScript";
    }
    return "";
}

Variant LuauResourceLoader::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
    String code = FileAccess::get_file_as_string(p_path);
    if (code.is_empty() && !FileAccess::file_exists(p_path)) {
        UtilityFunctions::printerr("[Luau] ResourceLoader: failed to open ", p_path);
        return Variant();
    }

    LuauScriptLanguage *lang = LuauScriptLanguage::get_singleton();
    if (!lang) {
        UtilityFunctions::printerr("[Luau] ResourceLoader: language singleton not available");
        return Variant();
    }

    Object *obj = lang->_create_script();
    if (!obj) {
        UtilityFunctions::printerr("[Luau] ResourceLoader: failed to create script object");
        return Variant();
    }

    LuauScript *raw_script = Object::cast_to<LuauScript>(obj);
    if (!raw_script) {
        UtilityFunctions::printerr("[Luau] ResourceLoader: created object is not a Script");
        return Variant();
    }

    Ref<LuauScript> script(raw_script);
    script->set_source_code(code);
    script->set_path(p_path);
    return script;
}

bool LuauResourceLoader::_exists(const String &p_path) const {
    return FileAccess::file_exists(p_path);
}

} // namespace godot