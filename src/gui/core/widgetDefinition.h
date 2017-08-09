#pragma once

#include"common\common.h"

namespace gui
{

struct ResolutionDefinition
{
	// size
	int minWidth;
	int minHeight;
	int maxWidth;
	int maxHeight;
	int defaultWidth;
	int defaultHeight;
};

using ResolutionDefinitionPtr = std::shared_ptr<ResolutionDefinition>;

}