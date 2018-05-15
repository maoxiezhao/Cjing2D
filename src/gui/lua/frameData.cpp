#include "frameData.h"
#include "gui\widget\multGrid.h"

static std::map<std::string, int> WidgetAlignMapping = {
	{ "top",     gui::ALIGN_VERTICAL_TOP },
	{ "center",  gui::ALIGN_VERTICAL_CENTER },
	{ "bottom",  gui::ALIGN_VERTICAL_BOTTOM },
	{ "left",    gui::ALIGN_VERTICAL_TOP },
	{ "right",   gui::ALIGN_VERTICAL_BOTTOM },
};

int FrameData::mIndex = 0;

const std::map<gui::WIDGET_TYPE, const FrameData::FieldDescriptions> frameTypeDescriptions = {
	{ gui::WIDGET_TYPE::WIDGET_FRAME, {}	},
	{ gui::WIDGET_TYPE::WIDGET_BUTTON,{
		{ "label",  FrameData::ValueFlag::VALUE_FALG_DEFAULT, FrameData::DefaultValue(std::string("")) }
	}},
	{ gui::WIDGET_TYPE::WIDGET_IMAGE,{
		{ "path",   FrameData::ValueFlag::VALUE_FALG_DEFAULT, FrameData::DefaultValue(std::string("")) }
	} },
};

FrameData::DefaultValue::DefaultValue() :
	mType(VALUE_TYPE_STRING),
	mIntValue(0),
	mStringValue("")
{
}
FrameData::DefaultValue::DefaultValue(int value) :
	mType(VALUE_TYPE_INTEGER),
	mIntValue(value),
	mStringValue("")
{
}
FrameData::DefaultValue::DefaultValue(const string & value) :
	mType(VALUE_TYPE_STRING),
	mIntValue(0),
	mStringValue(value)
{
}
FrameData::DefaultValue::DefaultValue(bool value) :
	mType(VALUE_TYPE_BOOLEAN),
	mIntValue(static_cast<int>(value)),
	mStringValue("")
{
}

FrameData::FrameData():
	mType(gui::WIDGET_TYPE::WIDGET_UNKNOW)
{
}

FrameData FrameData::CheckFrameData(lua_State * l, int index, gui::WIDGET_TYPE type)
{
	LuaTools::CheckType(l, index, LUA_TTABLE);

	/** common property */
	const std::string name = LuaTools::CheckFieldStringByDefault(l, index, "name", 
		std::to_string(FrameData::mIndex++));
	Point2 pos = LuaTools::CheckFieldPoint2ByDefault(l, index, "pos", Point2());
	Point2 gridPos = LuaTools::CheckFieldPoint2ByDefault(l, index, "grid_pos", Point2());
	Point2 size = LuaTools::CheckFieldPoint2ByDefault(l, index, "size", Point2());

	const std::string& verticalAlign = LuaTools::CheckFieldStringByDefault(l, index, "vertical", "top");
	const std::string& horizontalAlign = LuaTools::CheckFieldStringByDefault(l, index, "horizontal", "left");
	int flag = WidgetAlignMapping[verticalAlign];
	flag |= WidgetAlignMapping[horizontalAlign] << gui::ALIGN_HORIZONTAL_SHIFT;

	FrameData frameData;
	frameData.SetType(type);
	frameData.SetPos(pos);
	frameData.SetSize({ size.x, size.y });
	frameData.SetAlignFlag(flag);

	/** special property */
	auto& frameFieldDescriptions = frameTypeDescriptions.at(type);
	for (const auto& frameDesc : frameFieldDescriptions)
	{
		const std::string& key = frameDesc._key;
		auto flag = frameDesc._flag;
		auto defaultValue = frameDesc._default;

		string tmpStrValue;
		int tmpIntValue = 0;
		switch (defaultValue.mType)
		{
		case ValueType::VALUE_TYPE_STRING:
			if (flag == ValueFlag::VALUE_FALG_DEFAULT)
				tmpStrValue = LuaTools::CheckFieldStringByDefault(l, index, key, defaultValue.mStringValue);
			else
				tmpStrValue = LuaTools::CheckFieldString(l, index, key);

			frameData.SetValueString(key, tmpStrValue);
			break;
		case ValueType::VALUE_TYPE_BOOLEAN:
			if (flag == ValueFlag::VALUE_FALG_DEFAULT)
				tmpIntValue = static_cast<int>(LuaTools::CheckFieldBoolByDefault(l, index, key, static_cast<bool>(defaultValue.mIntValue)));
			else
				tmpIntValue = static_cast<int>(LuaTools::CheckFieldBool(l, index, key));

			frameData.SetValueBoolean(key, tmpIntValue);
			break;
		case ValueType::VALUE_TYPE_INTEGER:
			if (flag == ValueFlag::VALUE_FALG_DEFAULT)
				tmpIntValue = LuaTools::CheckFieldIntByDefault(l, index, key, defaultValue.mIntValue);
			else
				tmpIntValue = LuaTools::CheckFieldInt(l, index, key);

			frameData.SetValueInteger(key, tmpIntValue);
		}
	}
	return frameData;
}

bool FrameData::ImportFromLua(lua_State * l)
{
	return false;
}


void FrameData::SetValueBoolean(const string & key, bool value)
{
	FieldDescription fieldDescription;
	fieldDescription._key = key;
	fieldDescription._flag = ValueFlag::VALUE_FALG_DEFAULT;
	fieldDescription._default = DefaultValue(value);
	mValueField[key] = fieldDescription;
}

void FrameData::SetValueString(const string & key, const string & value)
{
	FieldDescription entityFieldDescription;
	entityFieldDescription._key = key;
	entityFieldDescription._flag = ValueFlag::VALUE_FALG_DEFAULT;
	entityFieldDescription._default = DefaultValue(value);
	mValueField[key] = entityFieldDescription;
}

void FrameData::SetValueInteger(const string & key, int value)
{
	FieldDescription entityFieldDescription;
	entityFieldDescription._key = key;
	entityFieldDescription._flag = ValueFlag::VALUE_FALG_DEFAULT;
	entityFieldDescription._default = DefaultValue(value);
	mValueField[key] = entityFieldDescription;
}

const FrameData::FieldDescription & FrameData::GetValue(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");

	return it->second;
}

bool FrameData::GetValueBoolean(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");
	Debug::CheckAssertion(it->second._default.mType == ValueType::VALUE_TYPE_BOOLEAN,
		"Excepted boolean value.");

	return static_cast<bool>(it->second._default.mIntValue);
}

int FrameData::GetValueInteger(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");
	Debug::CheckAssertion(it->second._default.mType == ValueType::VALUE_TYPE_INTEGER,
		"Excepted boolean value.");

	return it->second._default.mIntValue;
}

const string & FrameData::GetValueString(const string & key) const
{
	auto& it = mValueField.find(key);
	Debug::CheckAssertion(it != mValueField.end(),
		"Tried to get the not exists key in entity valueField.");
	Debug::CheckAssertion(it->second._default.mType == ValueType::VALUE_TYPE_STRING,
		"Excepted boolean value.");

	return it->second._default.mStringValue;
}