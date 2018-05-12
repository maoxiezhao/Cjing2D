#include"uiStage.h"
#include"core\logger.h"
#include"core\video.h"
#include"gui\core\uiRender.h" 
#include"gui\widget\selections.h"
#include"gui\widget\selections_private.h"
#include"gui\widget\toggleButton.h"
#include"gui\widget\button.h"

void TestStatement()
{
	//mGUI = std::unique_ptr<gui::GUIManager>(new gui::GUIManager());

	//auto selections = std::make_shared<gui::Selections>(
	//	new gui::VerticalList,
	//	new gui::Selected,
	//	new gui::MaxmumOneItem,
	//	new gui::MinmumOneItem);

	//selections->Connect();
	//selections->Place(Point2(100, 0), Size(150, 100));

	//for (int i = 0; i < 6; i++)
	//{
	//	auto button1 = std::make_shared<gui::ToggleButton>();
	//	button1->Connect();
	//	button1->Place(Point2(0, 0), Size(150, 50));

	//	selections->AddItem(button1);
	//}

	//mWindow = std::make_shared<gui::Window>(450, 30, 150, 400);
	//mWindow->AddCols(1);
	//mWindow->Show(false);
	//mWindow->SetChildren(selections, 0, 0, gui::ALIGN_VERTICAL_TOP | gui::ALIGN_HORIZONTAL_BOTTOM, 0);
	//mWindow->AddToKeyboardFocusChain(selections.get());
}


std::map<gui::WIDGET_TYPE, UIStage::WidgetCreateFunc> UIStage::mCreateFuncMapping =
{
	{
		gui::WIDGET_TYPE::WIDGET_WINDOW,		
		[](WidgetData data)->gui::WidgetPtr {
			return nullptr;
		}
	}
};

UIStage::UIStage():
	mGUI(nullptr),
	mRoot(nullptr)
{
}

UIStage::~UIStage()
{
}

void UIStage::Initiazlize()
{
	Logger::Info("[GUI] initialize ui Render");
	gui::UIRender::Initiazlize();

	/** 创建GUI管理器 */
	mGUI = std::unique_ptr<gui::GUIManager>(new gui::GUIManager());

	/** 创建根节点Root */
	const Size& wantedSize = Video::GetScreenSize();
	mRoot = std::make_shared<gui::Window>(0, 0, wantedSize.width, wantedSize.height);
	mRoot->AddCols(1);
	mRoot->AddRows(1);
	mRoot->RefreshPlace();

	/** Test */
	auto button = std::make_shared<gui::Button>();
	button->Connect();
	button->SetWantedPosition({ 0, 50 });
	button->SetSize({150, 50});
	mRoot->SetChildren(button, 1, 1, gui::ALIGN_VERTICAL_TOP| gui::ALIGN_HORIZONTAL_LEFT, 0);
}

void UIStage::Quit()
{
	Logger::Info("[GUI] quit ui Render");
	gui::UIRender::Quit();
}

bool UIStage::NotifyInput(const InputEvent & ent)
{
	bool handle = false;
	if (mGUI != nullptr)
		mGUI->HandleEvent(ent);

	return handle;
}

void UIStage::Draw()
{
	if (mRoot != nullptr)
		mRoot->Draw();

	gui::UIRender::RenderDebugDemo();
}

void UIStage::Update()
{
}

void UIStage::SetVisible(bool visible)
{
	auto visiblity = visible ? gui::Widget::Visiblility::Visible :
		gui::Widget::Visiblility::Hidden;
	mRoot->SetVisibility(visiblity);
}

bool UIStage::IsVisible() const
{
	return mRoot->GetVisibility() == 
		gui::Widget::Visiblility::Visible;
}

/**
*	\brief 创建一个新的Element
*	\param widgetType element的类型
*	\param data 创建的elemnet的基本信息
*	\param parent 创建该Element的父widget
*/
bool UIStage::CreateElement(gui::WIDGET_TYPE widgetType, UIStage::WidgetData data, gui::WidgetPtr parent)
{

	return true;
}
