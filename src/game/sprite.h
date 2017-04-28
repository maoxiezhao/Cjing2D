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
*	\brief ¾«Áé
*/
class Sprite : public Drawable
{
public:
	Sprite(const std::string& name);
	Sprite(TexturePtr& tex);
	Sprite(const Color4B& color, const Size& size);
	virtual ~Sprite();

	// system
	virtual void Update();
	virtual void Draw();
	virtual void Draw(const Point2& pos, float rotate);
	void Draw(Renderer& renderer, const Matrix4& transform);

	// status
	void SetVisible(bool visible);
	void SetTexture(TexturePtr texture);
	void SetTextureRect(const Rect& rect,bool bResize = false);
	void SetTextureCroods(const Rect& rect);
	void SetSize(const Size& size);
	void SetColor(const Color4B&color);
	void SetDefaultProgramState();
	void SetProgramState(GLProgramStatePtr programstate);
	void SetDirty(bool dirty);
	void SetAnchor(const Point2& anchor);
	void SetBlendFunc(const BlendFunc& blendFunc);

	TexturePtr GetTexture()const;
	Rect GetRect()const;
	Size GetSize()const;
	Color4B GetColor()const;
	GLProgramStatePtr GetProgramState()const;
	Point2 GetAnchor()const;
	BlendFunc GetBlendFunc()const;

	bool IsDirty()const;
	bool IsVisible()const;

private:
	bool InitWithFile(const std::string& name);
	bool InitWithTexture(TexturePtr texture);
	bool InitWithTexture(TexturePtr texture, const Rect& rect);
	
	void UpdateTransform();

protected:
	Quad mQuad;
	QuadCommand mQuadCommand;
	GLProgramStatePtr mProgramState;
	TexturePtr mTexture;
	Rect mTextureRect;
	Size mSize;
	Point2 mAnchor;
	BlendFunc mBlendFunc;

	bool mVisible;
	bool mDirty;

};



#endif