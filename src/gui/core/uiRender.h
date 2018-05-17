#pragma once

#include"gui\core\handler.h"
#include"gui\widget\widget.h"
#include"gui\widget\image.h"
#include"gui\widget\label.h"

namespace gui
{

class Button;
	
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
	static void RenderText(const std::string& str, int x, int y);
	static void RenderShape(const Rect& rect, const Color4B& color);
	static void RenderButton(const Button& button);
	static void RenderLabel(const TextRenderInfo& labelInfo);

	// resource
	static int LoadUIImage(const std::string& path, int imageFlag = 0);
	static Size GetImageSize(const std::string& path);
	static int LoadFontTTF(const std::string& id, const std::string& path, bool fallback = true);
	static bool HasFontLoaded(const std::string&id);
	static std::string GetSystemFontFace();

	// debug
	static void SetDebugDraw(bool drawed);
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