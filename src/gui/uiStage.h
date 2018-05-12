#pragma once

#include"gui\core\handler.h"
#include"gui\widget\window.h"

/**
*	\brief ui场景管理类
*
*	 uistage负责维护整个GUI的管理实例，维护一个root，
*	并提供创建删除管理各个gui实例的接口
*/

class UIStage
{
public:
	/** 创建Widget时的信息 */
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
	std::unique_ptr<gui::GUIManager> mGUI;	// gui管理器
	std::shared_ptr<gui::Window> mRoot;		// widget根节点,所有widget创建作为mRoot的children

	using WidgetCreateFunc = std::function<gui::WidgetPtr(WidgetData) >;
	static std::map<gui::WIDGET_TYPE, WidgetCreateFunc> mCreateFuncMapping;
};