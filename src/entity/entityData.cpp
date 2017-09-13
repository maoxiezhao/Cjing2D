#include"entity\entityData.h"

namespace {

/**
*	\brief entity 各个typel类型的属性描述
*/
const std::map<EntityType, const EntityData::EntityFieldDescriptions> entityTypeDescriptions = {
	{
		EntityType::TITLE, {				// entity type
			{"width",  EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0)},								
			{"height", EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT, EntityData::EntityDefaultValue(0) }
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
			{ "name",       EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT,  EntityData::EntityDefaultValue("") },
			{ "direction",  EntityData::EntityValueFlag::VALUE_FLAG_NO_DEFAULT,  EntityData::EntityDefaultValue("")}
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
	EntityData entityData;


}

bool EntityData::ImportFromLua(lua_State * l)
{
	return false;
}
