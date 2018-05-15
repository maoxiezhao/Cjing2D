#include "gui\widget\image.h"
#include "gui\lua\uiApi.h"
#include "gui\core\uiRender.h"

namespace gui {

Image::Image():
	StyledWidget()
{
}

Image::Image(const std::string & path):
	StyledWidget(),
	mImagePath(path)
{
	Connect();
	SetImagePath(path);
}

/**
*	\brief 设置当前图片路径
*	\param path 图片路径，如果路径为空则不显示
*/
void Image::SetImagePath(const std::string & path)
{
	mImagePath = path;
	if (path != "")
		UIRender::LoadUIImage(GetImagePath());
}

std::string Image::GetImagePath() const
{
	return mImagePath;
}

void Image::SetImageGrid(float left, float top, float right, float bottom)
{
	mGrid.left   = (int)left;
	mGrid.top    = (int)top;
	mGrid.right  = (int)right;
	mGrid.bottom = (int)bottom;
}

ImageGrid Image::GetImageGrid() const
{
	return mGrid;
}

Size Image::GetImageSize() const
{
	return UIRender::GetImageSize(GetImagePath());
}

void Image::ImplDrawBackground(const Point2 & offset)
{
	ImageRenderInfo renderInfo;
	renderInfo.imageGrid = GetImageGrid();
	renderInfo.imageSize = GetImageSize();
	renderInfo.imageRect = { GetPositon(), GetSize() };
	renderInfo.imagePath = GetImagePath();

	UIRender::RenderImage(renderInfo);
}

const string Image::GetLuaObjectName() const
{
	return gui::ui_lua_image_name;
}

///**
//*	\brief 注册事件信号回调函数
//*/
//void Image::RegisterSignalCallBack()
//{
//	ConnectSignal<ui_event::UI_EVENT_MOUSE_ENTER>(
//		std::bind(&Image::SignalHandlerMouseEnter, this, std::placeholders::_2, std::placeholders::_3));
//	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEAVE>(
//		std::bind(&Image::SignalHandlerMouseLeave, this, std::placeholders::_2, std::placeholders::_3));
//	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_DOWN>(
//		std::bind(&Image::SignalHandlerMouseLeftButtonDown, this, std::placeholders::_2, std::placeholders::_3));
//	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_UP>(
//		std::bind(&Image::SignalHandlerMouseLeftButtonUp, this, std::placeholders::_2, std::placeholders::_3));
//	ConnectSignal<ui_event::UI_EVENT_MOUSE_LEFT_BUTTON_CLICK>(
//		std::bind(&Image::SignalHandlerMouseLeftButtonClick, this, std::placeholders::_2, std::placeholders::_3));
//}


}
