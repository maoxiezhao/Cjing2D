#pragma once

#include"common\common.h"
#include"gui\widget\widget.h"
#include"gui\core\builderWindow.h"
#include"gui\core\widgetDefinition.h"

/**
*	\brief 
*/
namespace gui
{
class Config;
class Font;

namespace implementation
{
	class BuilderStyledWidget;
}

using StringMap = std::map<string, string>;

/**
*	\brief styledWidget 
*	包含了更多样式信息包括lable信息和定义信息
*/
class StyledWidget : public Widget
{
public:
	StyledWidget();
	StyledWidget(const implementation::BuilderStyledWidget& builder);
	virtual~StyledWidget();

	void SetMembers(const StringMap& data);
	virtual void SetActivite(const bool activite)const = 0;
	virtual bool GetActivite()const;
	virtual unsigned int GetState()const = 0;

	Size GetConfigMinSize()const;
	Size GetConfigMaxSize()const;
	Size GetConfigDefaultSize()const;

	void SetLabel(const string& label);
	string GetLable()const;

	void SetTextAlignment(const string& textAlignment);
	string GetTextAlignment()const;

	ResolutionDefinitionPtr GetConfig();

/*** *** *** *** *** layout and size. *** *** *** *** ***/

	virtual Size GetBestTextSize(const Size& minSize, const Size& maxSize)const;
	virtual Size CalculateBestSize()const;

	virtual void Place(const Point2& pos, const Size& size);

	virtual void RequestReduceWidth(const int maxnumWidth);
	virtual void DemandReduceWidth(const int maxnumWidth);

	virtual void RequestReduceHeight(const int maxnumHeight);
	virtual void DemandReduceHeight(const int maxnumHeight);

public:
	virtual Widget* Find(string& id, const bool activited);
	virtual const Widget* Find(string& id, const bool activied)const;
	virtual bool HasWidget(const Widget& widget)const;

private:
	void LoadConfig();

	std::string mDefinition;		/* widget class id */

	std::string mLabel;

	ResolutionDefinitionPtr mConfig;

protected:
	virtual void LoadConfigEx(){};
	virtual void LoadDefinitionConfig(const string& controlType);

	virtual void ImplDrawBackground();
	virtual void ImplDrawForeground();
	virtual void ImplDrawChildren();

	/*** *** Font *** ***/
	std::shared_ptr<Font> mFont;
	int mTextMaxWidth;
	string mTextAlignment;

};

namespace implementation
{

class BuilderStyledWidget : public BuilderWidget
{
public:
	BuilderStyledWidget(const Config& config);

	void InitControl(StyledWidget* styledWidget)const;

	virtual Widget* Build()const;

	std::string mDefinition;
	std::string mLabel;
};
}

}