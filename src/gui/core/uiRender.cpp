#include"gui\core\uiRender.h"
#include"utils\nanovg.h"
#include"utils\nanovg_gl.h"
#include"core\logger.h"

// debug
#include"main\demo.h"
#include"core\video.h"

namespace gui
{
namespace
{
	NVGcontext* vg = nullptr;
	DemoData data;
	PerfGraph fps;

	struct DebugRect { Rect rect; Color4B color; };
	std::vector<DebugRect> mRenderDebugRect;	/** 绘制的rect,当前临时这么做 */
	struct DebugText { Point2 pos; std::string text; };
	std::vector<DebugText> mRenderDebugText;
	/**
	*	\brief 检查是否需要九宫格处理
	*/
	bool CheckImageGrid(const ImageGrid& imageGrid)
	{
		return true;
	}

	/**
	*	\brief 获取对应的九宫格信息
	*	\param subImage 返回局部信息
	*	\param index 获取九宫格的索引
	*/
	bool GetImageGrid(Rect& subDestRect ,Rect& subImgRect, int index, const ImageRenderInfo& info)
	{
		return true;
	}
}

bool UIRender::Initiazlize()
{
	vg = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_DEBUG);
	if (vg == NULL)
	{
		Logger::Error("Failed to create nanovg context");
		return false;
	}

	InitDebugData();

	return true;
}

bool UIRender::Quit()
{
	freeDemoData(vg, &data);
	nvgDeleteGL3(vg);
	return true;
}

void UIRender::BeginRender(int width, int height, float ratio)
{
	nvgBeginFrame(vg, width, height, ratio);
}

void UIRender::EndRender()
{
	nvgEndFrame(vg);
}

void UIRender::BeginGray()
{
}

void UIRender::EndGray()
{
}

void UIRender::BeginMask()
{
}

void UIRender::EndMask()
{
}

/**
*	\brief 渲染图片widget
*	\param imageInfo 包含九宫格的图片信息
*/
void UIRender::RenderImage(const ImageRenderInfo & imageInfo)
{
	const Size& imageSize = imageInfo.imageSize;
	if (imageSize.width <= 0 || imageSize.height <= 0)
	{
		return;
	}

	const Rect& imageRect = imageInfo.imageRect;
	if (imageRect.width <= 0 || imageRect.height <= 0)
	{
		return;
	}

	float imageVerts[4 * 9] = { 0 };
	float imageUv[4 * 9] = { 0 };
	int count = 0;
	if (CheckImageGrid(imageInfo.imageGrid))
	{
		Rect subImageRect, subDestRect;
		for (int i = 0; i < 9; i++)
		{
			bool needDraw = GetImageGrid(subDestRect, subImageRect, i, imageInfo);
			if (needDraw)
			{
				// vert data
				imageVerts[count * 4 + 0] = (float)subDestRect.x;
				imageVerts[count * 4 + 1] = (float)subDestRect.y;
				imageVerts[count * 4 + 2] = (float)(subDestRect.x + subDestRect.width);
				imageVerts[count * 4 + 3] = (float)(subDestRect.y + subDestRect.height);

				// uv data
				imageUv[count * 4 + 0] = (float)(subImageRect.x / imageSize.width);
				imageUv[count * 4 + 1] = (float)(subImageRect.y / imageSize.height);
				imageUv[count * 4 + 2] = (float)(subImageRect.x + subImageRect.width) / imageSize.width;
				imageUv[count * 4 + 3] = (float)(subImageRect.y + subImageRect.height) / imageSize.width;
				count++;
			}
		}
	}
}

void UIRender::RenderText(const std::string & str, int x, int y)
{
	DebugText debugText;
	debugText.pos = Point2(x, y);
	debugText.text = str;
	mRenderDebugText.push_back(debugText);
}

/**
*	\brief 绘制颜色矩形，目前临时作为绘制debug rect
*/
void UIRender::RenderShape(const Rect & rect, const Color4B& color)
{
	DebugRect debugRect;
	debugRect.rect = rect;
	debugRect.color = color;
	mRenderDebugRect.push_back(debugRect);
}

/**********************************************************
						Debug 
**********************************************************/

void UIRender::initGraph(PerfGraph * fps, int style, const char * name)
{
	memset(fps, 0, sizeof(PerfGraph));
	fps->style = style;
	strncpy(fps->name, name, sizeof(fps->name));
	fps->name[sizeof(fps->name) - 1] = '\0';
}

void UIRender::updateGraph(PerfGraph * fps, float frameTime)
{
	fps->head = (fps->head + 1) % GRAPH_HISTORY_COUNT;
	fps->values[fps->head] = frameTime;
}

float UIRender::getGraphAverage(PerfGraph * fps)
{
	int i;
	float avg = 0;
	for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
		avg += fps->values[i];
	}
	return avg / (float)GRAPH_HISTORY_COUNT;
}

void UIRender::renderGraph(float x, float y, PerfGraph * fps)
{
	int i;
	float avg, w, h;
	char str[64];

	avg = getGraphAverage(fps);

	w = 200;
	h = 35;

	nvgBeginPath(vg);
	nvgRect(vg, x, y, w, h);
	nvgFillColor(vg, nvgRGBA(0, 0, 0, 128));
	nvgFill(vg);

	nvgBeginPath(vg);
	nvgMoveTo(vg, x, y + h);
	if (fps->style == GRAPH_RENDER_FPS) {
		for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
			float v = 1.0f / (0.00001f + fps->values[(fps->head + i) % GRAPH_HISTORY_COUNT]);
			float vx, vy;
			if (v > 80.0f) v = 80.0f;
			vx = x + ((float)i / (GRAPH_HISTORY_COUNT - 1)) * w;
			vy = y + h - ((v / 80.0f) * h);
			nvgLineTo(vg, vx, vy);
		}
	}
	else if (fps->style == GRAPH_RENDER_PERCENT) {
		for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
			float v = fps->values[(fps->head + i) % GRAPH_HISTORY_COUNT] * 1.0f;
			float vx, vy;
			if (v > 100.0f) v = 100.0f;
			vx = x + ((float)i / (GRAPH_HISTORY_COUNT - 1)) * w;
			vy = y + h - ((v / 100.0f) * h);
			nvgLineTo(vg, vx, vy);
		}
	}
	else {
		for (i = 0; i < GRAPH_HISTORY_COUNT; i++) {
			float v = fps->values[(fps->head + i) % GRAPH_HISTORY_COUNT] * 1000.0f;
			float vx, vy;
			if (v > 20.0f) v = 20.0f;
			vx = x + ((float)i / (GRAPH_HISTORY_COUNT - 1)) * w;
			vy = y + h - ((v / 20.0f) * h);
			nvgLineTo(vg, vx, vy);
		}
	}
	nvgLineTo(vg, x + w, y + h);
	nvgFillColor(vg, nvgRGBA(255, 192, 0, 128));
	nvgFill(vg);

	nvgFontFace(vg, "sans");

	if (fps->name[0] != '\0') {
		nvgFontSize(vg, 14.0f);
		nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240, 240, 240, 192));
		nvgText(vg, x + 3, y + 1, fps->name, NULL);
	}

	if (fps->style == GRAPH_RENDER_FPS) {
		nvgFontSize(vg, 18.0f);
		nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240, 240, 240, 255));
		sprintf(str, "%.2f FPS", 1.0f / avg);
		nvgText(vg, x + w - 3, y + 1, str, NULL);

		nvgFontSize(vg, 15.0f);
		nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_BOTTOM);
		nvgFillColor(vg, nvgRGBA(240, 240, 240, 160));
		sprintf(str, "%.2f ms", avg * 1000.0f);
		nvgText(vg, x + w - 3, y + h - 1, str, NULL);
	}
	else if (fps->style == GRAPH_RENDER_PERCENT) {
		nvgFontSize(vg, 18.0f);
		nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240, 240, 240, 255));
		sprintf(str, "%.1f %%", avg * 1.0f);
		nvgText(vg, x + w - 3, y + 1, str, NULL);
	}
	else {
		nvgFontSize(vg, 18.0f);
		nvgTextAlign(vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);
		nvgFillColor(vg, nvgRGBA(240, 240, 240, 255));
		sprintf(str, "%.2f ms", avg * 1000.0f);
		nvgText(vg, x + w - 3, y + 1, str, NULL);
	}
}

namespace {
	std::string debugBoardString = "";
}
/**
*	\brief 绘制debug board
*/
void UIRender::renderDebugBoard(float x, float y, float w, float h)
{
	// debug board bg
	nvgBeginPath(vg);
	nvgRect(vg, x, y, w, h);
	nvgFillColor(vg, nvgRGBA(50, 50, 0, 128));
	nvgFill(vg);

	// debug string,临时做法
	NVGtextRow rows[3];
	int nrows;
	float lineh;

	nvgFontSize(vg, 18.0f);
	nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
	nvgFillColor(vg, nvgRGBA(240, 240, 240, 255));
	nvgTextMetrics(vg, NULL, NULL, &lineh);

	const char *start = debugBoardString.data();
	const char* end = start + strlen(debugBoardString.c_str());
	while ((nrows = nvgTextBreakLines(vg, start, end, w, rows, 3))) {
		for (int i = 0; i < nrows; i++) {
			NVGtextRow* row = &rows[i];		
			nvgBeginPath(vg);
			nvgText(vg, x, y, row->start, row->end);

			y += lineh;
		}
		start = rows[nrows - 1].next;
	}
	debugBoardString = "";

	// 绘制debugRect
	for (auto debugRect : mRenderDebugRect)
	{
		nvgBeginPath(vg);
		Rect rect = debugRect.rect;
		Color4B color = debugRect.color;
		nvgRect(vg, rect.x, rect.y, rect.width, rect.height);
		nvgFillColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
		nvgFill(vg);
	}
	mRenderDebugRect.clear();

	// 绘制debugText
	for (auto debugText : mRenderDebugText)
	{
		auto& text = debugText.text;
		const char *start = text.data();
		const char* end = start + strlen(text.c_str());
		nvgBeginPath(vg);
		nvgText(vg, debugText.pos.x, debugText.pos.y, start, end);
	}
	mRenderDebugText.clear();
}

/**
*	\brief 绘制debug文本
*/
void UIRender::renderDebugString(const string & str)
{
	debugBoardString += str + string("\n");
}

void UIRender::InitDebugData()
{
	// debug
	if (loadDemoData(vg, &data) == -1)
		Logger::Error("Failed to Load nanovg demo data");

	initGraph(&fps, GRAPH_RENDER_FPS, "frame time");
}

#include<iostream>
/**
*	\brief 绘制demo
*/
void UIRender::RenderDebugDemo()
{
	Size fbSize = Video::GetScreenSize();
	float fbWidth = (float)fbSize.width;
	float fbHeight = (float)fbSize.height;
	float mx = fbWidth * 0.5f;
	float my = fbHeight * 0.5f;
	/*float t = (float)glfwGetTime();*/
	nvgBeginFrame(vg, (int)fbWidth, (int)fbHeight, 1.0f);
	//renderDemo(vg, mx, my, fbWidth, fbHeight, t, false, &data);
	renderGraph(5, 5, &fps);
	renderDebugBoard(5, 50, 200, 100);
	nvgEndFrame(vg);

	float dt = (float)Video::GetFrameDelta() / 1000.0f;
	updateGraph(&fps, dt);
}

}