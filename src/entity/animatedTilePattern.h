#pragma once

#include"entity\tilepattern.h"

/**
*	\brief Ö¡¶¯»­µÄµØÍ¼¿é
*/
class AnimatedTilePattern : public TilePattern
{
public:
	AnimatedTilePattern(const TilePatternData& data);
	AnimatedTilePattern(const TilePatternData& data, const std::vector<Rect>& frameRect);

	virtual void Init();
	static  void Update();

	virtual Rect GetTextureRect()const;

	void SetFrameRect(const std::vector<Rect>& frameRect);
	const std::vector<Rect>& GetFrameRect()const;

	Rect GetCurTileRect(int index)const;

private:
	// frame control
	std::vector<Rect> mFrameRects;

	static uint32_t mNextFrameData;
	static bool mFrameChanged;
	static uint32_t mFrameCounter;

};