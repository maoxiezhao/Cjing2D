#include "entity\animatedTilePattern.h"
#include "entity\tileset.h"
#include "core\system.h"

static constexpr uint32_t FIXED_ANIMATED_TILE_DELAY = 400;

uint32_t AnimatedTilePattern::mNextFrameData = 0;
bool AnimatedTilePattern::mFrameChanged = false;
uint32_t AnimatedTilePattern::mFrameCounter = 0;

AnimatedTilePattern::AnimatedTilePattern(const TilePatternData & data):
	TilePattern(data)
{
}

AnimatedTilePattern::AnimatedTilePattern(
		const TilePatternData & data, 
		const std::vector<Rect>& frameRect):
	TilePattern(data),
	mFrameRects(frameRect)
{
}

void AnimatedTilePattern::SetFrameRect(const std::vector<Rect>& frameRect)
{
	mFrameRects = frameRect;
}

const std::vector<Rect>& AnimatedTilePattern::GetFrameRect() const
{
	return mFrameRects;
}

Rect AnimatedTilePattern::GetCurTileRect(int index) const
{
	Debug::CheckAssertion(index < mFrameRects.size(),
				"Invalid animated tile pattern index.");
	return mFrameRects.at(index);
}

void AnimatedTilePattern::Init()
{
	mNextFrameData = System::Now();
}

/**
*	\brief 根据当前时间刷新当前帧，该刷新不受suspended影响
*/
void AnimatedTilePattern::Update()
{
	uint32_t now = System::Now();
	while (now >= mNextFrameData)
	{
		mFrameCounter++;
		mNextFrameData += FIXED_ANIMATED_TILE_DELAY;
	}
}

/**
*	\brief 绘制帧动画图块
*/
void AnimatedTilePattern::Draw(const Point2 & pos, const Size & size, const Tileset & tileset) const
{
	Debug::CheckAssertion(mFrameRects.size() > 0,
		"The Animated tile pattern' frame count > 0.");

	auto& tilesetImg = tileset.GetTileImage();
	int curFrame = mFrameCounter % mFrameRects.size();
	const Rect& rect = GetCurTileRect(curFrame);

	tilesetImg->SetTextureRect(rect);
	tilesetImg->SetPos(pos);
	tilesetImg->SetSize(size);

	tilesetImg->Draw();
}
