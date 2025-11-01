@tool
extends EditorPlugin

# Script handler to prevent crashes on attachment
var script_handler: Node = null

func _enter_tree():
	# Load and register the Luau language extension
	var luau_extension_path := _get_extension_path()
	_register_extension(luau_extension_path)
	
	# Add script handler to prevent crashes when attaching scripts
	script_handler = Node.new()
	script_handler.name = "LuauScriptHandler"
	script_handler.set_script(load("res://Addons/Luau-by-v3nn7/luau_script_handler.gd"))
	add_child(script_handler)

func _exit_tree():
	# Unregister the extension
	var luau_extension_path: String = _get_extension_path()
	_unregister_extension(luau_extension_path)
	
	# Remove script handler
	if script_handler:
		script_handler.queue_free()
		script_handler = null

func _get_extension_path() -> String:
	var addon_dir: String = "res://Addons/Luau-by-v3nn7"
	var script: Script = get_script()
	if script:
		var resource_path: String = String(script.resource_path)
		if resource_path != "":
			addon_dir = resource_path.get_base_dir()
	return addon_dir.path_join("luau.gdextension")

func _register_extension(path: String) -> void:
	# Check if the GDExtension file exists
	if not FileAccess.file_exists(path):
		push_warning("Luau GDExtension not found at %s" % path)
		return
	
	# Check if the binary libraries directory exists
	var addon_dir = path.get_base_dir()
	var bin_dir = addon_dir.path_join("bin")
	var dir = DirAccess.open(addon_dir)
	if not dir or not dir.dir_exists("bin"):
		push_warning("Binary directory not found at %s. Creating it." % bin_dir)
		if dir:
			dir.make_dir("bin")
			push_warning("Created binary directory at %s" % bin_dir)
		else:
			push_error("Failed to create binary directory at %s" % bin_dir)
			return
	
	# Add the extension to the project settings
	var setting_path: String = "application/config/metadata/extension_list"
	var list: PackedStringArray = ProjectSettings.get_setting(setting_path, PackedStringArray())
	var res_path := ProjectSettings.localize_path(path)
	if res_path not in list:
		list.append(res_path)
		ProjectSettings.set_setting(setting_path, list)
		ProjectSettings.save()
		push_warning("Luau extension registered. Restart the editor to finish loading.")

func _unregister_extension(path: String) -> void:
	var setting_path := "application/config/metadata/extension_list"
	var list: PackedStringArray = ProjectSettings.get_setting(setting_path, PackedStringArray())
	var res_path := ProjectSettings.localize_path(path)
	if res_path in list:
		list.erase(res_path)
		ProjectSettings.set_setting(setting_path, list)
		ProjectSettings.save()
