#pragma once

#include"common\common.h"
#include"lua\luaData.h"
#include"utils\point.h"
#include"utils\size.h"
#include"gui\widget\widgetInfo.h"

/**
*	\brief Frame数据
*
*	TODO 功能和EntityData相同，应当提取优化
*/
class FrameData : public LuaData
{
public:
	FrameData();
	enum ValueFlag
	{
		VALUE_FLAG_NO_DEFAULT,
		VALUE_FALG_DEFAULT
	};

	enum ValueType
	{
		VALUE_TYPE_BOOLEAN,
		VALUE_TYPE_STRING,
		VALUE_TYPE_INTEGER
	};
	struct DefaultValue
	{
		DefaultValue();
		DefaultValue(int value);
		DefaultValue(const string& value);
		DefaultValue(bool value);

		ValueType mType;
		int mIntValue;				// 数值或者boolean
		string mStringValue;
	};
	struct FieldDescription
	{
		std::string _key;			// 类型描述的值的名字
		ValueFlag _flag;
		DefaultValue _default;		// 类型描述的值的默认值，由optionFlag决定是否使用
	};
	using FieldDescriptions = std::vector<FieldDescription>;

	/**** **** value set **** ****/
	void SetValueBoolean(const string& key, bool value);
	void SetValueString(const string& key, const string& value);
	void SetValueInteger(const string& key, int value);

	const FieldDescription& GetValue(const string& key)const;
	bool GetValueBoolean(const string& key)const;
	int GetValueInteger(const string& key)const;
	const string& GetValueString(const string& key)const;

public:
	static const std::map<gui::WIDGET_TYPE, const FieldDescriptions>& GetFieldDescriptions();
	static FrameData CheckFrameData(lua_State*l, int index, gui::WIDGET_TYPE type);

	virtual bool ImportFromLua(lua_State*l);

	SetterGetterMethod(gui::WIDGET_TYPE, Type);
	SetterGetterMethod(std::string, Name)
	SetterGetterMethod(Point2, Pos)
	SetterGetterMethod(Point2, GridPos)
	SetterGetterMethod(Size, Size)
	SetterGetterMethod(int, AlignFlag)

private:
	gui::WIDGET_TYPE mType;
	string mName;
	Point2 mPos;
	Point2 mGridPos;
	Size mSize;
	int mAlignFlag;

	static int mIndex;
	std::map<std::string, FieldDescription> mValueField;	// 变量域
};