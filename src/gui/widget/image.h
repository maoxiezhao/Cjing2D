#pragma once

#include"gui\widget\styledwidget.h"

namespace gui
{
/**
*	\brief 图片网格，作用时按图片原大小计算
*/
struct ImageGrid
{
	int left;
	int top;
	int right;
	int bottom;

	ImageGrid() :left(0), top(0), right(0), bottom(0) {}
};

/**
*	\brief 渲染相关信息
*/
struct ImageRenderInfo
{
	Size imageSize;			/** 图片原始大小 */
	Rect imageRect;			/** Widget信息 */
	ImageGrid imageGrid;	/** 图片九宫格 */
	std::string imagePath;  /** 图片路径 */
};

/**
*	\brief widget Image
*
*	目前依然使用的是Nanovg渲染库，未来考虑一是直接这里使用Sprite
*	或者是修改Nanovg的渲染方法
*/

class Image : public StyledWidget
{
public:
	Image();
	Image(const std::string& path);

	void SetImagePath(const std::string& path);
	std::string GetImagePath()const;
	void SetImageGrid(float left, float top, float right, float bottom);
	ImageGrid GetImageGrid()const;
	Size GetImageSize()const;

	virtual void ImplDrawBackground(const Point2& offset);

	virtual WIDGET_TYPE GetWidgetType()const
	{
		return WIDGET_TYPE::WIDGET_IMAGE;
	}

	virtual const string GetLuaObjectName()const;
private:
	std::string mImagePath;
	ImageGrid mGrid;

};
}