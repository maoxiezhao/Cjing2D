#include"entity\entityData.h"

namespace {

/**
*	\brief entity 各个typel类型的属性描述
*/
const std::map<EntityType, const EntityData::EntityFieldDescriptions> entityTypeDescriptions = {
	{
		EntityType::TITLE, {				// entity type
			{"width",   EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0)},								
			{"height",  EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0)},
			{"pattern", EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0)}
		}
	},
	{
		EntityType::DYNAMIC_TITLE,{				
			{ "width",  EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0) },
			{ "height", EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0) }
		}
	},
	{
		EntityType::DESTIMATION,{
			{ "name",       EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT,  EntityData::EntityDefaultValue(string("")) },
			{ "direction",  EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT,  EntityData::EntityDefaultValue(string(""))}
		}
	},
	{
		EntityType::PICKABLE,{
			{ "itemName",  EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(string("")) },
			{ "itemCount", EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0) }
		}
	},
	{
		EntityType::ENEMEY,{
			{ "templ",      EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(string("")) },
			{ "direction",  EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0)}
		}
	},
	{
		EntityType::BULLET,{
			{ "templ",      EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(string("")) },
		}
	},
	{
		EntityType::BLOCK,{
			{ "sprite",     EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(string("")) },
			{ "push",       EntityData::EntityValueFlag::VALUE_FALG_DEFAULT, EntityData::EntityDefaultValue(false) },
			{ "pull",       EntityData::EntityValueFlag::VALUE_FALG_DEFAULT, EntityData::EntityDefaultValue(false) },
			{ "destory",    EntityData::EntityValueFlag::VALUE_FALG_DEFAULT, EntityData::EntityDefaultValue(false) },
		}
	},
};
}

/****** *****  EntityDefaultValue ******* ******/
EntityData::EntityDefaultValue::EntityDefaultValue():
	mType(VALUE_TYPE_STRING),
	mIntValue(0),
	mStringValue("")
{
}

EntityData::EntityDefaultValue::EntityDefaultValue(int value):
	mType(VALUE_TYPE_INTEGER),
	mIntValue(value),
	mStringValue("")
{
}

EntityData::EntityDefaultValue::EntityDefaultValue(const string & value):
	mType(VALUE_TYPE_STRING),
	mIntValue(0),
	mStringValue(value)
{
}

EntityData::EntityDefaultValue::EntityDefaultValue(bool value):
	mType(VALUE_TYPE_BOOLEAN),
	mIntValue(static_cast<int>(value)),
	mStringValue("")
{
}

/********* ********  EntityData ********* ********/
EntityData::EntityData():
	mName(""),
	mPos(),
	mLayer(0),
	mType(EntityType::UNKNOW)
{
}

const std::map<EntityType, const EntityData::EntityFieldDescriptions>& EntityData::GetEntityFieldDescriptions()
{
	return entityTypeDescriptions;
}

/**
*	\brief 根据entityType类型创建对应的entityData
*/
EntityData EntityData::CheckEntityData(lua_State * l, int index, EntityType type)
{
	LuaTools::CheckType(l, index, LUA_TTABLE);
	// default value
	int x = LuaTools::CheckFieldIntByDefault(l, index, "x", 0);
	int y = LuaTools::CheckFieldIntByDefault(l, index, "y", 0);
	int layer = LuaTools::CheckFieldIntByDefault(l, index, "layer", 0);
	const std::string name = LuaTools::CheckFieldStringByDefault(l, index, "name", "");

	EntityData entityData;
	entityData.SetEntityType(type);
	entityData.SetLayer(layer);
	entityData.SetPos({ x, y });
	entityData.SetName(name);

	// 根据entityTypeDecription来填充特有的属性
	const EntityData::EntityFieldDescriptions& entityFieldDescriptions = entityTypeDescriptions.at(type);
	for (const auto& entityFieldDescription : entityFieldDescriptions)
	{
		const std::string& key = entityFieldDescription._key;
		EntityValueFlag flag = entityFieldDescription._flag;
		EntityDefaultValue defaultValue = entityFieldDescription._default;

		string tmpStrValue;
		int tmpIntValue = 0;
		switch (defaultValue.mType)
		{
		case EntityValueType::VALUE_TYPE_STRING:
			if (flag == EntityValueFlag::VALUE_FALG_DEFAULT)
			{
				tmpStrValue = LuaTools::CheckFieldStringByDefault(l, index, key, defaultValue.mStringValue);
			}
			else
			{
				tmpStrValue = LuaTools::CheckFieldString(l, index, key);
			}
			entityData.SetValueString(key, tmpStrValue);
			break;
		case EntityValueType::VALUE_TYPE_BOOLEAN:
			if (flag == EntityValueFlag::VALUE_FALG_DEFAULT)
			{
				tmpIntValue = static_cast<int>(LuaTools::CheckFieldBoolByDefault(l, index, key, static_cast<bool>(defaultValue.mIntValue)) );
			}
			else
			{
				tmpIntValue = static_cast<int>(LuaTools::CheckFieldBool(l, index, key));
			}
			entityData.SetValueBoolean(key, tmpIntValue);
			break;
		case EntityValueType::VALUE_TYPE_INTEGER:
			if (flag == EntityValueFlag::VALUE_FALG_DEFAULT)
			{
				tmpIntValue = LuaTools::CheckFieldIntByDefault(l, index, key, defaultValue.mIntValue);
			}
			else
			{
				tmpIntValue = LuaTools::CheckFieldInt(l, index, key);
			}
			entityData.SetValueInteger(key, tmpIntValue);
		}
	}

	return entityData;
}

bool EntityData::ImportFromLua(lua_State * l)
{
	return false;
}

void EntityData::SetValueBoolean(const string & key, bool value)
{
	EntityFieldDescription entityFieldDescription;
	entityFieldDescription._key = key;
	entityFieldDescription._flag = EntityValueFlag::VALUE_FALG_DEFAULT;
	entityFieldDescription._default = EntityDefaultValue(value);
	mValueField[key] = entityFieldDescription;
}

void EntityData::SetValueString(const string & key, const string & value)
{
	EntityFieldDescription entityFieldDescription;
	entityFieldDescription._key = key;
	entityFieldDescription._flag = EntityValueFlag::VALUE_FALG_DEFAULT;
	entityFieldDescription._default = EntityDefaultValue(value);
	mValueField[key] = entityFieldDescription;
}

void EntityData::SetValueInteger(const string & key, int value)
{
	EntityFieldDescription entityFieldDescription;
	entityFieldDescription._key = key;
	entityFieldDescription._flag = EntityValueFlag::VALUE_FALG_DEFAULT;
	entityFieldDescription._default = EntityDefaultValue(value);
	mValueField[key] = entityFieldDescription;
}

const EntityData::EntityFieldDescription & EntityData::GetValue(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");

	return it->second;
}

bool EntityData::GetValueBoolean(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");
	Debug::CheckAssertion(it->second._default.mType == EntityValueType::VALUE_TYPE_BOOLEAN,
		"Excepted boolean value.");

	return static_cast<bool>(it->second._default.mIntValue);
}

int EntityData::GetValueInteger(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");
	Debug::CheckAssertion(it->second._default.mType == EntityValueType::VALUE_TYPE_INTEGER,
		"Excepted boolean value.");

	return it->second._default.mIntValue;
}

const string & EntityData::GetValueString(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");
	Debug::CheckAssertion(it->second._default.mType == EntityValueType::VALUE_TYPE_STRING,
		"Excepted boolean value.");

	return it->second._default.mStringValue;
}
