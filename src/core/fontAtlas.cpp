#include"fontAtlas.h"

namespace font{

FontConfig::FontConfig() :
	name(""),
	fontData(nullptr),
	fontDataSize(0),
	GlyphOffset(0, 0),
	sizePixels(0),
	merageMode(0)
{
}

}