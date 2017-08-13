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
*/
class Sprite : public Drawable
{
public:
	Sprite(const std::string& name);
	Sprite(TexturePtr& tex);
	Sprite(const Color4B& color, const Size& size);
	~Sprite();

	// system
	virtual void Update();
	virtual void Draw();
	void Draw(const Point2& pos, float rotate);

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
	void SetBlendFunc(const BlendFunc& blendFunc);
	void SetModelView(const Matrix4& modelView);
	void SetOpacity(int opacity);

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

	// child
	void AddChildSprite(std::shared_ptr<Sprite> childSprite);

	// lua
	virtual const string GetLuaObjectName()const;

protected:
	Sprite();
	bool InitWithFile(const std::string& name);
	bool InitWithTexture(TexturePtr texture);
	bool InitWithTexture(TexturePtr texture, const Rect& rect);
	void UpdateTransform();
	void Draw(Renderer& renderer, const Matrix4& transform);

protected:
	Quad mQuad;
	QuadCommand mQuadCommand;
	GLProgramStatePtr mProgramState;
	TexturePtr mTexture;
	Rect mTextureRect;
	Size mSize;
	Point2 mAnchor;
	BlendFunc mBlendFunc;
	Matrix4 mModelView;

	bool mVisible;
	bool mDirty;

	// children
	std::vector<std::shared_ptr<Sprite>> mChildSprites;
	bool mSuspended;

};

using SpritePtr = std::shared_ptr<Sprite>;


#endif