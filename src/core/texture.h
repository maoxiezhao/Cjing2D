#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include"common\common.h"

class Texture
{
public:
	Texture();
	~Texture();
};

using TexturePtr = std::shared_ptr<Texture>;

#endif