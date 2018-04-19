#include"sprite.h"
#include"core\texture.h"
#include"core\debug.h"
#include"core\resourceCache.h"
#include"core\system.h"
#include"lua\luaContext.h"
#include"utils\size.h"

/**
*	\brief 创建一个默认的精灵
*/
Sprite::Sprite():
	mProgramState(nullptr),
	mPreProgramState(nullptr),
	mTexture(nullptr),
	mNormalTexture(nullptr),
	mVisible(true),
	mDirty(true),
	mDeferred(false),
	mPreDeferred(false),
	mAnchor(0, 0),
	mAnchorX(0.0f),
	mAnchorY(0.0f),
	mRotateAnchorX(0.0f),
	mRotateAnchorY(0.0f),
	mBlendFunc(),
	mBlinkDelay(0),
	mBlinkNextDate(0),
	mIsBlinkVisible(true),
	mOutLineWidth(0.0f),
	mFlipX(false),
	mFlipY(false)
{
	SetDefaultNormalState();
}

/**
*	\brief 创建一个基于纹理的精灵实例
*/
Sprite::Sprite(const std::string & name):
	mProgramState(nullptr),
	mPreProgramState(nullptr),
	mTexture(nullptr),
	mNormalTexture(nullptr),
	mVisible(true),
	mDirty(true),
	mDeferred(false),
	mPreDeferred(false),
	mAnchor(0,0),
	mAnchorX(0.0f),
	mAnchorY(0.0f),
	mRotateAnchorX(0.0f),
	mRotateAnchorY(0.0f),
	mBlendFunc(),
	mBlinkDelay(0),
	mBlinkNextDate(0),
	mIsBlinkVisible(true),
	mOutLineWidth(0.0f),
	mFlipX(false),
	mFlipY(false)
{
	InitWithFile(name);
	SetDefaultNormalState();
}

/**
*	\brief 创建一个基于纹理的精灵实例
*/
Sprite::Sprite(TexturePtr tex, TexturePtr normalTex):
	mProgramState(nullptr),
	mPreProgramState(nullptr),
	mTexture(tex),
	mNormalTexture(normalTex),
	mVisible(true),
	mDirty(true),
	mDeferred(false),
	mPreDeferred(false),
	mAnchor(0, 0),
	mAnchorX(0.0f),
	mAnchorY(0.0f),
	mRotateAnchorX(0.0f),
	mRotateAnchorY(0.0f),
	mBlendFunc(),
	mBlinkDelay(0),
	mBlinkNextDate(0),
	mIsBlinkVisible(true),
	mOutLineWidth(0.0f),
	mFlipX(false),
	mFlipY(false)
{
	InitWithTexture(mTexture);
	SetDefaultNormalState();
}

/**
*	\brief 创建一个纯色不使用纹理的精灵实例
*	\param 精灵的颜色
*	\param 精灵的大小
*/
Sprite::Sprite(const Color4B & color, const Size & size):
	mProgramState(nullptr),
	mPreProgramState(nullptr),
	mTexture(nullptr),
	mNormalTexture(nullptr),
	mVisible(true),
	mDirty(true),
	mDeferred(false),
	mPreDeferred(false),
	mAnchor(0, 0),
	mAnchorX(0.0f),
	mAnchorY(0.0f),
	mRotateAnchorX(0.0f),
	mRotateAnchorY(0.0f),
	mBlendFunc(),
	mBlinkDelay(0),
	mBlinkNextDate(0),
	mIsBlinkVisible(true),
	mOutLineWidth(0.0f),
	mFlipX(false),
	mFlipY(false)
{
	SetSize(size);
	SetColor(color);
	SetDefaultColorProgramState();
}

Sprite::~Sprite()
{
	mChildSprites.clear();
}

/**
*	\brief 更新自身状态同时更新所有children
*/
void Sprite::Update()
{
	Drawable::Update();

	if (IsSuspended())
	{
		return;
	}

	// 处理blinking
	if (IsBlinking())
	{
		if (System::Now() >= mBlinkNextDate)
		{
			mIsBlinkVisible = !mIsBlinkVisible;
			mBlinkNextDate += mBlinkDelay;
		}
	}

	// 遍历child sprite
	for (auto& sprite : mChildSprites)
		sprite->Update();
}

/**
*	\brief 绘制精灵
*
*   以精灵保存的位置和旋转角度绘制精灵
*/
void Sprite::Draw()
{
	if (IsDirty())
		UpdateTransform();

	if (mVisible && (!IsBlinking() || mIsBlinkVisible))
	{		
		Draw(GetPos(), GetRotated());
	}
}


/**
*	\brief 绘制精灵
*
*   以自定义的位置绘制精灵，角度还是以sprite的为主
*/
void Sprite::Draw(const Point2 & pos)
{
	if (IsDirty())
		UpdateTransform();

	if (mVisible && (!IsBlinking() || mIsBlinkVisible))
	{
		Draw(pos, GetRotated());
	}
}

/**
*	\brief 在指定位置和指定角度绘制精灵
*
*   根据位置和角度生成对应的变换矩阵
*/
void Sprite::Draw(const Point2 & pos, float rotate)
{
	Matrix4 transfomr;
	// 因为是右乘坐标，所有操作顺序为从下往上
	// 移动到指定位置
	transfomr =  Matrix4::Translate(Vec3f((float)pos.x,(float)pos.y, .0f));

	// 移动到原点旋转后移回
	// 旋转的锚点暂时使用和位置一致的 
	//transfomr *= Matrix4::Translate(Vec3f(mSize.width*0.5f, mSize.height*0.5f, 0.0f));
	//transfomr *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
	//transfomr *= Matrix4::Translate(Vec3f(-mSize.width*0.5f, -mSize.height*0.5f, 0.0f));

	// 额外设置旋转锚点
	transfomr *= Matrix4::Translate(Vec3f(mRotateAnchorX, mRotateAnchorY, 0.0f));
	transfomr *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
	transfomr *= Matrix4::Translate(Vec3f(-mRotateAnchorX, -mRotateAnchorY, 0.0f));

	Draw(Renderer::GetInstance(), transfomr);
}

/**
*	\brief 实现多次绘制
*
*	因为存在频繁的renderCommand的创建释放，所以存在
*	性能问题
*/
void Sprite::MultiplyDraw()
{
	if (IsDirty())
		UpdateTransform();

	if (mVisible)
		MultiplyDraw(GetPos(), GetRotated());
}

void Sprite::MultiplyDraw(const Point2 & pos, float rotate)
{
	Matrix4 transfomr;
	// 因为是右乘坐标，所有操作顺序为从下往上
	// 移动到指定位置
	transfomr = Matrix4::Translate(Vec3f((float)pos.x, (float)pos.y, .0f));

	// 移动到原点旋转后移回
	transfomr *= Matrix4::Translate(Vec3f(mSize.width*0.5f, mSize.height*0.5f, 0.0f));
	transfomr *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
	transfomr *= Matrix4::Translate(Vec3f(-mSize.width*0.5f, -mSize.height*0.5f, 0.0f));

	MultiplyDraw(Renderer::GetInstance(), transfomr);
}

/**
*	\brief 向renderer提交渲染命令
*/
void Sprite::Draw(Renderer & renderer, const Matrix4 & transform)
{
	Debug::CheckAssertion(mProgramState != nullptr, "Invaild programState in Sprite::Draw().");

	mQuadCommand.Init(GetGlobalOrder(),mProgramState,
		mTexture != nullptr ? mTexture->GetTextureID(): 0,	// 这里需要考虑无纹理色块
		mNormalTexture != nullptr ? mNormalTexture->GetTextureID() : 0,
		mQuad, 1, mBlendFunc,transform,mModelView);
	mQuadCommand.SetDeferredShade(mDeferred);
	renderer.PushCommand(&mQuadCommand);
}

void Sprite::MultiplyDraw(Renderer & renderer, const Matrix4 & transform)
{
	Debug::CheckAssertion(mProgramState != nullptr, "Invaild programState in Sprite::Draw().");

	auto quadCommand = new QuadCommand();
	quadCommand->Init(0, mProgramState,
		mTexture != nullptr ? mTexture->GetTextureID() : 0,	// 这里需要考虑无纹理色块
		mNormalTexture != nullptr ? mNormalTexture->GetTextureID() : 0,
		mQuad, 1, mBlendFunc, transform, mModelView, true);
	quadCommand->SetDeferredShade(mDeferred);
	renderer.PushCommand(quadCommand);
}

/**
*	\brief 设置该精灵是否可见
*/
void Sprite::SetVisible(bool visible)
{
	mVisible = visible;
}

/**
*	\brief 设置当前纹理
*
*	只是更换当前texurePtr,如果需要重新获得纹理的大小
*   使用InitWithTexture
*/
void Sprite::SetTexture(TexturePtr texture)
{
	mTexture = texture;
}

/**
*	\brief 设置纹理矩形
*	\param 纹理矩形
*	\param 是否调整精灵大小,默认不调整
*/
void Sprite::SetTextureRect(const Rect & rect,bool bResize)
{
	SetTextureCroods(rect);
	if (bResize)
		SetSize(rect.GetSize());
}

/**
*	\brief 设置纹理坐标
*/
void Sprite::SetTextureCroods(const Rect & rect)
{
	Debug::CheckAssertion(mTexture != nullptr, "Set TextureCrooed without texture.");

	float width =  (float)mTexture->GetSize().width;
	float height = (float)mTexture->GetSize().height;
	Point2 orgin = rect.GetPos();
	Size   size = rect.GetSize();

	float left = orgin.x / width;
	float right = (orgin.x + size.width) / width;
	float top = orgin.y / height;
	float bottom = (orgin.y + size.height) / height;

	// do flip if need.
	if (mFlipX)
	{
		std::swap(left, right);
	}
	if (mFlipY)
	{
		std::swap(top, bottom);
	}

	mQuad.lt.texs.u = left;
	mQuad.lt.texs.v = bottom;
	mQuad.lb.texs.u = left;
	mQuad.lb.texs.v = top;
	mQuad.rt.texs.u = right;
	mQuad.rt.texs.v = bottom;
	mQuad.rb.texs.u = right;
	mQuad.rb.texs.v = top;
}

/**
*	\brief 设置精灵大小
*/
void Sprite::SetSize(const Size & size)
{
	mSize = size;
	SetAnchorFloat(mAnchorX, mAnchorY);
	SetDirty(true);
}

/**
*	\brief 设置精灵顶点颜色
*
*   当使用无纹理的色块时，直接影响显示的颜色
*/
void Sprite::SetColor(const Color4B & color)
{
	mQuad.lt.colors = color;
	mQuad.lb.colors = color;
	mQuad.rt.colors = color;
	mQuad.rb.colors = color;
}

/**
*	\brief 设置为默认的精灵正常着色器状态,包括着色器，变换矩阵
*/
void Sprite::SetDefaultNormalState()
{
	mProgramState =  ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_SPRITE_NORMAL_PROGRAMSTATE_NAME);
	mModelView = Renderer::GetInstance().GetCameraMatrix();
}


/**
*	\brief 设置为默认的精灵仅包含颜色着色器状态,包括着色器，变换矩阵
*/
void Sprite::SetDefaultColorProgramState()
{
	// 着色器优化，避免Link过多的着色器
	mProgramState = ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_SPRITE_COLOR_PROGRAMSTATE_NAME);
	mModelView = Renderer::GetInstance().GetCameraMatrix();
}

/**
*	\brief 设置指定的精灵着色器状态
*/
void Sprite::SetProgramState(GLProgramStatePtr programstate)
{
	mProgramState = programstate;
}

/**
*	\brief 设置当前为脏精灵，在渲染前会重新填充mquad
*/
void Sprite::SetDirty(bool dirty)
{
	mDirty = dirty;
}

/**
*	\brief 设置当前锚点
*/
void Sprite::SetAnchor(const Point2 & anchor)
{
	mAnchorX = -(float)(anchor.x) / (float)mSize.width;
	mAnchorY = -(float)(anchor.y) / (float)mSize.height;
	mAnchor = -anchor;
	SetDirty(true);
}

/**
*	\brief 设置当前锚点
*/
void Sprite::SetAnchorFloat(float x, float y)
{
	mAnchorX = x;
	mAnchorY = y;
	mAnchor = Point2((int)(-x*mSize.width),int(-y*mSize.height));
	SetDirty(true);
}

void Sprite::SetRotateAnchor(float x, float y)
{
	mRotateAnchorX = x;
	mRotateAnchorY = y;
}

/**
*	\brief 设置混合模式
*/
void Sprite::SetBlendFunc(const BlendFunc & blendFunc)
{
	mBlendFunc = blendFunc;
}

void Sprite::SetModelView(const Matrix4 & modelView)
{
	mModelView = modelView;
}

/**
*	\brief 设置透明度(alpha)
*/
void Sprite::SetOpacity(int opacity)
{
	Debug::CheckAssertion(opacity >= 0 && opacity <= 255,
		"Invalid alpha value.");

	mQuad.lt.colors.SetAlpha(opacity);
	mQuad.lb.colors.SetAlpha(opacity);
	mQuad.rb.colors.SetAlpha(opacity);
	mQuad.rt.colors.SetAlpha(opacity);

	SetDirty(true);
}

void Sprite::SetScale(float s)
{
	SetScale(s, s);
}

void Sprite::SetScaleX(float x)
{
	SetScale(x, scaleY);
}

void Sprite::SetScaleY(float y)
{
	SetScale(scaleX, y);
}

void Sprite::SetScale(float x, float y)
{
	scaleX = x;
	scaleY = y;
}

/**
*	\brief 翻转x轴
*/
void Sprite::SetFlipX(bool fliped)
{
	if (mFlipX != fliped)
	{
		mFlipX = fliped;
		Rect rect = Rect(GetPos(), GetSize());
		SetTextureRect(rect);
	}
}

/**
*	\brief 翻转y轴
*/
void Sprite::SetFlipY(bool fliped)
{
	if (mFlipY != fliped)
	{
		mFlipY = fliped;
		Rect rect = Rect(GetPos(), GetSize());
		SetTextureRect(rect);
	}
}

/**
*	\brief 设置当前sprite为延迟绘制
*/
void Sprite::SetDeferredDraw(bool deferred)
{
	if (mDeferred != deferred)
	{
		if (deferred)	{
			SetProgramState(ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_G_BUFFER_PROGRAMSTATE_NAME));
			mDeferred = true;
		}else{
			SetDefaultNormalState();
			mDeferred = false;
		}
	}
}

/**
*	\brief 返回纹理
*/
TexturePtr Sprite::GetTexture() const
{
	return mTexture;
}

/**
*	\brief 返回矩形有效区域
*/
Rect Sprite::GetRect() const
{
	return Rect();
}

/**
*	\brief 返回大小
*/
Size Sprite::GetSize() const
{
	return mSize;
}

/**
*	\brief 返回精灵（顶点）颜色
*/
Color4B Sprite::GetColor() const
{
	return mQuad.lt.colors;
}

/**
*	\brief 返回着色器状态
*/
GLProgramStatePtr Sprite::GetProgramState() const
{
	return mProgramState;
}

/**
*	\brief 返回锚点
*/
Point2 Sprite::GetAnchor() const
{
	return mAnchor;
}

/**
*	\brief 返回混合模式
*/
BlendFunc Sprite::GetBlendFunc() const
{
	return mBlendFunc;
}

/**
*	\brief 返回透明度(alpha)
*/
int Sprite::GetOpacity() const
{
	return mQuad.lt.colors.GetAlpha();
}

void Sprite::SetSuspended(bool suspended)
{
	if (suspended != IsSuspended() )
	{
		Drawable::SetSuspended(suspended);
		
		// 设置暂停了，即便闪烁也直接可见
		if (!suspended)
		{
			mBlinkNextDate = System::Now();
		}
		else
		{
			mIsBlinkVisible = true;
		}
	}
}

/**
*	\brief 是否脏精灵（是否需要更新位置）
*/
bool Sprite::IsDirty() const
{
	return mDirty;
}

/**
*	\brief 是否可见
*/
bool Sprite::IsVisible() const
{
	return mVisible;
}

/**
*	\brief 是否是动画精灵，Sprite必然返回false;
*/
bool Sprite::IsAnimationed() const
{
	return false;
}

void Sprite::AddChildSprite(std::shared_ptr<Sprite> childSprite)
{
	mChildSprites.push_back(childSprite);
}

const string Sprite::GetLuaObjectName() const
{
	return LuaContext::module_sprite_name;
}

bool Sprite::IsBlinking() const
{
	return mBlinkDelay != 0;
}

/**
*	\brief 设置闪烁效果
*	\param blinkDelay 闪烁的时间间隔,为0时关闭闪烁
*/
void Sprite::SetBlinking(uint32_t blinkDelay)
{
	mBlinkDelay = blinkDelay;
	mBlinkNextDate = System::Now() + mBlinkDelay;
}

bool Sprite::IsOutLine() const
{
	return mOutLineWidth != 0.0f;
}

/**
*	\brief 设置描边效果
*	\param outLineWidth 描边的宽度，当宽度为0时关闭描边效果
*	
*	注意当调用其他programState时，应默认关闭outlined效果
*/
void Sprite::SetOutLine(float outLineWidth)
{
	Debug::CheckAssertion(outLineWidth >= 0,
		"The outline width unable less than 0.");

	// 当设置相同值时返回
	if (fabs(outLineWidth - mOutLineWidth) <= 1e-4)
	{
		return;
	}

	if (outLineWidth > 0)
	{
		// 加载默认的outlined sprite program
		auto& resourceCache = ResourceCache::GetInstance();
		auto outLinedprogramState = std::make_shared<GLProgramState>();
		outLinedprogramState->Set(resourceCache.GetGLProgram(GLProgram::DEFAULT_SPRITE_OUTLINED_PROGRAM_NAME));
		outLinedprogramState->SetUniform1f("lineWidth", outLineWidth);
		outLinedprogramState->SetUniform3f("textureSize", { (float)GetSize().width, (float)GetSize().height, 0.0f });

		mPreProgramState = GetProgramState();
		SetProgramState(outLinedprogramState);
		mOutLineWidth = outLineWidth;
		mPreDeferred = mDeferred;
	}
	else if(outLineWidth <= 0.0f)
	{
		Debug::CheckAssertion(mPreProgramState != nullptr, 
			"The previous programState is nullptr.");
		SetProgramState(mPreProgramState);
		mOutLineWidth = outLineWidth;
		mDeferred = mPreDeferred;
	}
}

/**
*	\brief 以图片fileName初始化sprite
*/
bool Sprite::InitWithFile(const std::string & name)
{
	auto texturePtr = ResourceCache::GetInstance().LoadTexture2D(name);
	if (texturePtr == nullptr)
	{
		Debug::Error("Invalid texture name in sprite.");
		return false;
	}
	Rect rect(texturePtr->GetSize());
	return InitWithTexture(texturePtr,rect);
}

/**
*	\brief 以纹理指针初始化sprite
*/
bool Sprite::InitWithTexture(TexturePtr texture)
{
	if (texture == nullptr)
		return false;
	Rect rect(texture->GetSize());
	return InitWithTexture(texture,rect);
}

/**
*	\brief 以纹理指针初始化sprite
*	\param 纹理指针
*	\param 纹理范围的rect表示
*/
bool Sprite::InitWithTexture(TexturePtr texture, const Rect & rect)
{
	memset(&mQuad, 0, sizeof(mQuad));
	mQuad.lt.colors = Color4B::WHITE;
	mQuad.lb.colors = Color4B::WHITE;
	mQuad.rt.colors = Color4B::WHITE;
	mQuad.rb.colors = Color4B::WHITE;

	SetSize(rect.GetSize());
	SetTexture(texture);
	SetTextureRect(rect);
	SetDirty(true);

	return true;
}

/**
*	\brief 更新当前mQuad数据
*
*   一般设置为dirty时，会自动调用
*/
void Sprite::UpdateTransform()
{
	float x1 = 0 + (float)mAnchor.x;
	float y1 = 0 + (float)mAnchor.y;
	float x2 = x1 + mSize.width;	
	float y2 = y1 + mSize.height;

	mQuad.lb.vertices = Vec3f(x1, y1, .0f);
	mQuad.lt.vertices = Vec3f(x1, y2, .0f);
	mQuad.rb.vertices = Vec3f(x2, y1, .0f);
	mQuad.rt.vertices = Vec3f(x2, y2, .0f);

	SetDirty(false);
}
