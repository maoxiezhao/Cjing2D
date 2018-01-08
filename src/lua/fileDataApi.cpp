#include"lua\luaContext.h"
#include"lua\luaBinder.h"
#include"core\fileData.h"

const string LuaContext::module_file_data_name = "FileData";

/**
*	\brief FileData.Read(path)
*/
std::string file_api_read_file(const std::string& path)
{
	return FileData::ReadFile(path);
}

/**
*	\brief FileData.Exists(path)
*	\return true if exists or false
*/
bool file_api_exists_file(const std::string& path)
{
	return FileData::IsFileExists(path);
}

/**
*	\brief FileData.Save(path, buffer)
*/
bool file_api_save_file(const std::string& path, const std::string& buf )
{
	return FileData::SaveFile(path, buf);
}

void LuaContext::RegisterFileData(lua_State* l)
{
	DefaultLuaBinder bindClass(l, "FileData");

	bindClass.AddFunction("Read", file_api_read_file);
	bindClass.AddFunction("Exists", file_api_exists_file);
	bindClass.AddFunction("Save", file_api_save_file);
}

