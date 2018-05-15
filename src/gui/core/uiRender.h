#pragma once

#include"gui\core\handler.h"
#include"gui\widget\widget.h"
#include"gui\widget\image.h"

namespace gui
{

class Button;


struct TextRenderInfo{};

/**
*	\brief CPU perf工具函数
*/

enum GraphrenderStyle {
	GRAPH_RENDER_FPS,
	GRAPH_RENDER_MS,
	GRAPH_RENDER_PERCENT,
};

#define GRAPH_HISTORY_COUNT 100
struct PerfGraph {
	int style;
	char name[32];
	float values[GRAPH_HISTORY_COUNT];
	int head;
};
typedef struct PerfGraph PerfGraph;

/**
*	\brief ui渲染類
*
*	 uiRender 负责渲染各个类型widget，并维护唯一的
*    nanovg content 实体，暂时实现为零例模式（或者单例模式）
*/

class UIRender
{
public:
	static bool Initiazlize();
	static bool Quit();

	// render mode
	static void BeginRender(int width, int height, float ratio);
	static void EndRender();
	static void BeginGray();
	static void EndGray();
	static void BeginMask();
	static void EndMask();

	// render function 
	static void RenderImage(const ImageRenderInfo& imageInfo);
	static void RenderText(const std::string& str, int x, int y);
	static void RenderShape(const Rect& rect, const Color4B& color);
	static void RenderButton(const Button& button);

	// resource
	static int LoadUIImage(const std::string& path, int imageFlag = 0);
	static Size GetImageSize(const std::string& path);

	// debug
	static void InitDebugData();
	static void RenderDebugDemo();
	static void initGraph(PerfGraph* fps, int style, const char* name);
	static void updateGraph(PerfGraph* fps, float frameTime);
	static void renderGraph(float x, float y, PerfGraph* fps);
	static void renderDebugBoard(float x, float y, float w, float h);
	static void renderDebugString(const string& str);
	static float getGraphAverage(PerfGraph* fps);
};

}