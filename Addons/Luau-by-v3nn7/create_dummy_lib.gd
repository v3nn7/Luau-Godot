@tool
extends EditorScript

func _run():
	# Create dummy DLL with a simple header structure
	var path = "res://Addons/Luau-by-v3nn7/bin/libluau-windows.template_debug.x86_64.dll"
	var file = FileAccess.open(path, FileAccess.WRITE)
	if file:
		# Write a minimal DLL header (MZ header)
		file.store_8(0x4D)  # 'M'
		file.store_8(0x5A)  # 'Z'
		# Fill with zeros to create a valid-looking file
		for i in range(0, 1024):
			file.store_8(0)
		file.close()
		print("Created dummy DLL at: " + path)
	else:
		printerr("Failed to create dummy DLL")
