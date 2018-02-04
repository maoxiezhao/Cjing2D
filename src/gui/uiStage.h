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
	UIStage();
	~UIStage();

	void Initiazlize();
	void Quit();
	bool NotifyInput(const InputEvent& ent);
	void Draw();
	void Update();

	/*** widget base operation */
	bool CreateElement();

private:
	std::unique_ptr<gui::GUIManager> mGUI;	// gui������

	std::shared_ptr<gui::Widget> mRoot;		// widget���ڵ�,����widget������ΪmRoot��children

};