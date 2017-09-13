#pragma once

#include"common\common.h"
#include"lua\luaData.h"
#include"entity\entityInfo.h"
#include"utils\point.h"

/**
*	\brief entity数据
*/
class EntityData : public LuaData
{
public:
	/***** ***** ***** entity filed data ***** ***** *****/
	// entity的值是否使用默认值
	enum EntityValueFlag
	{
		VALUE_FLAG_NO_DEFAULT,
		VALUE_FALG_DEFAULT
	};

	enum EntityValueType
	{
		VALUE_TYPE_BOOLEAN,
		VALUE_TYPE_STRING,
		VALUE_TYPE_INTEGER
	};

	//brief entity类型默认数据
	struct EntityDefaultValue
	{
		EntityDefaultValue();
		EntityDefaultValue(int value);
		EntityDefaultValue(const string& value);

		EntityValueType mType;
		int mIntValue;			// 数值或者boolean
		string mStringValue;	
	};
	

	//brief entity类型的描述数据
	struct EntityFieldDescription
	{
		const std::string _key;			// 类型描述的值的名字
		EntityValueFlag _flag;
		EntityDefaultValue _default;	// 类型描述的值的默认值，由optionFlag决定是否使用
	};

	using EntityFieldDescriptions = std::vector<EntityFieldDescription>;
	

public:
	EntityData();

	static const std::map<EntityType, const EntityData::EntityFieldDescriptions>& GetEntityFieldDescriptions();
	static EntityData CheckEntityData(lua_State*l, int index, EntityType type);

	virtual bool ImportFromLua(lua_State*l);

	/**** **** getter/setter **** ****/

private:
	EntityType mType;
	string mName;
	int mLayer;
	Point2 mPos;

	std::map<std::string, EntityFieldDescription> mValueField;	// 变量域

};