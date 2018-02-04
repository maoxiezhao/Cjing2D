#include"core\font.h"
#include"core\resourceCache.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"thirdparty\SOIL.h"
#include"core\logger.h"

namespace font {

namespace {
	const int MAX_FONT_TEXTURE_WIDTH = 2048;
	const int MAX_FONT_TEXTURE_HEIGHT = 2048;
	const int BEARING_STANDARD_HEIGHT_CODE = 0x4E09;	// 用"三"做基准线
}

Font::Font():
	mFontSize(0),
	mScale(1.0f),
	mAscent(0),
	mDescent(0),
	mLetterSpacing(0),
	mLineHeight(0),
	mConfig(nullptr),
	mTexture(nullptr),
	mProgramState(nullptr),
	mIsDirty(false),
	mIsLoaded(false),
	mIsDynamicLoad(false)
{
	InitRender();
}

Font::Font(const string & font):
	mFontName(font),
	mFontSize(0),
	mScale(1.0f),
	mAscent(0),
	mDescent(0),
	mLetterSpacing(0),
	mLineHeight(0),
	mConfig(nullptr),
	mTexture(nullptr),
	mProgramState(nullptr),
	mIsDirty(false),
	mIsLoaded(false),
	mIsDynamicLoad(false)
{
	InitRender();
}

Font::~Font()
{
	UnLoad();
}

void Font::UnLoad()
{
	if (IsLoaded())
	{
		if (IsDynamicLoadTexture())
		{
			FT_Done_Face(face);
			FT_Done_FreeType(ft);
		}
	}
	mGlyphs.clear();
}

/**
*	\brief 刷新过程
*
*	刷新过程会遍历整个字模表，对于长期不使用的字模，
*	清除动作，仅用于动态加载字体纹理使用
*/
void Font::Flush()
{
}

/**
*	\brief 加载字体文件
*/
void Font::LoadFont(const string& fontData, const std::vector<FontCodeRange>& codeRanges)
{
	Logger::Info("Start loading Font.");

	if (IsLoaded() || mFontName.empty())
	{
		return;
	}
	if (FT_Init_FreeType(&ft))
	{
		Debug::Error("Faild to init FT_Library.");
	}
	if (fontData.empty())
	{
		Debug::Error("Failed to open font file.");
	}
	if (FT_New_Memory_Face(ft, (unsigned char*)fontData.c_str(), fontData.size(), 0, &face))
	{
		Debug::Error("Failed to create new font face.");
	}
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(face, 0, mFontSize);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	mAscent = face->ascender >> 6;
	mDescent = face->descender >> 6;
	
	// 特殊统一处理不可见字符（小于33）
	for (unsigned int code = 0; code < 33; code++)
	{
		if (FT_Load_Char(face, code , FT_LOAD_RENDER))
		{
			Debug::Warning("failed to load char.");
			continue;
		}
		Glyph glyph;
		glyph.advance = face->glyph->advance.x;
		glyph.charCode = code;
		glyph.size = Size(face->glyph->bitmap.width, face->glyph->bitmap.rows);
		glyph.bearing = Size(face->glyph->bitmap_left, face->glyph->bitmap_top);

		mGlyphs[code] = glyph;
	}

	int ammount = 0;
	if (!IsDynamicLoadTexture())
	{
		// 当未开启动态加载字体的时候，加载全部字模，并输出到纹理RGBA四个通道
		// 速度过于慢，建议使用异步加载
		mTexture = ResourceCache::GetInstance().CreateTexture2DBySize("mFontName", { MAX_FONT_TEXTURE_WIDTH, MAX_FONT_TEXTURE_HEIGHT });

		glBindTexture(GL_TEXTURE_2D, mTexture->GetTextureID());
		Point2 startPos = Point2(0, 0);
		const int advanceStep = 5;

		for (const auto& fontCodeRange : codeRanges)
		{
			for (uint32_t code = fontCodeRange.codeBegin; code <= fontCodeRange.codeEnd; code += 4)
			{
				std::vector<unsigned int> mDstColor;
				unsigned int maxCharWidth = 0;
				unsigned int maxCharHeight = 0;
				for (int i = 0; i < 4; i++)
				{
					ammount++;
					if (FT_Load_Char(face, code + i, FT_LOAD_RENDER))
					{
						Debug::Warning("failed to load char.");
						continue;
					}
					if ((unsigned int)face->glyph->bitmap.pitch > maxCharWidth)
					{
						maxCharWidth = face->glyph->bitmap.pitch;
					}
					if (face->glyph->bitmap.rows > maxCharHeight)
					{
						maxCharHeight = face->glyph->bitmap.rows;
					}
				}
				if (maxCharHeight == 0 || maxCharWidth == 0)
				{
					continue;
				}

				mDstColor.resize(maxCharWidth * maxCharHeight);
				auto ToRGBA = [maxCharWidth, maxCharHeight](GLenum curFormat, const unsigned char* src,
					std::vector<unsigned int>& dst, int srcPitch, int srcRow)
				{
					for (int row = 0; row < srcRow; row++)
					{
						for (int col = 0; col < srcPitch; col++)
						{
							switch (curFormat)
							{
							case GL_RED:
								dst[row * maxCharWidth + col] |= src[row * srcPitch + col] << 0;
								break;
							case GL_GREEN:
								dst[row * maxCharWidth + col] |= src[row * srcPitch + col] << 8;
								break;
							case GL_BLUE:
								dst[row * maxCharWidth + col] |= src[row * srcPitch + col] << 16;
								break;
							case GL_ALPHA:
								dst[row * maxCharWidth + col] |= src[row * srcPitch + col] << 24;
							default:
								break;
							}
						}
					}
				};

				for (int i = 0; i < 4; i++)
				{
					if (FT_Load_Char(face, code + i, FT_LOAD_RENDER))
					{
						Debug::Warning("failed to load char.");
						continue;
					}

					Glyph glyph;
					glyph.advance = face->glyph->advance.x;
					glyph.charCode = code + i;
					glyph.chanel = i;
					glyph.size = Size(face->glyph->bitmap.width, face->glyph->bitmap.rows);
					glyph.bearing = Size(face->glyph->bitmap_left, face->glyph->bitmap_top);

					float u0 = (float(startPos.x) / MAX_FONT_TEXTURE_WIDTH);
					float v0 = (float(startPos.y) / MAX_FONT_TEXTURE_HEIGHT);
					float u1 = (float(startPos.x + face->glyph->bitmap.width)) / MAX_FONT_TEXTURE_WIDTH;
					float v1 = (float(startPos.y + face->glyph->bitmap.rows)) / MAX_FONT_TEXTURE_HEIGHT;
					glyph.u0 = u0;
					glyph.v0 = v0;
					glyph.u1 = u1;
					glyph.v1 = v1;
					mGlyphs[code + i] = glyph;

					ToRGBA(GL_RED + i, face->glyph->bitmap.buffer, mDstColor, face->glyph->bitmap.pitch, face->glyph->bitmap.rows);

				}

				if (startPos.x + maxCharWidth >= MAX_FONT_TEXTURE_WIDTH)
				{
					startPos.x = 0;
					if (startPos.y + maxCharHeight > MAX_FONT_TEXTURE_HEIGHT)
					{
						break;
					}
					startPos.y += maxCharHeight + advanceStep;
				}
				unsigned char* pixels = (unsigned char*)mDstColor.data();
				glTexSubImage2D(GL_TEXTURE_2D, 0, startPos.x, startPos.y, maxCharWidth, maxCharHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
				startPos.x += (face->glyph->advance.x >> 6) + advanceStep;
			}
		}
		
		glBindTexture(GL_TEXTURE_2D, 0);

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
	else
	{
		// 动态维护纹理信息，需要指定纹理大小或默认纹理大小，通过LRU方法维护
		// 未来版本将会添加
		for (uint32_t code = FONT_CODE_CHINESE_START; code <= FONT_CODE_CHINESE_END; code ++)
		{
			if (FT_Load_Char(face, code, FT_LOAD_RENDER))
			{
				Debug::Warning("failed to load char.");
				continue;
			}
			Glyph glyph;
			glyph.advance = face->glyph->advance.x;
			glyph.charCode = code;
			glyph.chanel = GL_RED;
			mGlyphs[code] = glyph;
		}
	}
	mIsLoaded = true;
	Logger::Info("Load Font successed.");
}

/**
*	\brief 建立动态查询表
*/
void Font::BuildLookupTable()
{
}

/**
*	\brief 绘制文字，简化了的渲染接口
*	\param size 子号
*	\param pos  位置
*	\param renderText 渲染文字
*/
void Font::RenderText(float size, const Point2 & pos, const string & renderText, unsigned int textAlign)
{
	if (renderText.empty())
	{
		return;
	}
	const Rect clipRect = Rect(0, 0);
	float wrapWidth = 0.0f;
	const char* textBegin = renderText.c_str();

	RenderText(size, pos, clipRect, wrapWidth, textBegin, nullptr, textAlign);
}

/**
*	\brief 渲染文字到指定位置
*	\param size 子号
*	\param pos  位置
*	\param cols 列数
*	\param clipRect 裁剪矩形
*	\param renderText 渲染文字
*/
void Font::RenderText(float size, const Point2 & pos, const Rect & clipRect, float wrapWidth, const string& renderText, unsigned int textAlign)
{
	if (renderText.empty())
	{
		return;
	}

	const char* textBegin = renderText.c_str();
	RenderText(size, pos, clipRect, wrapWidth, textBegin, nullptr, textAlign);
}

/**
*	\brief 渲染文字到指定位置
*	\param size 子号
*	\param pos  位置
*	\param cols 列数
*	\param clipRect 裁剪矩形
*	\param renderText 渲染文字
*/
void Font::RenderText(float size, const Point2 & pos, const Rect & clipRect, float wrapWidth, 
				const char * textBegin, const char * textEnd, unsigned int textAlign)
{
	int posX = pos.x;
	int posY = pos.y;

	float scale = size / mFontSize;
	int lineHeight = (int)size < mLineHeight ? mLineHeight : (int)size;
	bool wrapEnabled = (wrapWidth > 0);
	bool clipEnabled = clipRect.width > 0 && clipRect.height > 0;

	if (clipEnabled && posY >= clipRect.height + clipRect.y)
	{
		return;
	}

	const char* wrapChar = nullptr;
	const char* curCharPtr = textBegin;

	if (textEnd == nullptr)
	{
		textEnd = textBegin + strlen(textBegin);
	}

	auto RenderFuncion = [&](const std::vector<unsigned int>& charCodes, int maxWidth, unsigned int textAlign, int posY)
	{
		// 根据最大长度和对齐方式计算开始位置
		if (textAlign == TEXT_ALIGN_LEFT)
		{
			posX = pos.x;
		}
		else if (textAlign == TEXT_ALIGN_CENTER)
		{
			posX = pos.x - maxWidth / 2;
		}
		else if (textAlign == TEXT_ALIGN_RIGHT)
		{
			posX = pos.x - maxWidth;
		}
		// 遍历字符向量
		for (const auto& curChar : charCodes)
		{	// render
			int charWidth = 0;
			if (const Glyph* glyph = FindGlyph(curChar))
			{
				charWidth = (glyph->advance >> 6);

				// 对于空格符和制表符则直接不进行渲染操作
				if (curChar != ' ' && curChar != '\t')
				{
					int x = posX + (int)(glyph->bearing.width * scale);
					int y = posY + (mAscent - (int)glyph->bearing.height);

					if (!clipEnabled || (x >= clipRect.x && x + charWidth <= (clipRect.x + clipRect.width)))
					{
						const Size charSize = Size(int(glyph->size.width * scale),
												   int(glyph->size.height * scale));

						SetCharTexs(glyph->u0, glyph->v0, glyph->u1, glyph->v1);
						SetCharSize(charSize);
						if (!IsDynamicLoadTexture())
							SetCharChanel((float)glyph->chanel);

						Render(Point2(x, y - lineHeight), charSize, 0);
					}
				}
			}
			posX += (int)(charWidth * scale) + mLetterSpacing;
		}
	};

	// 循环遍历字符，先解码后保存到队列
	// 并计算最大高度和宽度
	std::vector<unsigned int> charCodes;
	int maxWidth = 0;
	int maxHieght = lineHeight;
	while (curCharPtr < textEnd)
	{	// 处理换行
		if (wrapEnabled)
		{
			// 计算需要换行的字符指针位置
			if (wrapChar == nullptr)
			{
				wrapChar = GetWordWraoPosition(curCharPtr, textEnd, scale, wrapWidth - (posX - pos.x));
			}
			if (curCharPtr >= wrapChar)
			{
				RenderFuncion(charCodes, maxWidth, textAlign, posY);

				maxWidth = 0;
				maxHieght += lineHeight;
				posX = pos.x;
				posY += lineHeight;
				wrapChar = nullptr;
				charCodes.clear();
				continue;
			}
		}
		unsigned int curChar = (unsigned int)(*curCharPtr);
		if (curChar < 0x80)
		{	// 单字节
			curCharPtr++;
		}
		else
		{	// 多字节
			curCharPtr += implementation::ParsedUtf8(curChar, curCharPtr, textEnd);
			if (curChar == 0)
			{
				Debug::Warning(string("Invalid text to render. ") + textBegin);
				break;
			}
		}
		// 处理转移符
		if (curChar < 32)
		{
			if (curChar == '\n')
			{
				RenderFuncion(charCodes, maxWidth, textAlign, posY);

				maxWidth = 0;
				maxHieght += lineHeight;
				posX = pos.x;
				posY += lineHeight;
				charCodes.clear();
				if (clipEnabled && posY >= clipRect.height + clipRect.y)
				{
					break;
				}
				if (!wrapEnabled && posY + lineHeight < clipRect.y)
				{
					while (curCharPtr < textEnd && *curCharPtr != '\n')
						curCharPtr++;
				}
				continue;
			}
		}
		int charWidth = 0;
		if (const Glyph* glyph = FindGlyph(curChar))
		{
			charWidth = (glyph->advance >> 6);
		}
		maxWidth += (int)(charWidth * scale) + mLetterSpacing;
		charCodes.push_back(curChar);
	}

	RenderFuncion(charCodes, maxWidth, textAlign, posY);	
}

bool Font::IsLoaded() const
{
	return mIsLoaded;
}

bool Font::IsDirty() const
{
	return mIsDirty;
}

/**
*	\brief 获取文本宽度
*/
int Font::GetTextWidth(const string& text) const
{
	if (!IsLoaded())
	{
		return 0;
	}
	return 0;
}

/**
*	\brief 获取文本高度
*/
int Font::GetTextHeight(const string& text) const
{
	if (!IsLoaded())
	{
		return 0;
	}

	return 0;
}

/***** ***** *****  Glyph ***** ***** *****/

Font::Glyph* Font::FindGlyph(wchar code)
{
	auto itor = mGlyphs.find(code);
	if (itor != mGlyphs.end())
	{
		return &itor->second;
	}
	return nullptr;
}

const Font::Glyph* Font::FindGlyph(wchar code) const
{
	auto itor = mGlyphs.find(code);
	if (itor != mGlyphs.end())
	{
		return &itor->second;
	}
	return nullptr;
}

/**
*	\brief 设置文字颜色
*
*   当使用无纹理的色块时，直接影响显示的颜色
*/
void Font::SetFontColor(const Color4B & color)
{
	mQuad.lt.colors = color;
	mQuad.lb.colors = color;
	mQuad.rt.colors = color;
	mQuad.rb.colors = color;
}

const Color4B Font::GetFontColor() const
{
	return mQuad.rb.colors;
}

void Font::SetFontSize(int size)
{
	mFontSize = size;
}

int Font::GetFontSize() const
{
	return mFontSize;
}

void Font::SetLetterSpacing(int spacing)
{
	mLetterSpacing = spacing;
}

int Font::GetLetterSpacing() const
{
	return mLetterSpacing;
}

void Font::SetDynamicLoadTexture(bool isLoadAll, const Size & size)
{
	if (IsLoaded())
	{
		Debug::Warning("Set Load all texture must before font::load().");
		return;
	}
	mIsDynamicLoad = isLoadAll;
	mFontTextureSize = size;
}

bool Font::IsDynamicLoadTexture() const
{
	return mIsDynamicLoad;
}

/**
*	\biref 初始化渲染状态
*/
void Font::InitRender()
{
	// test init
	SetCharSize({ 512, 480 });
	SetCharTexs(0.0f, 0.0f, 1.0f, 1.0f);
	SetFontSize(16);
	SetFontColor(Color4B::WHITE);
	SetLetterSpacing(1);
	SetLineHeight(mFontSize);

	// 初始化渲染状态
	mProgramState = ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_FONT_NORMAL_PROGRAMSTATE_NAME);
	mModelView = Renderer::GetInstance().GetCameraMatrix();
}

void Font::CalcTextureRect(int width, int height)
{
}

/**
*	\brief 向renderer提交渲染命令
*/
void Font::Render(Renderer & renderer, const Matrix4 & transform)
{
	Debug::CheckAssertion(mProgramState != nullptr, "Invaild programState in Font::Rener().");

	// 为了生成多个quadCommand，且为了支持多态，这里创建只能指针传递,手动管理内存
	// 未来需要考虑更简洁的传递方式
	auto quadCommand = new QuadCommand();

	quadCommand->Init(0, mProgramState,
		mTexture != nullptr ? mTexture->GetTextureID() : 0,	// 这里需要考虑无纹理色块
		0, mQuad, 1, mBlendFunc, transform, mModelView, true);

	renderer.PushCommand(quadCommand);
}

/**
*	\brief 在指定位置、指定大小、指定角度绘制字
*
*   根据位置和角度生成对应的变换矩阵
*/
void Font::Render(const Point2 & pos, const Size& size, float rotate)
{
	Matrix4 transfomr;
	// 因为是右乘坐标，所有操作顺序为从下往上
	// 移动到指定位置
	transfomr = Matrix4::Translate(Vec3f((float)pos.x, (float)pos.y, .0f));

	// 移动到原点旋转后移回
	transfomr *= Matrix4::Translate(Vec3f(size.width*0.5f, size.height*0.5f, 0.0f));
	transfomr *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
	transfomr *= Matrix4::Translate(Vec3f(-size.width*0.5f, -size.height*0.5f, 0.0f));

	Render(Renderer::GetInstance(), transfomr);
}

void Font::SetCharSize(const Size& size)
{
	float x1 = 0;
	float y1 = 0;
	float x2 = x1 + size.width;
	float y2 = y1 + size.height;

	mQuad.lb.vertices = Vec3f(x1, y1, .0f);
	mQuad.lt.vertices = Vec3f(x1, y2, .0f);
	mQuad.rb.vertices = Vec3f(x2, y1, .0f);
	mQuad.rt.vertices = Vec3f(x2, y2, .0f);
}

void Font::SetCharTexs(float u0, float v0, float u1, float v1)
{
	mQuad.lt.texs.u = u0;
	mQuad.lt.texs.v = v1;
	mQuad.lb.texs.u = u0;
	mQuad.lb.texs.v = v0;
	mQuad.rt.texs.u = u1;
	mQuad.rt.texs.v = v1;
	mQuad.rb.texs.u = u1;
	mQuad.rb.texs.v = v0;
}

/**
*	\brief 设置单个字符的缩放比例
*
*	由于字体的大小在加载纹理时就已经设置，所有scale用于在
*	设置指定大小设定缩放比例
*/
void Font::SetCharScale(float scale)
{
	mScale = scale;
}

/**
*	\brief 设置当前字符的渲染所处的通道
*
*	当使用全部加载的时候使用
*/
void Font::SetCharChanel(float chanel)
{
	mQuad.lb.vertices.z = chanel;
	mQuad.lt.vertices.z = chanel;
	mQuad.rb.vertices.z = chanel;
	mQuad.rt.vertices.z = chanel;
}

/**
*	\brief 设置行间距
*
*	这里需要注意，设计上行间距的设置会始终大于等于fontSize
*/
void Font::SetLineHeight(int lineHeight)
{
	mLineHeight = lineHeight;
}

int Font::GetLineHeight() const
{
	return mLineHeight;
}

/**
*	\brief 计算换行的字符
*/
const char* Font::GetWordWraoPosition(const char * textBegin, const char * textEnd, float scale, float wrapWidth)
{
	float wordWidth = 0.0f;
	const char* curCharPtr = textBegin;
	const char* nextCharPtr = nullptr;

	// 需要考虑utf8单字节和多字节情况
	while (curCharPtr < textEnd)
	{
		unsigned int curChar = (unsigned int)*curCharPtr;
		if (curChar < 0x80)
		{
			nextCharPtr = curCharPtr + 1;
		}
		else
		{
			nextCharPtr = curCharPtr + implementation::ParsedUtf8(curChar, curCharPtr, textEnd);
		}
		if (curChar == 0)
		{
			break;
		}
		
		float charWidth = 0;
		if (const Glyph* glyph = FindGlyph(curChar))
		{
			charWidth = (float)(glyph->advance >> 6);
		}
		wordWidth += charWidth;

		if (wordWidth >= wrapWidth)
		{
			return nextCharPtr;
			break;
		}
		curCharPtr = nextCharPtr;

	}

	return curCharPtr;
}

/**
*	\brief 解析utf8编码字符
*	\param outChar 解码后的字符值
*	\param textBegin 解码开始的位置
*	\param textEnd 解码结束的位置
*	\return utf8的字节个数
*
*	1字节  10xxxxxx
*	2字节  110xxxxx 10xxxxxx
*	3字节  1110xxxx 10xxxxxx 10xxxxxx
*	4字节  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
int implementation::ParsedUtf8(unsigned int& outChar, const char * textBegin, const char * textEnd)
{
	unsigned int result = (unsigned int)-1;
	const unsigned char* curChar = (const unsigned char*)textBegin;

	// 单字节
	if ((*curChar & 0xc0) == 0x80)
	{
		result = (unsigned int)(*curChar++);
		outChar = result;
		return 1;
	}
	// 2字节 
	if ((*curChar & 0xe0) == 0xc0)
	{
		result = (unsigned int)((*curChar++ & 0x1F) << 6);
		if ((*curChar & 0xc0) != 0x80)
			return 2;
		result += (unsigned int)(*curChar++ & 0x3f);
		outChar = result;
		return 2;
	}

	// 3字节
	if ((*curChar & 0xf0) == 0xe0)
	{
		result = (unsigned int)((*curChar++ & 0x0F) << 12);
		if ((*curChar & 0xc0) != 0x80)
			return 3;
		result += (unsigned int)((*curChar++ & 0x3f) << 6);
		if ((*curChar & 0xc0) != 0x80)
			return 3;
		result += (unsigned int)(*curChar++ & 0x3f);
		outChar = result;
		return 3;
	}

	// 4字节
	if ((*curChar & 0xf8) == 0xf0)
	{
		result = (unsigned int)((*curChar++ & 0x07) << 18);
		if ((*curChar & 0xc0) != 0x80)
			return 4;
		result += (unsigned int)((*curChar++ & 0x3f) << 12);
		if ((*curChar & 0xc0) != 0x80)
			return 4;
		result += (unsigned int)((*curChar++ & 0x3f) << 6);
		if ((*curChar & 0xc0) != 0x80)
			return 4;
		result += (unsigned int)(*curChar++ & 0x3f);
		outChar = result;
		return 4;
	}
	outChar = 0;
	return 0;
}



}
