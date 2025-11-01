@tool
extends EditorPlugin

func _enter_tree():
	# Load and register the Luau language extension
	var luau_extension_path := _get_extension_path()
	_register_extension(luau_extension_path)

func _exit_tree():
	var luau_extension_path: String = _get_extension_path()
	_unregister_extension(luau_extension_path)

func _get_extension_path() -> String:
	var addon_dir: String = "res://Addons/Luau-by-v3nn7"
	var script: Script = get_script()
	if script:
		var resource_path: String = String(script.resource_path)
		if resource_path != "":
			addon_dir = resource_path.get_base_dir()
	return addon_dir.path_join("luau.gdextension")

func _register_extension(path: String) -> void:
	if not FileAccess.file_exists(path):
		push_warning("Luau GDExtension not found at %s" % path)
		return
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
