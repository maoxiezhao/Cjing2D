#pragma once

#include"common\common.h"
#include"game\sprite.h"
#include"utils\point.h"
#include"gui\core\dispatcher.h"

namespace gui
{

/**
*	\brief widget»ùÀà
*/
class Widget : public Dispatcher
{
	/*** *** *** *** *** property. *** *** *** *** ***/
public:
	enum class Visiblility
	{
		Visible,
		Hidden,
		InVisible
	};
	enum class ReDrawAction
	{
		Full,
		None
	};

public:
	Widget();
	virtual ~Widget();

	Widget(const Widget&) = delete;
	Widget& operator=(const Widget&) = delete;

	void SetID(const string& id);
	const string& GetID()const;

	void DrawBackground();
	void DrawForeground();
	void DrawChildren();

	/******  setter/ getter *******/

	void SetIsDirty(bool isDirty);
	bool IsDirty()const;
	const Rect GetDirtyRect()const;

	Visiblility GetVisibility()const;
	void SetVisibility(const Visiblility& visibility);

	Widget* GetParent();

private:
	string mID;

	//std::weak_ptr<Widget> mParent;
	Widget* mParent;

	bool mIsDirty;
	Visiblility mVisible;
	ReDrawAction mReDrawAction;

	/*** *** *** *** *** layout and size. *** *** *** *** ***/
public:
	const Point2& GetPositon()const;
	void SetPosition(const Point2& position);
	const Size& GetSize()const;
	void SetSize(const Size& size);
	int GetWidth()const;
	int GetHeight()const;

	virtual void Move(const Point2& offset);
	virtual void Move(const int xoffset, const int yoffset);

	virtual void InitLayout();
	virtual Size GetBestSize()const;

	virtual void SetHorizontalAlignment();
	virtual void SetVerticalAlignment();

private:
	Point2 mPosition;

	Size mSize;

};

}