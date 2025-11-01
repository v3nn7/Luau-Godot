@tool
extends Node

# This script helps handle Luau script attachment in a way that won't crash the editor

func _ready():
	# Register for script creation signals if we're in the editor
	if Engine.is_editor_hint():
		# Get the editor interface
		var editor_interface = EditorPlugin.new().get_editor_interface()
		var script_editor = editor_interface.get_script_editor()
		
		# Connect to signals that might be triggered when attaching scripts
		if script_editor:
			script_editor.connect("editor_script_changed", _on_script_changed)
	
	print("Luau script handler initialized")

func _on_script_changed(script: Script) -> void:
	# Handle script changes safely
	if script and script.resource_path.ends_with(".luau"):
		print("Handling Luau script: " + script.resource_path)
		# Perform safe handling here

# Safe method to attach a Luau script to a node
static func attach_script_safely(node: Node, script_path: String) -> bool:
	if not FileAccess.file_exists(script_path):
		push_error("Script file not found: " + script_path)
		return false
		
	# Check if extension is properly registered first
	var luau_ext_path = "res://Addons/Luau-by-v3nn7/luau.gdextension"
	if not FileAccess.file_exists(luau_ext_path):
		push_error("Luau extension not found at: " + luau_ext_path)
		return false
		
	# Check for binary files
	var bin_dir = "res://Addons/Luau-by-v3nn7/bin"
	var dir = DirAccess.open(bin_dir)
	if not dir:
		push_error("Binary directory not found: " + bin_dir)
		return false
		
	# Safely try to load the script
	var script = load(script_path)
	if not script:
		push_error("Failed to load script: " + script_path)
		return false
		
	# Try to set the script on the node
	node.set_script(script)
	return true
