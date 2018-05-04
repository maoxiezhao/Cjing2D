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
*	\brief ����,������ʾ������ƶ���Ԫ������չʾ֡����
*
*	17.10.29 ���effect������blink��outLine��Ŀǰ�����Խӿڱ�����sprite�У�
*	�����ڸ��õ���չ�ԣ�����ʱ���ῼ���޸���չ
*
*	18.5.3  ������ؼ���ײ���
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
	GLProgramStatePtr mPreProgramState;		/** ��������һ��programState */

	TexturePtr mTexture;		/** diffuse ��ͼ */
	TexturePtr mNormalTexture;	/** ������ͼ */
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
	bool mDirty;			/** �Ƿ���Ҫ�������quad */
	bool mDeferred;			/** �Ƿ��ӳ���Ⱦ */
	bool mPreDeferred;		/** ��һ��programState�Ƿ���deferred�ģ�����Ϊ�����mPreProgramState */
	bool mPixelCollisionEnable; /** �Ƿ��������ؼ���ײ��� */

	uint32_t mBlinkDelay;
	uint32_t mBlinkNextDate;
	bool mIsBlinkVisible;	/** �Ƿ��Ǵ�����˸�Ŀɼ�ʱ���*/

	float mOutLineWidth;		/** �Ƿ������ */
	Color4B mOutLineColor;		/** ��ߵ���ɫ */

	// children
	std::vector<std::shared_ptr<Sprite>> mChildSprites;

};

using SpritePtr = std::shared_ptr<Sprite>;


#endif