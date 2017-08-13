#include"styledwidget.h"
#include"core\debug.h"

namespace gui
{
StyledWidget::StyledWidget():
	mDefinition("defualt"),
	mLabel(""),
	mConfig(nullptr)
{
}

StyledWidget::StyledWidget(const implementation::BuilderStyledWidget& builder):
	Widget(builder),
	mDefinition(builder.mDefinition),
	mLabel(builder.mLabel),
	mConfig(nullptr)
{
}

StyledWidget::~StyledWidget()
{
}

/**
*	\brief 设置成员值
*	\param data string作为k,v的map	
*/
void StyledWidget::SetMembers(const StringMap& data)
{
	auto itor = data.find("id");
	if (itor != data.end())
	{
		SetID(itor->second);
	}

	itor = data.find("linked_group");
	if (itor != data.end())
	{
		SetLinkGrounp(itor->second);
	}

	itor = data.find("lable");
	if (itor != data.end())
	{
		SetLabel(itor->second);
	}
}

bool StyledWidget::GetActivite() const
{
	return false;
}

Size StyledWidget::GetConfigMinSize()const
{
	Debug::CheckAssertion(mConfig != nullptr);

	return Size(mConfig->minWidth, mConfig->minHeight);
}

Size StyledWidget::GetConfigMaxSize()const
{
	Debug::CheckAssertion(mConfig != nullptr);

	return Size(mConfig->maxWidth, mConfig->maxHeight);
}

Size StyledWidget::GetConfigDefaultSize()const
{
	Debug::CheckAssertion(mConfig != nullptr);

	return Size(mConfig->defaultWidth, mConfig->defaultHeight);
}

void StyledWidget::SetLabel(const string& label)
{
	mLabel = label;
}

string StyledWidget::GetLable()const
{
	return mLabel;
}

ResolutionDefinitionPtr StyledWidget::GetConfig()
{
	return mConfig;
}

void StyledWidget::InitLayout()
{
}

Size StyledWidget::GetBestSize() const
{
	return Size();
}

void StyledWidget::Place(const Point2& pos, const Size& size)
{
	Widget::Place(pos, size);
}

Widget * StyledWidget::Find(string & id, const bool activited)
{
	return nullptr;
}

const Widget * StyledWidget::Find(string & id, const bool activied) const
{
	return nullptr;
}

bool StyledWidget::HasWidget(const Widget & widget) const
{
	return false;
}

/**
*	\brief 加载配置
*/
void StyledWidget::LoadConfig()
{
	if (GetConfig() != nullptr)
	{
		//LoadDefinitionConfig();
		LoadConfigEx();
	}
}

void StyledWidget::LoadDefinitionConfig(const string& controlType)
{
}

void StyledWidget::ImplDrawBackground()
{
}

void StyledWidget::ImplDrawForeground()
{
}

void StyledWidget::ImplDrawChildren()
{
}

implementation::BuilderStyledWidget::BuilderStyledWidget(const Config & config):
	BuilderWidget(config)
{
}

void implementation::BuilderStyledWidget::InitControl(StyledWidget * styledWidget) const
{
}

Widget * gui::implementation::BuilderStyledWidget::Build() const
{
	return nullptr;
}

}
