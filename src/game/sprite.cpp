#include"sprite.h"
#include"core\texture.h"
#include"core\debug.h"
#include"core\resourceCache.h"
#include"core\system.h"
#include"lua\luaContext.h"
#include"utils\size.h"

/**
*	\brief ����һ��Ĭ�ϵľ���
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
*	\brief ����һ����������ľ���ʵ��
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
*	\brief ����һ����������ľ���ʵ��
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
*	\brief ����һ����ɫ��ʹ������ľ���ʵ��
*	\param �������ɫ
*	\param ����Ĵ�С
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
*	\brief ��������״̬ͬʱ��������children
*/
void Sprite::Update()
{
	Drawable::Update();

	if (IsSuspended())
	{
		return;
	}

	// ����blinking
	if (IsBlinking())
	{
		if (System::Now() >= mBlinkNextDate)
		{
			mIsBlinkVisible = !mIsBlinkVisible;
			mBlinkNextDate += mBlinkDelay;
		}
	}

	// ����child sprite
	for (auto& sprite : mChildSprites)
		sprite->Update();
}

/**
*	\brief ���ƾ���
*
*   �Ծ��鱣���λ�ú���ת�ǶȻ��ƾ���
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
*	\brief ���ƾ���
*
*   ���Զ����λ�û��ƾ��飬�ǶȻ�����sprite��Ϊ��
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
*	\brief ��ָ��λ�ú�ָ���ǶȻ��ƾ���
*
*   ����λ�úͽǶ����ɶ�Ӧ�ı任����
*/
void Sprite::Draw(const Point2 & pos, float rotate)
{
	Matrix4 transfomr;
	// ��Ϊ���ҳ����꣬���в���˳��Ϊ��������
	// �ƶ���ָ��λ��
	transfomr =  Matrix4::Translate(Vec3f((float)pos.x,(float)pos.y, .0f));

	// �ƶ���ԭ����ת���ƻ�
	// ��ת��ê����ʱʹ�ú�λ��һ�µ� 
	//transfomr *= Matrix4::Translate(Vec3f(mSize.width*0.5f, mSize.height*0.5f, 0.0f));
	//transfomr *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
	//transfomr *= Matrix4::Translate(Vec3f(-mSize.width*0.5f, -mSize.height*0.5f, 0.0f));

	// ����������תê��
	transfomr *= Matrix4::Translate(Vec3f(mRotateAnchorX, mRotateAnchorY, 0.0f));
	transfomr *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
	transfomr *= Matrix4::Translate(Vec3f(-mRotateAnchorX, -mRotateAnchorY, 0.0f));

	Draw(Renderer::GetInstance(), transfomr);
}

/**
*	\brief ʵ�ֶ�λ���
*
*	��Ϊ����Ƶ����renderCommand�Ĵ����ͷţ����Դ���
*	��������
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
	// ��Ϊ���ҳ����꣬���в���˳��Ϊ��������
	// �ƶ���ָ��λ��
	transfomr = Matrix4::Translate(Vec3f((float)pos.x, (float)pos.y, .0f));

	// �ƶ���ԭ����ת���ƻ�
	transfomr *= Matrix4::Translate(Vec3f(mSize.width*0.5f, mSize.height*0.5f, 0.0f));
	transfomr *= Matrix4::Rotate(Vec3f(0.0f, 0.0f, 1.0f), rotate);
	transfomr *= Matrix4::Translate(Vec3f(-mSize.width*0.5f, -mSize.height*0.5f, 0.0f));

	MultiplyDraw(Renderer::GetInstance(), transfomr);
}

/**
*	\brief ��renderer�ύ��Ⱦ����
*/
void Sprite::Draw(Renderer & renderer, const Matrix4 & transform)
{
	Debug::CheckAssertion(mProgramState != nullptr, "Invaild programState in Sprite::Draw().");

	mQuadCommand.Init(GetGlobalOrder(),mProgramState,
		mTexture != nullptr ? mTexture->GetTextureID(): 0,	// ������Ҫ����������ɫ��
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
		mTexture != nullptr ? mTexture->GetTextureID() : 0,	// ������Ҫ����������ɫ��
		mNormalTexture != nullptr ? mNormalTexture->GetTextureID() : 0,
		mQuad, 1, mBlendFunc, transform, mModelView, true);
	quadCommand->SetDeferredShade(mDeferred);
	renderer.PushCommand(quadCommand);
}

/**
*	\brief ���øþ����Ƿ�ɼ�
*/
void Sprite::SetVisible(bool visible)
{
	mVisible = visible;
}

/**
*	\brief ���õ�ǰ����
*
*	ֻ�Ǹ�����ǰtexurePtr,�����Ҫ���»������Ĵ�С
*   ʹ��InitWithTexture
*/
void Sprite::SetTexture(TexturePtr texture)
{
	mTexture = texture;
}

/**
*	\brief �����������
*	\param �������
*	\param �Ƿ���������С,Ĭ�ϲ�����
*/
void Sprite::SetTextureRect(const Rect & rect,bool bResize)
{
	SetTextureCroods(rect);
	if (bResize)
		SetSize(rect.GetSize());
}

/**
*	\brief ������������
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
*	\brief ���þ����С
*/
void Sprite::SetSize(const Size & size)
{
	mSize = size;
	SetAnchorFloat(mAnchorX, mAnchorY);
	SetDirty(true);
}

/**
*	\brief ���þ��鶥����ɫ
*
*   ��ʹ���������ɫ��ʱ��ֱ��Ӱ����ʾ����ɫ
*/
void Sprite::SetColor(const Color4B & color)
{
	mQuad.lt.colors = color;
	mQuad.lb.colors = color;
	mQuad.rt.colors = color;
	mQuad.rb.colors = color;
}

/**
*	\brief ����ΪĬ�ϵľ���������ɫ��״̬,������ɫ�����任����
*/
void Sprite::SetDefaultNormalState()
{
	mProgramState =  ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_SPRITE_NORMAL_PROGRAMSTATE_NAME);
	mModelView = Renderer::GetInstance().GetCameraMatrix();
}


/**
*	\brief ����ΪĬ�ϵľ����������ɫ��ɫ��״̬,������ɫ�����任����
*/
void Sprite::SetDefaultColorProgramState()
{
	// ��ɫ���Ż�������Link�������ɫ��
	mProgramState = ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_SPRITE_COLOR_PROGRAMSTATE_NAME);
	mModelView = Renderer::GetInstance().GetCameraMatrix();
}

/**
*	\brief ����ָ���ľ�����ɫ��״̬
*/
void Sprite::SetProgramState(GLProgramStatePtr programstate)
{
	mProgramState = programstate;
}

/**
*	\brief ���õ�ǰΪ�ྫ�飬����Ⱦǰ���������mquad
*/
void Sprite::SetDirty(bool dirty)
{
	mDirty = dirty;
}

/**
*	\brief ���õ�ǰê��
*/
void Sprite::SetAnchor(const Point2 & anchor)
{
	mAnchorX = -(float)(anchor.x) / (float)mSize.width;
	mAnchorY = -(float)(anchor.y) / (float)mSize.height;
	mAnchor = -anchor;
	SetDirty(true);
}

/**
*	\brief ���õ�ǰê��
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
*	\brief ���û��ģʽ
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
*	\brief ����͸����(alpha)
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
*	\brief ��תx��
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
*	\brief ��תy��
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
*	\brief ���õ�ǰspriteΪ�ӳٻ���
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
*	\brief ��������
*/
TexturePtr Sprite::GetTexture() const
{
	return mTexture;
}

/**
*	\brief ���ؾ�����Ч����
*/
Rect Sprite::GetRect() const
{
	return Rect();
}

/**
*	\brief ���ش�С
*/
Size Sprite::GetSize() const
{
	return mSize;
}

/**
*	\brief ���ؾ��飨���㣩��ɫ
*/
Color4B Sprite::GetColor() const
{
	return mQuad.lt.colors;
}

/**
*	\brief ������ɫ��״̬
*/
GLProgramStatePtr Sprite::GetProgramState() const
{
	return mProgramState;
}

/**
*	\brief ����ê��
*/
Point2 Sprite::GetAnchor() const
{
	return mAnchor;
}

/**
*	\brief ���ػ��ģʽ
*/
BlendFunc Sprite::GetBlendFunc() const
{
	return mBlendFunc;
}

/**
*	\brief ����͸����(alpha)
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
		
		// ������ͣ�ˣ�������˸Ҳֱ�ӿɼ�
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
*	\brief �Ƿ��ྫ�飨�Ƿ���Ҫ����λ�ã�
*/
bool Sprite::IsDirty() const
{
	return mDirty;
}

/**
*	\brief �Ƿ�ɼ�
*/
bool Sprite::IsVisible() const
{
	return mVisible;
}

/**
*	\brief �Ƿ��Ƕ������飬Sprite��Ȼ����false;
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
*	\brief ������˸Ч��
*	\param blinkDelay ��˸��ʱ����,Ϊ0ʱ�ر���˸
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
*	\brief �������Ч��
*	\param outLineWidth ��ߵĿ�ȣ������Ϊ0ʱ�ر����Ч��
*	
*	ע�⵱��������programStateʱ��ӦĬ�Ϲر�outlinedЧ��
*/
void Sprite::SetOutLine(float outLineWidth)
{
	Debug::CheckAssertion(outLineWidth >= 0,
		"The outline width unable less than 0.");

	// ��������ֵͬʱ����
	if (fabs(outLineWidth - mOutLineWidth) <= 1e-4)
	{
		return;
	}

	if (outLineWidth > 0)
	{
		// ����Ĭ�ϵ�outlined sprite program
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
*	\brief ��ͼƬfileName��ʼ��sprite
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
*	\brief ������ָ���ʼ��sprite
*/
bool Sprite::InitWithTexture(TexturePtr texture)
{
	if (texture == nullptr)
		return false;
	Rect rect(texture->GetSize());
	return InitWithTexture(texture,rect);
}

/**
*	\brief ������ָ���ʼ��sprite
*	\param ����ָ��
*	\param ����Χ��rect��ʾ
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
*	\brief ���µ�ǰmQuad����
*
*   һ������Ϊdirtyʱ�����Զ�����
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
