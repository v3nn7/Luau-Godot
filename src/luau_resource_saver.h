#pragma once

#include <godot_cpp/classes/resource_format_saver.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

// No direct dependency on LuauScript; use base Script.

namespace godot {

class LuauResourceSaver : public ResourceFormatSaver {
    GDCLASS(LuauResourceSaver, ResourceFormatSaver);

protected:
    static void _bind_methods() {}

public:
    virtual Error _save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) override;
    virtual Error _set_uid(const String &p_path, int64_t p_uid) override;
    virtual bool _recognize(const Ref<Resource> &p_resource) const override;
    virtual PackedStringArray _get_recognized_extensions(const Ref<Resource> &p_resource) const override;
    virtual bool _recognize_path(const Ref<Resource> &p_resource, const String &p_path) const override;
};

} // namespace godot