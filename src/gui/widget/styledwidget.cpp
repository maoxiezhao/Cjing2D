#include"styledwidget.h"
#include"core\debug.h"
#include"core\font.h"

namespace gui
{
StyledWidget::StyledWidget():
	mDefinition("defualt"),
	mLabel(""),
	mConfig(nullptr),
	mFont(nullptr),
	mTextMaxWidth(0),
	mTextAlignment("")
{
}

StyledWidget::StyledWidget(const implementation::BuilderStyledWidget& builder):
	Widget(builder),
	mDefinition(builder.mDefinition),
	mLabel(builder.mLabel),
	mConfig(nullptr),
	mFont(nullptr),
	mTextMaxWidth(0),
	mTextAlignment("")
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

	itor = data.find("text_alignment");
	if (itor != data.end())
	{
		SetTextAlignment(itor->second);
	}
}

void StyledWidget::SetActivite(const bool activite) const
{
	// do nothing
}

bool StyledWidget::GetActivite() const
{
	return false;
}

unsigned int StyledWidget::GetState() const
{
	return 0;
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
	if (mLabel == label)
	{
		return;
	}
	mLabel = label;
}

string StyledWidget::GetLable()const
{
	return mLabel;
}

void StyledWidget::SetTextAlignment(const string& textAlignment)
{
	mTextAlignment = textAlignment;
}

string StyledWidget::GetTextAlignment()const
{
	return mTextAlignment;
}

ResolutionDefinitionPtr StyledWidget::GetConfig()
{
	return mConfig;
}

void StyledWidget::Place(const Point2& pos, const Size& size)
{
	Widget::Place(pos, size);
}

/**
*	\brief 请求减少当前宽度
*
*	请求减少当前宽度，因为以请求方式处理，所以会存在减少失败的情况
*	对于styledWidget,主要考虑lable的处理情况，包括是否可以缩减或者
*	换行
*/
void StyledWidget::RequestReduceWidth(const int maxnumWidth)
{
	if (!mLabel.empty() && CanWrap())
	{
		Size size = GetBestTextSize(Size(), Size(maxnumWidth, 0));
		SetLayoutSize(size);
	}
	else if (mLabel.empty())
	{
		Size bestSize = GetBestSize();
		Size maxnumSize = GetConfigMaxSize();
		bestSize.width = std::min(bestSize.width, maxnumSize.width);

		SetLayoutSize(bestSize);
	}
	else
	{
		Debug::Warning("The styledWidget '" + GetID() + "' failed to reduce widget.");
	}
}

void StyledWidget::DemandReduceWidth(const int maxnumWidth)
{
}

/**
*	\brief 请求减少当前高度
*
*	请求减少当前高度，因为以请求方式处理，所以会存在减少失败的情况
*/
void StyledWidget::RequestReduceHeight(const int maxnumHeight)
{
	if (!mLabel.empty())
	{
		Debug::Warning("The styledWidget '" + GetID() + "' failed to reduce height.");
		return;
	}

	Size bestSize = GetBestSize();
	Size maxnumSize = GetConfigMaxSize();
	bestSize.height = std::min(bestSize.height, maxnumSize.height);

	SetLayoutSize(bestSize);
}

void StyledWidget::DemandReduceHeight(const int maxnumHeight)
{
}

/**
*	\brief 计算文字的最佳大小
*
*	需要考虑到文字的大小，样式，对齐方式
*/
Size StyledWidget::GetBestTextSize(const Size& minSize, const Size& maxSize)const
{
	Size size = minSize;

	// 处理大小

	if (size.width < minSize.width)
	{
		size.width = minSize.width;
	}
	if (size.height < minSize.height)
	{
		size.height = minSize.height;
	}
	return size;
}

/**
*	\brief 计算最佳大小
*	
*	目前最佳大小由lable大小决定
*/
Size StyledWidget::CalculateBestSize()const
{
	return Widget::CalculateBestSize();

	const Size minSize = GetConfigMinSize();
	const Size maxSize = GetConfigMaxSize();

	return GetBestTextSize(minSize, maxSize);
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
	return Widget::HasWidget(widget);
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

/**
*	元件的渲染处理，需要一种能够控制顺序的方式
*	目前考虑设计一个渲染队列，将所有的渲染操作
*	保存在队列中
*/
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
