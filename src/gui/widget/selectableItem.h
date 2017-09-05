#pragma once

namespace gui
{
class Widget;

/**
*	\breif 可选择项widget的抽象基类,
*	提供设置是否选择状态的抽象接口
*/
class SelectableItem
{
public:
	virtual void SetSelected(bool selected) = 0;
	virtual bool IsSelected()const = 0;

};

}