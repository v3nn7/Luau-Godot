#include "luau_resource_saver.h"

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/script.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace godot {

Error LuauResourceSaver::_save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
    Ref<Script> script = p_resource;
    if (script.is_null()) {
        return ERR_UNAVAILABLE;
    }

    Ref<FileAccess> fa = FileAccess::open(p_path, FileAccess::WRITE);
    if (fa.is_null()) {
        UtilityFunctions::printerr("[Luau] ResourceSaver: failed to open path for write: ", p_path);
        return ERR_CANT_OPEN;
    }

    String source = script->get_source_code();
    fa->store_string(source);
    fa->flush();
    fa->close();
    return OK;
}

Error LuauResourceSaver::_set_uid(const String &p_path, int64_t p_uid) {
    // Not used for plain text scripts.
    return OK;
}

bool LuauResourceSaver::_recognize(const Ref<Resource> &p_resource) const {
    Ref<Script> script = p_resource;
    if (script.is_null()) {
        return false;
    }
    
    // Check if this is a Luau script by examining the path
    String path = script->get_path();
    return path.ends_with(".luau");
}

PackedStringArray LuauResourceSaver::_get_recognized_extensions(const Ref<Resource> &p_resource) const {
    PackedStringArray exts;
    exts.push_back("luau");
    return exts;
}

bool LuauResourceSaver::_recognize_path(const Ref<Resource> &p_resource, const String &p_path) const {
    return p_path.ends_with(".luau");
}

} // namespace godot