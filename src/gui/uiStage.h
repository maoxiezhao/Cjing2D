#pragma once

#include"gui\core\handler.h"
#include"gui\widget\frame.h"

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

	~UIStage();
	static UIStage& GetInstance();

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

	gui::Frame& GetRoot();

private:
	UIStage();

private:
	std::unique_ptr<gui::GUIManager> mGUI;	// gui������
	std::shared_ptr<gui::Frame> mRoot;		// widget���ڵ�,����widget������ΪmRoot��children
	std::shared_ptr<gui::Distributor> mDistributor;

	using WidgetCreateFunc = std::function<gui::WidgetPtr(WidgetData) >;
	static std::map<gui::WIDGET_TYPE, WidgetCreateFunc> mCreateFuncMapping;
};