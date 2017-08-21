#include"core\font.h"
#include"core\resourceCache.h"
#include"core\fileData.h"
#include"core\debug.h"
#include"thirdparty\SOIL.h"
#include"core\logger.h"

namespace font {

namespace {
	const int MAX_FONT_TEXTURE_WIDTH = 2048;
	const int MAX_FONT_TEXTURE_HEIGHT = 1024;
}

Font::Font():
	mFontSize(0.0f),
	mScale(1.0f),
	mAscent(0.0f),
	mDescent(0.0f),
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
	mFontSize(0.0f),
	mScale(1.0f),
	mAscent(0.0f),
	mDescent(0.0f),
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
		mGlyphs.clear();
		mTimesGlyphs.clear();

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
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
void Font::LoadFont()
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
	if (!FileData::IsFileExists(mFontName))
	{
		Debug::Error("Font file is not existed.");
	}
	string data = FileData::ReadFile(mFontName);
	if (data.empty())
	{
		Debug::Error("Failed to open font file.");
	}
	if (FT_New_Memory_Face(ft, (unsigned char*)data.c_str(), data.size(), 0, &face))
	{
		Debug::Error("Failed to create new font face.");
	}
	FT_Select_Charmap(face, FT_ENCODING_UNICODE);
	FT_Set_Pixel_Sizes(face, 0, 18);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	mTexture = std::make_shared<Texture2D>();
	int ammount = 0;
	if (!IsDynamicLoadTexture())
	{
		// 当未开启动态加载字体的时候，加载全部字模，并输出到纹理RGBA四个通道
		// 速度过于慢，建议使用异步加载

		mTexture->SetSize({MAX_FONT_TEXTURE_WIDTH, MAX_FONT_TEXTURE_HEIGHT});
		mTexture->InitWithChars(nullptr);
		glBindTexture(GL_TEXTURE_2D, mTexture->GetTextureID());
		GLenum curFormat = GL_RED;
		Point2 startPos = Point2(0, 0);
		const int advanceStep = 5;
		for (uint32_t code = FONT_CODE_CHINESE_START; code <= FONT_CODE_CHINESE_END; code+=4)
		{
			std::vector<unsigned int> mDstColor;
			int maxCharWidth = 0;
			int maxCharHeight = 0;
			for (int i = 0; i < 4; i++)
			{
				ammount++;
				if (FT_Load_Char(face, code + i, FT_LOAD_RENDER))
				{
					Debug::Warning("failed to load char.");
					continue;
				}
				if (face->glyph->bitmap.pitch > maxCharWidth)
				{
					maxCharWidth = face->glyph->bitmap.pitch;
				}
				if (face->glyph->bitmap.rows > maxCharHeight)
				{
					maxCharHeight = face->glyph->bitmap.rows;
				}
			}
			if ( maxCharHeight == 0 || maxCharWidth == 0)
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
				ToRGBA(GL_RED + i, face->glyph->bitmap.buffer, mDstColor, face->glyph->bitmap.pitch, face->glyph->bitmap.rows);

				Glyph glyph;
				glyph.advance = (float)face->glyph->advance.x;
				glyph.charCode = code;
				glyph.chanel = GL_RED + i;
				glyph.u0 = (float)(startPos.x / MAX_FONT_TEXTURE_WIDTH);
				glyph.v0 = (float)(startPos.y / MAX_FONT_TEXTURE_HEIGHT);
				glyph.u1 = (float)(startPos.x + face->glyph->bitmap.width) / MAX_FONT_TEXTURE_WIDTH;
				glyph.v1 = (float)(startPos.y + face->glyph->bitmap.rows) / MAX_FONT_TEXTURE_HEIGHT;
				mGlyphs[code] = glyph;
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
		
		glBindTexture(GL_TEXTURE_2D, 0);

		FT_Done_Face(face);
		FT_Done_FreeType(ft);
	}
	else
	{
		// 动态维护纹理信息，需要指定纹理大小或默认纹理大小，通过LRU方法维护
		// 未来版本将会添加

		// 在使用动态版本之前，需要设置纹理大小，否则使用默认的纹理大小
		if (mFontTextureSize.width == 0 || mFontTextureSize.height == 0)
		{
			mFontTextureSize = Size(mFontTextureSize.width, mFontTextureSize.height);
		}
		mTexture->SetSize(mFontTextureSize);
		mTexture->InitWithChars(nullptr);

		for (uint32_t code = FONT_CODE_CHINESE_START; code <= FONT_CODE_CHINESE_END; code ++)
		{
			if (FT_Load_Char(face, code, FT_LOAD_RENDER))
			{
				Debug::Warning("failed to load char.");
				continue;
			}
			Glyph glyph;
			glyph.advance = (float)face->glyph->advance.x;
			glyph.charCode = code;
			glyph.chanel = GL_RED;
			mGlyphs[code] = glyph;
		}
	}
	mIsLoaded = true;
	Logger::Info("Load Font successed.");
}

void Font::BuildLookupTable()
{
}

void Font::RenderText()
{
	Render({ 0,0 }, { 100,100 }, 0);
}

/**
*	\brief 渲染文字
*	\param size 子号
*	\param pos  位置
*	\param cols 列数
*	\param clipRect 裁剪矩形
*	\param renderText 渲染文字
*/
void Font::RenderText(float size, const Point2 & pos, float wrapWidth, const Rect & clipRect, const char* textBegin, const char* textEnd)
{
	// init
	Render({100,100}, {100,100}, 0);

	float posX = (float)pos.x;
	float posY = (float)pos.y;

	if (posY >= (float)(clipRect.y + clipRect.height))
		return;

	float scale = size / mFontSize;
	float lineHeight = size;
	bool wrapEnabled = (wrapWidth > 0.0f);
	const char* curCharPtr = textBegin;

	// encode
	while (curCharPtr < textEnd)
	{
		// 计算换行的位子
		if (wrapEnabled)
		{

		}

		// 根据0x8判断是单字节还是多字节
		const int curChar = *curCharPtr;
		if (curChar < 0x80)
		{
			curCharPtr += 1;
		}
		else
		{	// 处理utf8
			if (curChar == 0)
				break;
			

		}

		float charWidth = 0.0f;



		posX += charWidth;
	}
}

/***** ***** *****  Glyph ***** ***** *****/

Font::Glyph* Font::FindGlyph(wchar code)
{
	return nullptr;
}

const Font::Glyph* Font::FindGlyph(wchar code) const
{
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
	SetCharSize({ 512, 480 });
	SetCharTexs(0.0f, 0.0f, 1.0f, 1.0f);
	SetFontColor(Color4B::WHITE);

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

	mQuadCommand.Init(0, mProgramState,
		mTexture != nullptr ? mTexture->GetTextureID() : 0,	// 这里需要考虑无纹理色块
		&mQuad, 1, mBlendFunc, transform, mModelView);

	renderer.PushCommand(&mQuadCommand);
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



}