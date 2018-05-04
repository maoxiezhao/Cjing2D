#ifndef _SPRITE_H_
#define _SPRITE_H_

#include"common\common.h"
#include"core\types.h"
#include"core\quadCommand.h"
#include"core\texture.h"
#include"core\glProgramState.h"
#include"core\renderer.h"
#include"game\drawable.h"
#include"utils\rectangle.h"

/**
*	\brief 精灵,可以显示纹理的移动单元，不能展示帧动画
*
*	17.10.29 添加effect，包括blink和outLine，目前绘制以接口保存在sprite中，
*	不存在更好的拓展性，且暂时不会考虑修改拓展
*
*	18.5.3  添加像素级碰撞检测
*/
class Sprite : public Drawable
{
public:
	Sprite(const std::string& name);
	Sprite(TexturePtr tex, TexturePtr normalTex = nullptr);
	Sprite(const Color4B& color, const Size& size);
	~Sprite();

	// system
	virtual void Update();
	virtual void Draw();
	virtual void Draw(const Point2& pos);
	void Draw(const Point2& pos, float rotate);
	void MultiplyDraw();
	void MultiplyDraw(const Point2& pos, float rotate);

	// status 
	void SetVisible(bool visible);
	void SetTexture(TexturePtr texture);
	void SetTextureRect(const Rect& rect,bool bResize = false);
	void SetTextureCroods(const Rect& rect);
	void SetSize(const Size& size);
	void SetColor(const Color4B&color);
	void SetDefaultNormalState();
	void SetDefaultColorProgramState();
	void SetProgramState(GLProgramStatePtr programstate);
	void SetDirty(bool dirty);
	void SetAnchor(const Point2& anchor);
	void SetAnchorFloat(float x, float y);
	void SetRotateAnchor(float x, float y);
	void SetBlendFunc(const BlendFunc& blendFunc);
	void SetModelView(const Matrix4& modelView);
	void SetOpacity(int opacity);
	void SetScale(float s);
	void SetScaleX(float x);
	void SetScaleY(float y);
	void SetScale(float x, float y);
	void SetFlipX(bool fliped);
	void SetFlipY(bool fliped);
	void SetDeferredDraw(bool deferred);
	virtual void SetSuspended(bool suspended);

	TexturePtr GetTexture()const;
	Rect GetRect()const;
	Size GetSize()const;
	Color4B GetColor()const;
	GLProgramStatePtr GetProgramState()const;
	Point2 GetAnchor()const;
	BlendFunc GetBlendFunc()const;
	int GetOpacity()const;

	bool IsDirty()const;
	bool IsVisible()const;
	virtual bool IsAnimationed()const;

	bool IsPixelCollisionEnable()const;
	virtual void SetPixelCollisionEnable(bool enable);
	virtual bool TestCollision(Sprite& otherSprite, const Point2& srcPos, const Point2& otherPos);

	// child
	void AddChildSprite(std::shared_ptr<Sprite> childSprite);

	// lua
	virtual const string GetLuaObjectName()const;

	// effect
	bool IsBlinking()const;
	void SetBlinking(uint32_t blinkDelay);
	bool IsOutLine()const;
	void SetOutLine(float outLineWidth);

protected:
	Sprite();
	bool InitWithFile(const std::string& name);
	bool InitWithTexture(TexturePtr texture);
	bool InitWithTexture(TexturePtr texture, const Rect& rect);
	void UpdateTransform();
	void Draw(Renderer& renderer, const Matrix4& transform);
	void MultiplyDraw(Renderer& renderer, const Matrix4& transform);

protected:
	Quad mQuad;
	QuadCommand mQuadCommand;
	GLProgramStatePtr mProgramState;
	GLProgramStatePtr mPreProgramState;		/** 仅保存上一个programState */

	TexturePtr mTexture;		/** diffuse 贴图 */
	TexturePtr mNormalTexture;	/** 法线贴图 */
	Rect mTextureRect;

	Size mSize;
	Point2 mAnchor;
	BlendFunc mBlendFunc;
	Matrix4 mModelView;
	float scaleX, scaleY;
	bool mFlipX;
	bool mFlipY;
	float mAnchorX, mAnchorY;
	float mRotateAnchorX, mRotateAnchorY;

	bool mVisible;
	bool mDirty;			/** 是否需要重新填充quad */
	bool mDeferred;			/** 是否延迟渲染 */
	bool mPreDeferred;		/** 上一个programState是否是deferred的，仅仅为了配合mPreProgramState */
	bool mPixelCollisionEnable; /** 是否开启了像素级碰撞检测 */

	uint32_t mBlinkDelay;
	uint32_t mBlinkNextDate;
	bool mIsBlinkVisible;	/** 是否是处于闪烁的可见时间段*/

	float mOutLineWidth;		/** 是否开启描边 */
	Color4B mOutLineColor;		/** 描边的颜色 */

	// children
	std::vector<std::shared_ptr<Sprite>> mChildSprites;

};

using SpritePtr = std::shared_ptr<Sprite>;


#endif