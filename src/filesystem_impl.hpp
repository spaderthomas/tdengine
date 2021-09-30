// Don't use a leading slash. In general, mount directories with PhysFS to avoid
// having to make paths absolute. But sometimes, you need them (for instance, when
// mounting directories with PhysFS...)
std::string absolute_path(std::string relative) {
	// Check that we didn't pass this function an absolute path already
	if (relative.find(root_dir) != std::string::npos) {
		return relative;
	}
	return root_dir + relative;
}

// When we input real paths to PhysFS, we need to make sure they are for the
// platform we're on. This is the only time you should worry about this.
#ifdef WIN32
void make_path_platform_dependent(std::string& str) {
	string_replace(str, "/", "\\");
}
#else
void make_path_platform_dependent(std::string& str) {
	return;
}
#endif

// @kill
std::string relative_path(std::string absolute) {
	string_replace(absolute, root_dir, "");
	return absolute;
}

// @kill?
std::string path_join(std::vector<std::string> items) {
	std::string path = "";
	for (auto& item : items) {
		path += item + "/";
	}
	
	// Trim trailing slash
	return path.substr(0, path.size() - 1);
}

// @kill
std::string script_path(std::string script) {
	auto script_dir = absolute_path(path_join({"src", "scripts"}));
	auto path = path_join({ script_dir, script });
	normalize_path(path);
	return path;
}

// @kill
RelativePath::RelativePath(std::string path) {
	normalize_path(path);
	this->path = path;
}

// @kill
ScriptPath::ScriptPath(std::string raw) {
	normalize_path(raw);
	this->path = raw;
}
ScriptPath::ScriptPath(RelativePath relative) {
	std::string absolute = script_path(relative.path);
	normalize_path(absolute);
	this->path = absolute;
}

// @kill
AbsolutePath::AbsolutePath(std::string raw) {
	normalize_path(raw);
	this->path = raw;
}
	
AbsolutePath::AbsolutePath(RelativePath relative) {
	std::string absolute = absolute_path(relative.path);
	normalize_path(absolute);
	this->path = absolute;
}

// Script paths are stored absolutely, so we can convert for free
AbsolutePath::AbsolutePath(ScriptPath absolute) {
	this->path = absolute.path;
}

// @kill
/* Some utilities for dealing with files, directories, and paths */
// @note @spader 9/4/2019 Realllllllyyyyyyy need a better way of making paths good
#ifdef WIN32
void normalize_path(std::string& str) {
	string_replace(str, "/", "\\");
}
#else
void normalize_path(std::string& str) {
	return;
}
#endif

// Takes in a directory or file -- returns everything after the first slash
std::string filename_from_path(std::string path) {
	std::string name;
	for (int ichar = path.size() - 1; ichar > -1; ichar--) {
		if (path.at(ichar) == '/' || path.at(ichar) == '\\') { break; }
		name.insert(name.begin(), path.at(ichar));
	}
	
	return name;
}

std::string name_from_full_path(std::string path) {
	std::string asset_name;
	for (int ichar = path.size() - 1; ichar > -1; ichar--) {
		if (path.at(ichar) == '/' || path.at(ichar) == '\\') { break; }
		asset_name.insert(asset_name.begin(), path.at(ichar));
	}
	
	return asset_name;
}

// Accepts a filename, not a path. Returns all the characters before the first period.
std::string strip_extension(std::string filename) {
	std::string stripped;
	for (unsigned int ichar = 0; ichar < filename.size(); ichar++) {
		if (filename.at(ichar) == '.') {
			return stripped;
		}
		stripped.push_back(filename.at(ichar));
	}
	
	return stripped;
}

// Allowing alphanumerics, underscores, and periods
bool is_valid_filename(std::string& str) {
	auto is_numeric = [](char c) -> bool { return c >= '0' && c <= '9'; };
	auto is_alpha = [](char c) -> bool { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); };
	auto is_misc = [](char c) -> bool { return (c == '_') || c == '.'; };
	
	for (unsigned int ichar = 0; ichar < str.size(); ichar++) {
		char c = str.at(ichar);
		if (!(is_numeric(c) || is_alpha(c) || is_misc(c))) {
			return false;
		}
	}
	
	return true;
}

// @hack I'm sure there are PNG headers I could try parsing, but this works!
bool is_png(std::string& asset_path) {
	if (asset_path.size() < 5) { return false; } // "x.png" is the shortest name
	std::string should_be_png_extension = asset_path.substr(asset_path.size() - 4, 4);
	if (should_be_png_extension.compare(".png")) return false;
	return true;
}

bool is_lua(std::string& path) {
	if (path.size() < 5) { return false; } // "x.lua" is the shortest name
	std::string should_be_tds_extension = path.substr(path.size() - 4, 4);
	if (should_be_tds_extension.compare(".lua")) return false;
	return true;
}

void physfs_free(void* list) {
	PHYSFS_freeList(list);
}

char** physfs_enumerate(std::string pi_path) {
	auto files = PHYSFS_enumerateFiles(pi_path.c_str());
	check_physfs_error();
	return files;
}

void physfs_mount(std::string relative) {
	std::string absolute = absolute_path(relative);
	make_path_platform_dependent(absolute);
	int add_to_search_path = 1;
	PHYSFS_mount(absolute.c_str(), nullptr, add_to_search_path);
	check_physfs_error();
}

bool check_physfs_error() {
	PHYSFS_ErrorCode error = PHYSFS_getLastErrorCode();
	if (error != PHYSFS_ERR_OK) {
		const char* message = PHYSFS_getErrorByCode(error);
		tdns_log.write("@physfs_error");
		tdns_log.write(message);
		return false;
	}

	return true;
}

void init_filesystem() {
	int error = PHYSFS_init(nullptr);
	if (!check_physfs_error()) return;
	tdns_log.write("@physfs_init", Log_Flags::File);

	PHYSFS_setWriteDir(root_dir.c_str());
	
	physfs_mount("src/scripts/layouts");
	
	physfs_mount("asset");
	
	char** texture_names = physfs_enumerate("textures/backgrounds");
	defer { physfs_free(texture_names); };
	
	for (char** texture_name = texture_names; *texture_name; ++texture_name) {
		std::cout << *texture_name << std::endl;
	}
}
