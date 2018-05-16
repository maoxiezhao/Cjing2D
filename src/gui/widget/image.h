#pragma once

#include"gui\widget\styledwidget.h"

namespace gui
{
/**
*	\brief ͼƬ��������ʱ��ͼƬԭ��С����
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
*	\brief ��Ⱦ�����Ϣ
*/
struct ImageRenderInfo
{
	Size imageSize;			/** ͼƬԭʼ��С */
	Rect imageRect;			/** Widget��Ϣ */
	ImageGrid imageGrid;	/** ͼƬ�Ź��� */
	std::string imagePath;  /** ͼƬ·�� */
};

/**
*	\brief widget Image
*
*	Ŀǰ��Ȼʹ�õ���Nanovg��Ⱦ�⣬δ������һ��ֱ������ʹ��Sprite
*	�������޸�Nanovg����Ⱦ����
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