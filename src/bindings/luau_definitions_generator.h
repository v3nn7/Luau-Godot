#ifndef LUAU_DEFINITIONS_GENERATOR_H
#define LUAU_DEFINITIONS_GENERATOR_H

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/templates/hash_map.hpp>

using namespace godot;

/**
 * Generator definicji typów Luau dla API Godota
 * Tworzy pliki .d.luau które umożliwiają podpowiedzi kodu w edytorach
 */
class LuauDefinitionsGenerator {
public:
    // Struktura definicji metody
    struct MethodDefinition {
        String name;
        String return_type;
        Vector<String> parameters;
        Vector<String> parameter_types;
        String description;
        bool is_static = false;
        bool is_const = false;
    };
    
    // Struktura definicji właściwości
    struct PropertyDefinition {
        String name;
        String type;
        String description;
        bool readable = true;
        bool writable = true;
    };
    
    // Struktura definicji sygnału
    struct SignalDefinition {
        String name;
        Vector<String> parameters;
        Vector<String> parameter_types;
        String description;
    };
    
    // Struktura definicji klasy
    struct ClassDefinition {
        String class_name;
        String parent_class;
        String description;
        Vector<MethodDefinition> methods;
        Vector<PropertyDefinition> properties;
        Vector<SignalDefinition> signals;
        HashMap<String, String> constants;
        Vector<String> enums;
    };

public:
    static LuauDefinitionsGenerator* get_singleton();
    
    // Główne metody generowania
    void generate_all_definitions();
    void generate_class_definition(const String& class_name);
    void export_definitions_to_file(const String& output_path);
    
    // Generowanie dla konkretnych kategorii
    void generate_node_definitions();
    void generate_resource_definitions();
    void generate_singleton_definitions();
    void generate_global_definitions();
    void generate_builtin_types();
    
    // Dodawanie własnych definicji
    void add_custom_class_definition(const ClassDefinition& definition);
    void add_custom_global_function(const String& name, const String& signature, const String& description);
    
    // Export do różnych formatów
    String export_to_luau_definitions();
    String export_to_json();
    String export_class_to_luau(const ClassDefinition& def);
    
    // Pomoc dla edytorów
    String generate_autocomplete_data();
    String generate_hover_info(const String& symbol);
    Vector<String> get_available_methods(const String& class_name);
    Vector<String> get_available_properties(const String& class_name);

private:
    static LuauDefinitionsGenerator* singleton;
    HashMap<String, ClassDefinition> class_definitions;
    HashMap<String, String> global_functions;
    Vector<String> builtin_types;
    
    LuauDefinitionsGenerator();
    
    // Pomocnicze metody
    String format_method_signature(const MethodDefinition& method);
    String format_property_definition(const PropertyDefinition& prop);
    String format_signal_definition(const SignalDefinition& signal);
    String escape_luau_string(const String& str);
    String get_luau_type_from_variant(Variant::Type type);
    
    // Auto-discovery API Godota
    void discover_godot_classes();
    ClassDefinition extract_class_info(const String& class_name);
    Vector<MethodDefinition> extract_class_methods(const String& class_name);
    Vector<PropertyDefinition> extract_class_properties(const String& class_name);
    Vector<SignalDefinition> extract_class_signals(const String& class_name);
};

#endif // LUAU_DEFINITIONS_GENERATOR_H
