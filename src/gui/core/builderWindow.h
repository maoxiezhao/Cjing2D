#pragma once

#include"common\common.h"
#include<map>

/**
*	\brief 创建window/widge辅助库
*/
namespace gui
{
class Config;
class Widget;

/**
*	\brief 创建widget工厂类的基类
*
*	具体创建各个widget由派生类具体实现
*/
class BuilderWidget
{
	using replacementMap = std::map<string, std::shared_ptr<BuilderWidget> >;
public:
	BuilderWidget(const Config& config);
	virtual ~BuilderWidget();

	virtual Widget* Build()const = 0;

	string mID;
	string mLinkedGroup;
};

using BuilderWidgetPtr = std::shared_ptr<BuilderWidget>;

}