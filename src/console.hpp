struct Console {
    char                  InputBuf[256];
    ImVector<char*>       Items;
    bool                  ScrollToBottom;
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;
	Console();
	~Console();
    
	// Portable helpers
	static int Stricmp(const char* str1, const char* str2);
	static int Strnicmp(const char* str1, const char* str2, int n);
	static char* Strdup(const char *str);
	static void  Strtrim(char* str);
	
	void ClearLog();
	void AddLog(const char* fmt, ...) IM_FMTARGS(2);
	void AddLog(std::string message);
	void Draw(const char* title);
	void ExecCommand(char* command_line);

	static int TextEditCallbackStub(ImGuiInputTextCallbackData* data); 
	int TextEditCallback(ImGuiInputTextCallbackData* data);
};
Console console;

sol::protected_function_result on_lua_error(lua_State*, sol::protected_function_result);
