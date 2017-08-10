#pragma once

#include"common\common.h"
#include"game\sprite.h"
#include"utils\point.h"
#include"gui\core\dispatcher.h"

namespace gui
{

class Window;
class Grid;

/**
*	\brief widget基类
*/
class Widget : public Dispatcher
{
	/*** *** *** *** *** property. *** *** *** *** ***/
public:
	/** 可见性动作 */
	enum class Visiblility
	{
		Visible,
		Hidden,
		InVisible
	};

	/** 重绘动作 */
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

	void DrawBackground(const Point2& offset);
	void DrawForeground(const Point2& offset);
	void DrawChildren(const Point2& offset);
	void DrawDebugGround();

private:
	virtual void ImplDrawBackground();
	virtual void ImplDrawForeground();
	virtual void ImplDrawChildren();

public:
	/******  setter/ getter *******/

	void SetIsDirty(bool isDirty);
	bool IsDirty()const;
	const Rect GetDirtyRect()const;

	Visiblility GetVisibility()const;
	void SetVisibility(const Visiblility& visibility);

	void SetParent(Widget* parent);
	Widget* GetParent();

	Window* GetWindow();
	const Window* GetWindow()const;

	void SetLinkGrounp(const std::string& linkedGroup);
private:
	string mID;

	//std::weak_ptr<Widget> mParent;
	Widget* mParent;

	bool mIsDirty;			// 目前每帧重绘的情况下，dirty的存在暂定
	Visiblility mVisible;
	ReDrawAction mReDrawAction;

	std::string mLinkedGroup;

	/*** *** *** *** *** layout and size. *** *** *** *** ***/
public:
	const Point2& GetPositon()const;
	virtual void SetPosition(const Point2& position);
	const Size& GetSize()const;
	virtual void SetSize(const Size& size);

	int GetWidth()const;
	int GetHeight()const;

	virtual void Place(const Point2& pos, const Size& size);

	virtual void Move(const Point2& offset);
	virtual void Move(const int xoffset, const int yoffset);

	virtual void InitLayout();
	virtual Size GetBestSize()const;

	virtual Size CalculateBestSize()const;

	virtual void SetHorizontalAlignment();
	virtual void SetVerticalAlignment();

private:
	Point2 mPosition;
	Size mSize;

public:
	virtual Widget* Find(string& id, const bool activited);
	virtual const Widget* Find(string& id, const bool activied)const;
	virtual bool HasWidget(const Widget& widget)const;
	virtual bool IsAt(const Point2& pos)const;


};

using WidgetPtr = std::shared_ptr<Widget>;
using WidgetConstPtr = std::shared_ptr<const Widget>;

}