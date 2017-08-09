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

namespace implementation
{
	class BuilderStyledWidget;
}

using StringMap = std::map<string, string>;

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

	ResolutionDefinitionPtr GetConfig();

/*** *** *** *** *** layout and size. *** *** *** *** ***/

	virtual void InitLayout();
	virtual Size GetBestSize()const;
	virtual void Place(const Point2& pos, const Size& size);

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