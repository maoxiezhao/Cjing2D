#ifndef _SPRITE_H_
#define _SPRITE_H_

#include"common\common.h"
#include"core\types.h"
#include"core\quadCommand.h"
#include"core\texture.h"
#include"game\drawable.h"

/**
*	\brief ¾«Áé
*/
class Sprite : public Drawable
{
public:
	Sprite();
	Sprite(const std::string& name);
	virtual ~Sprite();

	bool InitWithFile(const std::string& name);
	bool InitWithTexture(TexturePtr texture);

	void Update();
	void Draw();

protected:
	Quad mQuad;
	QuadCommand mQuadCommand;
	GLProgramStatePtr mProgramState;
	TexturePtr mTexture;

};



#endif