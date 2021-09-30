struct RelativePath {
	RelativePath(std::string path);
	
	std::string path;
};
	
struct ScriptPath {
	ScriptPath(std::string raw);
	ScriptPath(RelativePath relative);
	std::string path;
};

struct AbsolutePath {
	explicit AbsolutePath(std::string raw);
	AbsolutePath(RelativePath relative);
	AbsolutePath(ScriptPath absolute);
	
	std::string path;
};

std::string absolute_path(std::string relative);
void make_path_platform_dependent(std::string& path);

std::string relative_path(std::string absolute);
std::string path_join(std::vector<std::string> items);
std::string script_path(std::string script);
void normalize_path(std::string& str);
std::string filename_from_path(std::string path);
std::string name_from_full_path(std::string path);
std::string strip_extension(std::string filename);
bool is_valid_filename(std::string& str);
bool is_png(std::string& asset_path);
bool is_lua(std::string& path);

void        physfs_free(void* list);
char**      physfs_enumerate(std::string pi_path);
void        physfs_mount(std::string relative);
bool        check_physfs_error();
void        init_filesystem();
