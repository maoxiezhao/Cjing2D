#ifndef _ANIMATION_DATA_H_
#define _ANIMATION_DATA_H_

#include"common\common.h"
#include"lua\luaData.h"
#include"utils\point.h"
#include"utils\size.h"
#include"utils\rectangle.h"
#include<deque>
#include<map>

/**
*	\brief 精灵方向信息
*/
class AnimationDirectionData
{
public:
	AnimationDirectionData(const Point2& pos, const Point2&orgin, const Size& size, int numFrames, int numCloumn) :
		mPos(pos), mOrgin(orgin), mSize(size), mNumFrames(numFrames), mNumColumn(numCloumn) {};

	Point2 GetPos()const { return mPos; }
	Size GetSize()const { return mSize; }
	int GetNumFrames()const { return mNumFrames; }
	int GetNumColumn()const { return mNumColumn; }
	Rect GetBoundingBox()const { return Rect(-mOrgin, mSize); }

private:
	Point2 mPos;
	Point2 mOrgin;
	Size   mSize;
	int    mNumFrames;
	int    mNumColumn;
};

/**
*	\brief 精灵动画数据，存储精灵帧动画信息包括时间，间隔，方向信息序列
*/
class AnimationData 
{
public:
	AnimationData();
	AnimationData(const string&imageName, const std::deque<AnimationDirectionData>& directions, uint32_t frameDelay, int frameLoop);

	string GetImageName()const;
	uint32_t GetFrameDelay()const;
	int  GetFrameLoop()const;
	const std::deque<AnimationDirectionData>& GetDirections()const;

private:
	string mImageName;
	std::deque<AnimationDirectionData> mDirections;
	uint32_t mFrameDelay;
	int mFrameLoop;

};

/**
*	\brief 精灵动画数据Lua文件,用于从Lua格式文件中解析数据
*/
class AnimationLuaData : public LuaData
{
public:
	AnimationLuaData();

	const std::map<string, AnimationData>& GetAnimations()const;
	string GetDefaultName()const;

	virtual bool ImportFromLua(lua_State*l);
	std::map<string, AnimationData> mAnimations;
	string mDefaultName;

private:
	static int LuaAnimation(lua_State* l);

};


#endif