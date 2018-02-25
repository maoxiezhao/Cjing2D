#pragma once

#include"gui\core\handler.h"
#include"gui\widget\widget.h"

namespace gui
{
	struct ImageGrid
	{
		float left;
		float top;
		float right;
		float bottom;

		ImageGrid() :left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
	};

/**
*	\brief ��Ⱦ�����Ϣ
*/
struct ImageRenderInfo
{
	Size imageSize;
	Rect imageRect;
	ImageGrid imageGrid;
};

struct TextRenderInfo
{

};

/**
*	\brief CPU perf���ߺ���
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
*	\brief ui��Ⱦ�
*
*	 uiRender ������Ⱦ��������widget����ά��Ψһ��
*    nanovg content ʵ�壬��ʱʵ��Ϊ����ģʽ�����ߵ���ģʽ��
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
	static void RenderText();
	static void RenderShape(const Rect& rect, const Color4B& color);

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