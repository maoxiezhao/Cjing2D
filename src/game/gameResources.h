#pragma once

#include"common\common.h"
#include"game\gameResourceType.h"
#include"game\enumInfo.h"
#include"lua\luaData.h"

/**
*	\brief ��Ϸ������Դ
*
*	��Ϸ��Դ���dataĿ¼�¶�ȡresources�ļ�
*	�����ɶ�����gameReousrceType�е�ö�پ���
*/
class GameResource : public LuaData
{
public:
	/**
	*	\brief ��Ϸ��Դ��Ϣ
	*	resources�ļ��ᶨ��ΪTYPE{id = "xxxx"}
	*	��ȡ����Դ��Ϣ����vector����Ϣ����id
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

	/** ����������Ϸ��Դ��Ϣ */
	std::map<GameResourceType, GameResourceInfos> mResourceInfos;

};

template<>
struct EnumInfoTraits<GameResourceType>
{
	static const string prettyName;
	static const EnumInfo<GameResourceType>::nameType names;
};