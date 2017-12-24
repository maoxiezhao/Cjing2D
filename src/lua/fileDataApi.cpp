#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"core\fileData.h"

const string LuaContext::module_window_name = "FileData";

std::string file_api_read_file(const std::string& path)
{

}

bool file_api_exists_file(const std::string& path)
{
	return true;
}

bool file_api_save_file(const std::string& path)
{
	return true;
}

void LuaContext::RegisterFileData(lua_State* l)
{
	LuaBindClass<void> bindClass(l, "FileData");

	bindClass.AddFunction("GetInstance", file_api_read_file);
	bindClass.AddFunction("GetInstance", file_api_read_file);
	bindClass.AddFunction("GetInstance", file_api_read_file);
}

