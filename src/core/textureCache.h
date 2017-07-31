#ifndef _TEXTURE_CACHE_H_
#define _TEXTURE_CACHE_H_

#include"common\common.h"
#include"core\texture.h"

class TextureCache
{
public:
	

	TexturePtr LoadTexture2D(const string& texname);
	TexturePtr GetTexture2D(const string& texnname);
};


#endif