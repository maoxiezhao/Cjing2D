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
	EntityData();

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
		EntityDefaultValue(bool value);

		EntityValueType mType;
		int mIntValue;			// ��ֵ����boolean
		string mStringValue;	
	};
	

	//brief entity���͵���������
	struct EntityFieldDescription
	{
		std::string _key;			// ����������ֵ������
		EntityValueFlag _flag;
		EntityDefaultValue _default;	// ����������ֵ��Ĭ��ֵ����optionFlag�����Ƿ�ʹ��
	};
	using EntityFieldDescriptions = std::vector<EntityFieldDescription>;

public:


	static const std::map<EntityType, const EntityData::EntityFieldDescriptions>& GetEntityFieldDescriptions();
	static EntityData CheckEntityData(lua_State*l, int index, EntityType type);

	virtual bool ImportFromLua(lua_State*l);

	/**** **** getter/setter **** ****/
	void SetEntityType(const EntityType& type)
	{
		mType = type;
	}
	EntityType GetEntityType()const
	{
		return mType;
	}
	void SetName(const string& name)
	{
		mName = name;
	}
	string GetName()const
	{
		return mName;
	}
	void SetLayer(int layer)
	{
		mLayer = layer;	
	}
	int GetLayer()const
	{
		return mLayer;
	}
	void SetPos(const Point2& pos)
	{
		mPos = pos;
	}
	Point2 GetPos()const
	{
		return mPos;
	}

	/**** **** value set **** ****/
	void SetValueBoolean(const string& key, bool value);
	void SetValueString(const string& key, const string& value);
	void SetValueInteger(const string& key, int value);

private:
	EntityType mType;
	string mName;
	int mLayer;
	Point2 mPos;

	std::map<std::string, EntityFieldDescription> mValueField;	// ������

};