#pragma once

#include"common\common.h"
#include"lua\luaData.h"
#include"entity\entityInfo.h"
#include"utils\point.h"

/**
*	\brief entity����
*/
class EntityData : public LuaData
{
public:
	/***** ***** ***** entity filed data ***** ***** *****/
	// entity��ֵ�Ƿ�ʹ��Ĭ��ֵ
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

	//brief entity����Ĭ������
	struct EntityDefaultValue
	{
		EntityDefaultValue();
		EntityDefaultValue(int value);
		EntityDefaultValue(const string& value);

		EntityValueType mType;
		int mIntValue;			// ��ֵ����boolean
		string mStringValue;	
	};
	

	//brief entity���͵���������
	struct EntityFieldDescription
	{
		const std::string _key;			// ����������ֵ������
		EntityValueFlag _flag;
		EntityDefaultValue _default;	// ����������ֵ��Ĭ��ֵ����optionFlag�����Ƿ�ʹ��
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

	std::map<std::string, EntityFieldDescription> mValueField;	// ������

};