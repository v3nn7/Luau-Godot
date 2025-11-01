#include "luau_definitions_generator.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/dir_access.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

// Podstawowe klasy Godota dla definicji
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/canvas_item.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/engine.hpp>

LuauDefinitionsGenerator* LuauDefinitionsGenerator::singleton = nullptr;

LuauDefinitionsGenerator::LuauDefinitionsGenerator() {
    singleton = this;
    
    // Dodaj podstawowe typy Luau
    builtin_types.push_back("nil");
    builtin_types.push_back("boolean");
    builtin_types.push_back("number");
    builtin_types.push_back("string");
    builtin_types.push_back("function");
    builtin_types.push_back("table");
    builtin_types.push_back("userdata");
    builtin_types.push_back("thread");
    
    // Typy Godota
    builtin_types.push_back("Vector2");
    builtin_types.push_back("Vector3");
    builtin_types.push_back("Color");
    builtin_types.push_back("Rect2");
    builtin_types.push_back("Transform2D");
    builtin_types.push_back("Transform3D");
    builtin_types.push_back("Array");
    builtin_types.push_back("Dictionary");
}

LuauDefinitionsGenerator* LuauDefinitionsGenerator::get_singleton() {
    if (!singleton) {
        singleton = memnew(LuauDefinitionsGenerator);
    }
    return singleton;
}

void LuauDefinitionsGenerator::generate_all_definitions() {
    UtilityFunctions::print("Generating Luau type definitions...");
    
    // Wyczyść poprzednie definicje
    class_definitions.clear();
    global_functions.clear();
    
    // Generuj definicje
    generate_builtin_types();
    generate_global_definitions();
    generate_singleton_definitions();
    generate_node_definitions();
    generate_resource_definitions();
    
    // Odkryj klasy automatycznie
    discover_godot_classes();
    
    UtilityFunctions::print("Generated " + String::num_int64(class_definitions.size()) + " class definitions");
}

void LuauDefinitionsGenerator::generate_builtin_types() {
    // Vector2 definicja
    ClassDefinition vector2;
    vector2.class_name = "Vector2";
    vector2.description = "2D vector class with x and y components";
    
    // Właściwości Vector2
    PropertyDefinition x_prop = {"x", "number", "X component of the vector", true, true};
    PropertyDefinition y_prop = {"y", "number", "Y component of the vector", true, true};
    vector2.properties.push_back(x_prop);
    vector2.properties.push_back(y_prop);
    
    // Metody Vector2
    MethodDefinition length_method;
    length_method.name = "length";
    length_method.return_type = "number";
    length_method.description = "Returns the length of the vector";
    length_method.is_const = true;
    vector2.methods.push_back(length_method);
    
    MethodDefinition normalized_method;
    normalized_method.name = "normalized";
    normalized_method.return_type = "Vector2";
    normalized_method.description = "Returns a normalized version of the vector";
    normalized_method.is_const = true;
    vector2.methods.push_back(normalized_method);
    
    MethodDefinition dot_method;
    dot_method.name = "dot";
    dot_method.return_type = "number";
    dot_method.parameters.push_back("other");
    dot_method.parameter_types.push_back("Vector2");
    dot_method.description = "Returns the dot product with another vector";
    dot_method.is_const = true;
    vector2.methods.push_back(dot_method);
    
    // Konstanty Vector2
    vector2.constants["ZERO"] = "Vector2(0, 0)";
    vector2.constants["ONE"] = "Vector2(1, 1)";
    vector2.constants["UP"] = "Vector2(0, -1)";
    vector2.constants["DOWN"] = "Vector2(0, 1)";
    vector2.constants["LEFT"] = "Vector2(-1, 0)";
    vector2.constants["RIGHT"] = "Vector2(1, 0)";
    
    class_definitions["Vector2"] = vector2;
    
    // Vector3 definicja
    ClassDefinition vector3;
    vector3.class_name = "Vector3";
    vector3.description = "3D vector class with x, y, and z components";
    
    PropertyDefinition x3_prop = {"x", "number", "X component", true, true};
    PropertyDefinition y3_prop = {"y", "number", "Y component", true, true}; 
    PropertyDefinition z3_prop = {"z", "number", "Z component", true, true};
    vector3.properties.push_back(x3_prop);
    vector3.properties.push_back(y3_prop);
    vector3.properties.push_back(z3_prop);
    
    vector3.constants["ZERO"] = "Vector3(0, 0, 0)";
    vector3.constants["ONE"] = "Vector3(1, 1, 1)";
    vector3.constants["UP"] = "Vector3(0, 1, 0)";
    vector3.constants["DOWN"] = "Vector3(0, -1, 0)";
    vector3.constants["FORWARD"] = "Vector3(0, 0, -1)";
    vector3.constants["BACK"] = "Vector3(0, 0, 1)";
    vector3.constants["LEFT"] = "Vector3(-1, 0, 0)";
    vector3.constants["RIGHT"] = "Vector3(1, 0, 0)";
    
    class_definitions["Vector3"] = vector3;
}

void LuauDefinitionsGenerator::generate_global_definitions() {
    // Globalne funkcje
    global_functions["print"] = "function print(...any): nil -- Prints values to console";
    global_functions["get_node"] = "function get_node(path: string): Node? -- Gets a node by path";
    global_functions["create_node"] = "function create_node(class_name: string): Node? -- Creates a new node";
    global_functions["load_resource"] = "function load_resource(path: string): Resource? -- Loads a resource";
    global_functions["preload"] = "function preload(path: string): Resource -- Preloads a resource at compile time";
    
    // Math functions
    global_functions["sin"] = "function sin(angle: number): number -- Sine function";
    global_functions["cos"] = "function cos(angle: number): number -- Cosine function";
    global_functions["tan"] = "function tan(angle: number): number -- Tangent function";
    global_functions["sqrt"] = "function sqrt(value: number): number -- Square root";
    global_functions["abs"] = "function abs(value: number): number -- Absolute value";
    global_functions["min"] = "function min(a: number, b: number): number -- Minimum of two numbers";
    global_functions["max"] = "function max(a: number, b: number): number -- Maximum of two numbers";
    global_functions["clamp"] = "function clamp(value: number, min_val: number, max_val: number): number -- Clamps value between min and max";
    global_functions["lerp"] = "function lerp(from: number, to: number, weight: number): number -- Linear interpolation";
    global_functions["move_toward"] = "function move_toward(from: number, to: number, delta: number): number -- Move toward target by delta";
    
    // Conversion functions
    global_functions["deg_to_rad"] = "function deg_to_rad(degrees: number): number -- Convert degrees to radians";
    global_functions["rad_to_deg"] = "function rad_to_deg(radians: number): number -- Convert radians to degrees";
    
    // Type constructors
    global_functions["Vector2"] = "function Vector2(x: number?, y: number?): Vector2 -- Creates a new Vector2";
    global_functions["Vector3"] = "function Vector3(x: number?, y: number?, z: number?): Vector3 -- Creates a new Vector3";
    global_functions["Color"] = "function Color(r: number?, g: number?, b: number?, a: number?): Color -- Creates a new Color";
}

void LuauDefinitionsGenerator::generate_singleton_definitions() {
    // Input singleton
    ClassDefinition input;
    input.class_name = "Input";
    input.description = "Input singleton for handling input events";
    
    MethodDefinition is_action_pressed;
    is_action_pressed.name = "is_action_pressed";
    is_action_pressed.return_type = "boolean";
    is_action_pressed.parameters.push_back("action");
    is_action_pressed.parameter_types.push_back("string");
    is_action_pressed.description = "Returns true if action is currently pressed";
    is_action_pressed.is_static = true;
    input.methods.push_back(is_action_pressed);
    
    MethodDefinition is_action_just_pressed;
    is_action_just_pressed.name = "is_action_just_pressed";
    is_action_just_pressed.return_type = "boolean";
    is_action_just_pressed.parameters.push_back("action");
    is_action_just_pressed.parameter_types.push_back("string");
    is_action_just_pressed.description = "Returns true if action was just pressed this frame";
    is_action_just_pressed.is_static = true;
    input.methods.push_back(is_action_just_pressed);
    
    MethodDefinition get_axis;
    get_axis.name = "get_axis";
    get_axis.return_type = "number";
    get_axis.parameters.push_back("negative_action");
    get_axis.parameters.push_back("positive_action");
    get_axis.parameter_types.push_back("string");
    get_axis.parameter_types.push_back("string");
    get_axis.description = "Returns axis value between -1 and 1";
    get_axis.is_static = true;
    input.methods.push_back(get_axis);
    
    input.constants["MOUSE_MODE_VISIBLE"] = "0";
    input.constants["MOUSE_MODE_HIDDEN"] = "1";
    input.constants["MOUSE_MODE_CAPTURED"] = "2";
    
    class_definitions["Input"] = input;
    
    // Engine singleton
    ClassDefinition engine;
    engine.class_name = "Engine";
    engine.description = "Engine singleton for engine-related functions";
    
    MethodDefinition get_frames_per_second;
    get_frames_per_second.name = "get_frames_per_second";
    get_frames_per_second.return_type = "number";
    get_frames_per_second.description = "Returns current FPS";
    get_frames_per_second.is_static = true;
    engine.methods.push_back(get_frames_per_second);
    
    class_definitions["Engine"] = engine;
}

void LuauDefinitionsGenerator::generate_node_definitions() {
    // Node base class
    ClassDefinition node;
    node.class_name = "Node";
    node.description = "Base class for all scene objects";
    
    // Właściwości Node
    PropertyDefinition name_prop = {"name", "string", "Name of the node", true, true};
    PropertyDefinition process_mode_prop = {"process_mode", "number", "Process mode of the node", true, true};
    node.properties.push_back(name_prop);
    node.properties.push_back(process_mode_prop);
    
    // Metody Node
    MethodDefinition get_child;
    get_child.name = "get_child";
    get_child.return_type = "Node";
    get_child.parameters.push_back("index");
    get_child.parameter_types.push_back("number");
    get_child.description = "Gets child node at index";
    node.methods.push_back(get_child);
    
    MethodDefinition get_node_method;
    get_node_method.name = "get_node";
    get_node_method.return_type = "Node?";
    get_node_method.parameters.push_back("path");
    get_node_method.parameter_types.push_back("string");
    get_node_method.description = "Gets node by path";
    node.methods.push_back(get_node_method);
    
    MethodDefinition add_child;
    add_child.name = "add_child";
    add_child.return_type = "nil";
    add_child.parameters.push_back("child");
    add_child.parameter_types.push_back("Node");
    add_child.description = "Adds a child node";
    node.methods.push_back(add_child);
    
    MethodDefinition queue_free;
    queue_free.name = "queue_free";
    queue_free.return_type = "nil";
    queue_free.description = "Queues the node for deletion";
    node.methods.push_back(queue_free);
    
    // Lifecycle methods (będą overridowane w skryptach)
    MethodDefinition ready;
    ready.name = "_ready";
    ready.return_type = "nil";
    ready.description = "Called when node is ready";
    node.methods.push_back(ready);
    
    MethodDefinition process;
    process.name = "_process";
    process.return_type = "nil";
    process.parameters.push_back("delta");
    process.parameter_types.push_back("number");
    process.description = "Called every frame";
    node.methods.push_back(process);
    
    node.constants["PROCESS_MODE_INHERIT"] = "0";
    node.constants["PROCESS_MODE_PAUSABLE"] = "1";
    node.constants["PROCESS_MODE_WHEN_PAUSED"] = "2";
    node.constants["PROCESS_MODE_DISABLED"] = "3";
    
    class_definitions["Node"] = node;
    
    // Node2D class
    ClassDefinition node2d;
    node2d.class_name = "Node2D";
    node2d.parent_class = "Node";
    node2d.description = "2D scene node with position, rotation, and scale";
    
    PropertyDefinition position_prop = {"position", "Vector2", "Position in 2D space", true, true};
    PropertyDefinition rotation_prop = {"rotation", "number", "Rotation in radians", true, true};
    PropertyDefinition scale_prop = {"scale", "Vector2", "Scale factor", true, true};
    node2d.properties.push_back(position_prop);
    node2d.properties.push_back(rotation_prop);
    node2d.properties.push_back(scale_prop);
    
    class_definitions["Node2D"] = node2d;
    
    // CharacterBody2D class
    ClassDefinition char_body2d;
    char_body2d.class_name = "CharacterBody2D";
    char_body2d.parent_class = "Node2D";
    char_body2d.description = "2D character physics body";
    
    PropertyDefinition velocity_prop = {"velocity", "Vector2", "Current velocity", true, true};
    char_body2d.properties.push_back(velocity_prop);
    
    MethodDefinition move_and_slide;
    move_and_slide.name = "move_and_slide";
    move_and_slide.return_type = "boolean";
    move_and_slide.description = "Moves the body and slides along surfaces";
    char_body2d.methods.push_back(move_and_slide);
    
    MethodDefinition is_on_floor;
    is_on_floor.name = "is_on_floor";
    is_on_floor.return_type = "boolean";
    is_on_floor.description = "Returns true if on floor";
    char_body2d.methods.push_back(is_on_floor);
    
    MethodDefinition physics_process;
    physics_process.name = "_physics_process";
    physics_process.return_type = "nil";
    physics_process.parameters.push_back("delta");
    physics_process.parameter_types.push_back("number");
    physics_process.description = "Called every physics frame";
    char_body2d.methods.push_back(physics_process);
    
    class_definitions["CharacterBody2D"] = char_body2d;
}

void LuauDefinitionsGenerator::generate_resource_definitions() {
    ClassDefinition resource;
    resource.class_name = "Resource";
    resource.description = "Base class for all resources";
    
    PropertyDefinition resource_path_prop = {"resource_path", "string", "Path to the resource", true, false};
    resource.properties.push_back(resource_path_prop);
    
    class_definitions["Resource"] = resource;
}

String LuauDefinitionsGenerator::export_to_luau_definitions() {
    String output = "-- Godot API Definitions for Luau\n";
    output += "-- Auto-generated by Godot Luau Extension\n\n";
    
    // Export global functions
    output += "-- Global Functions\n";
    for (HashMap<String, String>::Iterator it = global_functions.begin(); it != global_functions.end(); ++it) {
        output += "declare " + it->value + "\n";
    }
    output += "\n";
    
    // Export classes
    for (HashMap<String, ClassDefinition>::Iterator it = class_definitions.begin(); it != class_definitions.end(); ++it) {
        output += export_class_to_luau(it->value) + "\n";
    }
    
    return output;
}

String LuauDefinitionsGenerator::export_class_to_luau(const ClassDefinition& def) {
    String output = String("-- ") + def.description + String("\n");
    
    if (!def.parent_class.is_empty()) {
        output += "export type " + def.class_name + " = " + def.parent_class + " & {\n";
    } else {
        output += "export type " + def.class_name + " = {\n";
    }
    
    // Properties
    for (const PropertyDefinition& prop : def.properties) {
        output += String("    ") + prop.name + String(": ") + prop.type;
        if (!prop.writable) {
            output += String(" --[[readonly]]");
        }
        output += String(", -- ") + prop.description + String("\n");
    }
    
    // Methods
    for (const MethodDefinition& method : def.methods) {
        output += String("    ") + method.name + String(": (");
        
        // Self parameter for non-static methods
        if (!method.is_static) {
            output += String("self: ") + def.class_name;
            if (!method.parameters.is_empty()) {
                output += String(", ");
            }
        }
        
        // Method parameters
        for (int i = 0; i < method.parameters.size(); i++) {
            if (i > 0) output += String(", ");
            output += method.parameters[i] + String(": ") + method.parameter_types[i];
        }
        
        output += String(") -> ") + method.return_type + String(", -- ") + method.description + String("\n");
    }
    
    // Constants
    for (HashMap<String, String>::ConstIterator it = def.constants.begin(); it != def.constants.end(); ++it) {
        output += String("    ") + it->key + String(": ") + it->value + String(",\n");
    }
    
    output += "}\n\n";
    
    // Constructor function
    output += String("declare function ") + def.class_name + String("(): ") + def.class_name + String("\n\n");
    
    return output;
}

void LuauDefinitionsGenerator::export_definitions_to_file(const String& output_path) {
    String definitions = export_to_luau_definitions();
    
    Ref<FileAccess> file = FileAccess::open(output_path, FileAccess::WRITE);
    if (file.is_null()) {
        UtilityFunctions::print(String("Failed to create definitions file: ") + output_path);
        return;
    }
    
    file->store_string(definitions);
    file->close();
    
    UtilityFunctions::print(String("Luau definitions exported to: ") + output_path);
}

void LuauDefinitionsGenerator::discover_godot_classes() {
    // To będzie rozszerzone z prawdziwym API discovery
    UtilityFunctions::print("Auto-discovering Godot classes...");
    
    // Na razie tylko podstawowe klasy, w pełnej implementacji 
    // użyjemy ClassDB::get_class_list() i reflection API
}

String LuauDefinitionsGenerator::get_luau_type_from_variant(Variant::Type type) {
    switch (type) {
        case Variant::NIL: return "nil";
        case Variant::BOOL: return "boolean";
        case Variant::INT: return "number";
        case Variant::FLOAT: return "number";
        case Variant::STRING: return "string";
        case Variant::VECTOR2: return "Vector2";
        case Variant::VECTOR3: return "Vector3";
        case Variant::ARRAY: return "Array";
        case Variant::DICTIONARY: return "Dictionary";
        case Variant::OBJECT: return "Object";
        default: return "any";
    }
}

// Simple JSON export used by editors for autocomplete and inspection
String LuauDefinitionsGenerator::export_to_json() {
    String json = "{";

    // Globals
    json += "\"globals\":[";
    bool first_global = true;
    for (HashMap<String, String>::Iterator it = global_functions.begin(); it != global_functions.end(); ++it) {
        if (!first_global) json += ",";
        first_global = false;
        json += String("{\"name\":\"") + it->key + String("\",\"signature\":\"") + it->value + String("\"}");
    }
    json += "],";

    // Builtin types
    json += "\"builtin_types\":[";
    for (int i = 0; i < builtin_types.size(); i++) {
        if (i > 0) json += ",";
        json += String("\"") + builtin_types[i] + String("\"");
    }
    json += "],";

    // Classes
    json += "\"classes\":[";
    bool first_class = true;
    for (HashMap<String, ClassDefinition>::Iterator it = class_definitions.begin(); it != class_definitions.end(); ++it) {
        const ClassDefinition& def = it->value;
        if (!first_class) json += ",";
        first_class = false;
        json += String("{\"name\":\"") + def.class_name + String("\",");
        json += String("\"description\":\"") + def.description + String("\",");
        json += String("\"parent\":\"") + def.parent_class + String("\",");

        // Properties
        json += "\"properties\":[";
        for (int pi = 0; pi < def.properties.size(); pi++) {
            const PropertyDefinition& p = def.properties[pi];
            if (pi > 0) json += ",";
            json += String("{\"name\":\"") + p.name + String("\",\"type\":\"") + p.type + String("\"}");
        }
        json += "],";

        // Methods
        json += "\"methods\":[";
        for (int mi = 0; mi < def.methods.size(); mi++) {
            const MethodDefinition& m = def.methods[mi];
            if (mi > 0) json += ",";
            json += String("{\"name\":\"") + m.name + String("\",\"return_type\":\"") + m.return_type + String("\",\"is_static\":") + (m.is_static ? String("true") : String("false")) + String(",\"parameters\":[");
            for (int ai = 0; ai < m.parameters.size(); ai++) {
                if (ai > 0) json += ",";
                json += String("{\"name\":\"") + m.parameters[ai] + String("\",\"type\":\"") + m.parameter_types[ai] + String("\"}");
            }
            json += "]}";
        }
        json += "]}";
    }
    json += "]}";

    return json;
}

String LuauDefinitionsGenerator::generate_autocomplete_data() {
    // Editors can consume JSON; reuse export to provide data quickly
    return export_to_json();
}

