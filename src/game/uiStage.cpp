#include"uiStage.h"
#include"core\logger.h"
#include"gui\core\uiRender.h" 
#include"gui\widget\selections.h"
#include"gui\widget\selections_private.h"
#include"gui\widget\toggleButton.h"

UIStage::UIStage()
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

UIStage::~UIStage()
{
}

void UIStage::Initiazlize()
{
	Logger::Info("[GUI] initialize ui Render");
	gui::UIRender::Initiazlize();

	mGUI = std::unique_ptr<gui::GUIManager>(new gui::GUIManager());
}

void UIStage::Quit()
{
	Logger::Info("[GUI] quit ui Render");
	gui::UIRender::Quit();
}

bool UIStage::NotifyInput(const InputEvent & ent)
{
	if (mGUI != nullptr)
	{
		mGUI->HandleEvent(ent);
	}
	return false;
}

void UIStage::Draw()
{

}

void UIStage::Update()
{
}
