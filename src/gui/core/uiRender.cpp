#include"gui\core\uiRender.h"
#include"utils\nanovg.h"
#include"utils\nanovg_gl.h"
#include"core\logger.h"
#include"core\fileData.h"

// debug
#include"main\demo.h"
#include"core\video.h"
#include"thirdparty\stb_image.h"

// widget
#include"gui\widget\button.h"

#include<numeric>

namespace gui
{
namespace
{
	/** UIRender static member  */
	// core data
	NVGcontext* vg = nullptr;
	DemoData data;
	PerfGraph fps;

	// debug data
	struct DebugRect { Rect rect; Color4B color; };
	std::vector<DebugRect> mRenderDebugRect;	/** ���Ƶ�rect,��ǰ��ʱ��ô�� */
	struct DebugText { Point2 pos; std::string text; };
	std::vector<DebugText> mRenderDebugText;

	// resource data
	const std::string uiNormalFontFace = "ui_normal";
	const std::string uiSystemFontFace = "sans";
	std::map<std::string, int> imageHandles;
	std::set<std::string> fontSets;
	int fontEmoji;

	/**
	*	\brief ����Ƿ���Ҫ�Ź�����
	*/
	bool CheckImageGrid(const ImageGrid& imageGrid)
	{
		return true;
	}

	/**
	*	\brief ��ȡ��Ӧ�ľŹ�����Ϣ
	*	\param subImage ���ؾֲ���Ϣ
	*	\param index ��ȡ�Ź��������(�����Ͻǵ����½ǣ�
	*/
	bool GetImageGrid(Rect& subDestRect ,Rect& subImgRect, int index, const ImageRenderInfo& info)
	{
		Size imageSize = info.imageSize;
		ImageGrid grid = info.imageGrid;
		std::vector<int> widths  = { grid.left, imageSize.width  - grid.left - grid.right,  grid.right };
		std::vector<int> heights = { grid.top,  imageSize.height - grid.top  - grid.bottom, grid.bottom };

		int col = index % 3;
		int row = index / 3;
		if (widths[col] > 0 && heights[row] > 0) 
		{
			// imgeRect
			int posX = std::accumulate(widths.begin(),  widths.begin() + col, 0);
			int posY = std::accumulate(heights.begin(), heights.begin() + row, 0);
			subImgRect.SetPos(posX, posY);
			subImgRect.SetSize(widths[col], heights[row]);

			// destRect
			subDestRect.SetPos(info.imageRect.GetPos());
			subDestRect.SetSize(info.imageRect.GetSize());

			return true;
		}
		return false;
	}

	void RenderSystemStyleButton(NVGcontext* vg, const char* text, float x, float y, float w, float h, NVGcolor col)
	{
		float cornerRadius = 4.0f;
		NVGpaint bg = nvgLinearGradient(vg, x, y, x, y + h, nvgRGBA(255, 255, 255,  32), nvgRGBA(0, 0, 0, 32));
		nvgBeginPath(vg);
		nvgRoundedRect(vg, x + 1, y + 1, w - 2, h - 2, cornerRadius - 1);
		nvgFillColor(vg, col);
		nvgFill(vg);
		nvgFillPaint(vg, bg);
		nvgFill(vg);

		nvgBeginPath(vg);
		nvgRoundedRect(vg, x + 0.5f, y + 0.5f, w - 1, h - 1, cornerRadius - 0.5f);
		nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 48));
		nvgStroke(vg);
	
		if (text != nullptr) {
			float tw = 0, iw = 0;
			nvgFontSize(vg, 20.0f);
			nvgFontFace(vg, "sans-bold");
			tw = nvgTextBounds(vg, 0, 0, text, NULL, NULL);

			nvgFontSize(vg, 20.0f);
			nvgFontFace(vg, "sans-bold");
			nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
			nvgFillColor(vg, nvgRGBA(0, 0, 0, 160));
			nvgText(vg, x + w*0.5f - tw*0.5f + iw*0.25f, y + h*0.5f - 1, text, NULL);
			nvgFillColor(vg, nvgRGBA(255, 255, 255, 160));
			nvgText(vg, x + w*0.5f - tw*0.5f + iw*0.25f, y + h*0.5f, text, NULL);
		}
	}

	void SetCurrentFontState(NVGcontext* vg, const TextRenderInfo& info)
	{
		if (!UIRender::HasFontLoaded(info.fontName.c_str()))
		{
			Debug::Error("The font " + info.fontName + " is not loaded.");
			return;
		}
		nvgFontSize(vg, info.fontSize);
		nvgFontFace(vg, info.fontName.c_str());
		nvgFillColor(vg, nvgRGBA(info.fontColor.r, info.fontColor.g, info.fontColor.b, info.fontColor.a));
		nvgTextAlign(vg, info.fontAlign);
	}

	void RenderSingleLineText(NVGcontext* vg, const TextRenderInfo& info)
	{
		SetCurrentFontState(vg, info);
		const auto rect = info.fontRect;
		nvgText(vg, (float)rect.x, (float)rect.y, info.text.c_str(), NULL);
	}

	void RenderMultLineText(NVGcontext* vg, const TextRenderInfo& info)
	{
		const Rect fontRect = info.fontRect;
		const char* start = info.text.c_str();
		const char* end = start + strlen(start);

		// �����ı�����
		SetCurrentFontState(vg, info);

		// ���ݿ�Ⱥͻ��з�ÿ�λ�ȡ��ǰ���ı���ǰ��������
		NVGtextRow rows[2];
		float xStart = (float)(fontRect.GetPos().x);
		float yStart = (float)(fontRect.GetPos().y);
		int nrows = 0;
		while ((nrows = nvgTextBreakLines(vg, start, end, (float)fontRect.width, rows, 2)))
		{
			for (int i = 0; i < nrows; i++)
			{
				NVGtextRow& row = rows[i];

				nvgBeginPath(vg);
				nvgFillColor(vg, nvgRGBA(info.fontColor.r, info.fontColor.g, info.fontColor.b, info.fontColor.a));
				nvgText(vg, xStart, yStart, row.start, row.end);

				yStart += info.lineHeight;
			}
			start = rows[nrows-1].next;
		}
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
	for (const auto& kvp : imageHandles)
		nvgDeleteImage(vg, kvp.second);
	imageHandles.clear();

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
*	\brief ��ȾͼƬwidget
*	\param imageInfo �����Ź����ͼƬ��Ϣ
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

	if (imageInfo.imagePath == "")
	{
		return;
	}

	float imageVerts[4 * 9] = { 0 };
	float imageUVs[4 * 9] = { 0 };
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
				imageUVs[count * 4 + 0] = (float)(subImageRect.x / imageSize.width);
				imageUVs[count * 4 + 1] = (float)(subImageRect.y / imageSize.height);
				imageUVs[count * 4 + 2] = (float)(subImageRect.x + subImageRect.width) / imageSize.width;
				imageUVs[count * 4 + 3] = (float)(subImageRect.y + subImageRect.height) / imageSize.height;
				count++;
			}
		}

		// ���������Ҫ���Ƶ�rect�����
		int imageHandle = LoadUIImage(imageInfo.imagePath);	// �Ƿ������һ��get�ӿڣ�
		float imageWidth  = (float)(imageSize.width);
		float imageHeight = (float)(imageSize.height);
		for (int i = 0; i < count; i++)
		{
			float* imageVert = &imageVerts[i * 4];
			float* imageUV = &imageUVs[i * 4];
			float gridWidth = imageVert[2] - imageVert[0];
			float gridHeight = imageVert[3] - imageVert[1];
			float xStart = imageVert[0];
			float yStart = imageVert[1];

			NVGpaint imgPaint = nvgImagePattern(vg,
				xStart, yStart,
				gridWidth, gridHeight,
				0.0f / 180.0f*NVG_PI,
				imageHandle,
				1.0f);

			float x = xStart + gridWidth  * imageUV[0];
			float y = yStart + gridHeight * imageUV[1];
			float w = gridWidth * (imageUV[2] - imageUV[0]);
			float h = gridHeight* (imageUV[3] - imageUV[1]);
			nvgBeginPath(vg);
			nvgRoundedRect(vg, x, y, w, h, 5);
			nvgFillPaint(vg, imgPaint);
			nvgFill(vg);
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
*	\brief ������ɫ���Σ�Ŀǰ��ʱ��Ϊ����debug rect
*/
void UIRender::RenderShape(const Rect & rect, const Color4B& color)
{
	DebugRect debugRect;
	debugRect.rect = rect;
	debugRect.color = color;
	mRenderDebugRect.push_back(debugRect);
}

void UIRender::RenderButton(const Button & button)
{
	if (button.IsUseSystemStyle())
	{
		const Point2 pos = button.GetPositon();
		const Size size = button.GetSize();
		const std::string label = button.GetLable();
		const Color4B color = button.GetCurColor();
		RenderSystemStyleButton(vg, 
			label.empty() ? "HELLO" : label.c_str(),
			(float)pos.x, (float)pos.y,
			(float)size.width, (float)size.height,
			nvgRGBA(color.r, color.g, color.b, color.a));
	}
}

void UIRender::RenderLabel(const TextRenderInfo & labelInfo)
{
	if (labelInfo.text != "")
	{
		if (labelInfo.multline)
			RenderMultLineText(vg, labelInfo);
		else
			RenderSingleLineText(vg, labelInfo);
	}
}

/**
*	\brief ������Դ����Դ�Ỻ����Nanovg��,��Handle��ʽ����
*
*	TODO Ŀǰ�����Ǽ���ɢ�ļ���δ����Ҫ���������֡��������һ��ͼƬ�ж�ȡ
*/
int UIRender::LoadUIImage(const std::string & path, int imageFlag)
{
	auto findIt = imageHandles.find(path);
	if (findIt != imageHandles.end())
	{
		//Debug::Warning("The ui image " + path + "has already loaded.");
		return findIt->second;
	}

	const string data = FileData::ReadFile(path); 
	int handle = nvgCreateImageMem(vg,imageFlag, (unsigned char*)data.c_str(), data.length());
	imageHandles[path] = handle;

	return handle;
}

/**
*	\brief ��ȡ�ƶ���Դ�Ĵ�С
*/
Size UIRender::GetImageSize(const std::string & path)
{
	Size ret(0, 0);
	auto findIt = imageHandles.find(path);
	if (findIt != imageHandles.end())
	{
		int imageHandle = findIt->second;
		int w, h;
		nvgImageSize(vg, imageHandle, &w, &h);
		ret.width = w;
		ret.height = h;
	}
	return ret;
}

int UIRender::LoadFontTTF(const std::string & id, const std::string & path, bool fallback )
{
	// ���������ظ�����
	if (fontSets.find(id) != fontSets.end())
	{
		Debug::Warning("The ui font " + path + "has already loaded.");
		return -1;
	}

	int length = 0;
	const char* data = FileData::ReadFileBytes(path, length);
	int font = nvgCreateFontMem(vg, id.c_str(), (unsigned char*)data, length, 1);
	if (font == -1)
	{
		Debug::Warning("The ui font " + path + "loaded failed.");
		return -1;
	}	

	// ����������Чʱ������
	if(fallback)
		nvgAddFallbackFontId(vg, font, fontEmoji);

	fontSets.insert(id);
	return font;
}

bool UIRender::HasFontLoaded(const std::string & id)
{
	return fontSets.find(id) != fontSets.end();
}

std::string UIRender::GetSystemFontFace()
{
	return uiNormalFontFace;
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
*	\brief ����debug board
*/
void UIRender::renderDebugBoard(float x, float y, float w, float h)
{
	//return;

	// debug board bg
	nvgBeginPath(vg);
	nvgRect(vg, x, y, w, h);
	nvgFillColor(vg, nvgRGBA(50, 50, 0, 128));
	nvgFill(vg);

	// debug string,��ʱ����
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

	// ����debugRect
	for (auto debugRect : mRenderDebugRect)
	{
		nvgBeginPath(vg);
		Rect rect = debugRect.rect;
		Color4B color = debugRect.color;
		nvgRect(vg, (float)rect.x, (float)rect.y, (float)rect.width, (float)rect.height);
		nvgFillColor(vg, nvgRGBA(color.r, color.g, color.b, color.a));
		nvgFill(vg);
	}
	mRenderDebugRect.clear();

	// ����debugText
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
*	\brief ����debug�ı�
*/
void UIRender::renderDebugString(const string & str)
{
	debugBoardString += str + string("\n");
}

void UIRender::InitDebugData()
{
	// debug
	//if (loadDemoData(vg, &data) == -1)
	//	Logger::Error("Failed to Load nanovg demo data");

	initGraph(&fps, GRAPH_RENDER_FPS, "frame time");

	// load default font
	fontEmoji = LoadFontTTF("emoji", "fonts/NotoEmoji-Regular.ttf", false);

	LoadFontTTF(uiSystemFontFace, "fonts/Roboto-Regular.ttf");
	LoadFontTTF(uiNormalFontFace, "fonts/pingfang.ttf");
}

#include<iostream>
/**
*	\brief ����demo
*/
void UIRender::RenderDebugDemo()
{
	Size fbSize = Video::GetScreenSize();
	float fbWidth = (float)fbSize.width;
	float fbHeight = (float)fbSize.height;
	float mx = fbWidth * 0.5f;
	float my = fbHeight * 0.5f;
	//float t = (float)glfwGetTime();
	nvgBeginFrame(vg, (int)fbWidth, (int)fbHeight, 1.0f);
	//renderDemo(vg, mx, my, fbWidth, fbHeight, t, false, &data);
	renderGraph(5, 5, &fps);
	renderDebugBoard(5, 50, 200, 100);
	nvgEndFrame(vg);

	float dt = (float)Video::GetFrameDelta() / 1000.0f;
	updateGraph(&fps, dt);
}

}