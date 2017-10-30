#pragma once

#include"common\common.h"
#include"game\gameResourceType.h"
#include"game\enumInfo.h"
#include"lua\luaData.h"

/**
*	\brief 游戏基本资源
*
*	游戏资源会从data目录下读取resources文件
*	类型由定义在gameReousrceType中的枚举决定
*/
class GameResource : public LuaData
{
public:
	/**
	*	\brief 游戏资源信息
	*	resources文件会定义为TYPE{id = "xxxx"}
	*	读取的资源信息会以vector的信息保存id
	*/
	using GameResourceInfos = std::vector<string>;

	GameResourceInfos& GetGameResourcesInfos(GameResourceType type);
	const GameResourceInfos& GetGameResourcesInfos(GameResourceType type)const;
	void AddGameResourceInfo(GameResourceType type, const std::string id);
	void RemoveResourceInfo(GameResourceType type, const std::string id);
	bool ExistsResource(GameResourceType type, const std::string id);

	virtual bool ImportFromLua(lua_State*l);

	static GameResource& GetGameResource();

	// lua function
	static int LuaLoadResourceData(lua_State* l);
	static int LuaCheckDefualtType(lua_State* l, int index, GameResourceType type);
	static int LuaCheckItemType(lua_State* l, int index);

private:
	GameResource();

	/** 保存所有游戏资源信息 */
	std::map<GameResourceType, GameResourceInfos> mResourceInfos;

};

template<>
struct EnumInfoTraits<GameResourceType>
{
	static const string prettyName;
	static const EnumInfo<GameResourceType>::nameType names;
};