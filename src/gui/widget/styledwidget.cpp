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
*	\brief ���ó�Աֵ
*	\param data string��Ϊk,v��map	
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
*	\brief ������ٵ�ǰ���
*
*	������ٵ�ǰ��ȣ���Ϊ������ʽ�������Ի���ڼ���ʧ�ܵ����
*	����styledWidget,��Ҫ����lable�Ĵ�������������Ƿ������������
*	����
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
*	\brief ������ٵ�ǰ�߶�
*
*	������ٵ�ǰ�߶ȣ���Ϊ������ʽ�������Ի���ڼ���ʧ�ܵ����
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
*	\brief �������ֵ���Ѵ�С
*
*	��Ҫ���ǵ����ֵĴ�С����ʽ�����뷽ʽ
*/
Size StyledWidget::GetBestTextSize(const Size& minSize, const Size& maxSize)const
{
	Size size = minSize;

	// �����С

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
*	\brief ������Ѵ�С
*	
*	Ŀǰ��Ѵ�С��lable��С����
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
*	\brief ��������
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
*	Ԫ������Ⱦ������Ҫһ���ܹ�����˳��ķ�ʽ
*	Ŀǰ�������һ����Ⱦ���У������е���Ⱦ����
*	�����ڶ�����
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
