#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/script_language_extension.hpp>

#include "luau_script_language/luau_script_language.h"
#include "luau_script/luau_script.h"
#include "luau_resource_saver.h"
#include "luau_resource_loader.h"

using namespace godot;

static LuauScriptLanguage *luau_language;
static Ref<LuauResourceSaver> luau_saver;
static Ref<LuauResourceLoader> luau_loader;

void initialize_luau_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    // Register our classes
    ClassDB::register_class<LuauScriptLanguage>();
    ClassDB::register_class<LuauScript>();
    ClassDB::register_class<LuauResourceSaver>();
    ClassDB::register_class<LuauResourceLoader>();

    // Create and register our script language
    luau_language = memnew(LuauScriptLanguage);
    Engine::get_singleton()->register_script_language(luau_language);

    // Register resource saver for .luau scripts
    luau_saver.instantiate();
    ResourceSaver::get_singleton()->add_resource_format_saver(luau_saver);

    luau_loader.instantiate();
    ResourceLoader::get_singleton()->add_resource_format_loader(luau_loader);
}

void uninitialize_luau_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

    if (luau_language) {
        Engine::get_singleton()->unregister_script_language(luau_language);
        memdelete(luau_language);
        luau_language = nullptr;
    }

    if (luau_saver.is_valid()) {
        ResourceSaver::get_singleton()->remove_resource_format_saver(luau_saver);
        luau_saver.unref();
    }

    if (luau_loader.is_valid()) {
        ResourceLoader::get_singleton()->remove_resource_format_loader(luau_loader);
        luau_loader.unref();
    }
}

extern "C" {
    // Initialization
    GDExtensionBool GDE_EXPORT luau_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_luau_module);
        init_obj.register_terminator(uninitialize_luau_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}
