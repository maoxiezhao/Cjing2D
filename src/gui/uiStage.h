#pragma once

#include"gui\core\handler.h"
#include"gui\widget\window.h"

/**
*	\brief ui����������
*
*	 uistage����ά������GUI�Ĺ���ʵ����ά��һ��root��
*	���ṩ����ɾ���������guiʵ���Ľӿ�
*/

class UIStage
{
public:
	/** ����Widgetʱ����Ϣ */
	struct WidgetData
	{
		Point2 wantedPos;
		Size wantedSize;
	};

	UIStage();
	~UIStage();

	void Initiazlize();
	void Quit();
	bool NotifyInput(const InputEvent& ent);
	void Draw();
	void Update();

	/** widget operation */
	bool CreateElement(gui::WIDGET_TYPE widgetType, WidgetData data, gui::WidgetPtr parent = nullptr);

	/** status */
	bool IsMouseOnGUI();
	bool IsKeyboardOnGUI();
	
	void SetVisible(bool visible);
	bool IsVisible()const;

private:
	std::unique_ptr<gui::GUIManager> mGUI;	// gui������
	std::shared_ptr<gui::Window> mRoot;		// widget���ڵ�,����widget������ΪmRoot��children

	using WidgetCreateFunc = std::function<gui::WidgetPtr(WidgetData) >;
	static std::map<gui::WIDGET_TYPE, WidgetCreateFunc> mCreateFuncMapping;
};