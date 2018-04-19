#ifndef _ANIMATION_SPRITE_H_
#define _ANIMATION_SPRITE_H_

#include"common\common.h"
#include"game\sprite.h"
#include"game\animationSet.h"
#include"lua\luaRef.h"
#include<map>

/**
*	\brief �������飬���Լ��ض����ļ������ž���֡����
*/
class AnimationSprite : public Sprite
{
public:
	//AnimationSprite();
	AnimationSprite(const string& id);
	~AnimationSprite();

	// system
	virtual void Update();
	virtual void Draw();
	virtual void Draw(const Point2& pos);
	virtual bool IsAnimationed()const;
	
	// frame status
	bool IsFrameChanged()const;
	void SetFrameChanged(bool changed);
	void SetCurrFrame(int currFrame);
	int  GetCurrFrame()const;
	void SetFrameFinished(bool finished);
	bool IsFrameFinished()const;
	bool IsFrameStarted()const ;
	void SetFrameDelay(uint32_t delay);
	void SetPaused(bool paused);
	uint32_t GetFrameDelay()const;
	int GetNextFrame()const;
	int GetNumFrames()const;
	void SetSize(const Size& size);// size��������
	Size GetSize()const;
	void SetFlipX(bool fliped);	 // overlaps
	void SetFlipY(bool fliped);

	virtual void SetSuspended(bool suspended);

	// direction
	int GetCurrDirection()const;
	int GetNumDirections()const;
	void SetCurrDirection(int direction);

	// animation status
	void ResetAnimation();
	void StartAnimation();
	void StopAnimation();

	void SetCurrAnimation(const string& name);
	void SetCurrAnimationSetId(const string& id);
	string GetCurrAnimationSetId()const;
	bool HasAnimation(const string& name)const;
	std::string GetCurAnimationName()const;

	// notify
	void NotifyFinished();

	// lua
	virtual const string GetLuaObjectName()const;

private:
	void SetDataFromAnimation(const Animation& animation);

	AnimationSet& GetAnimationSet(const string& id);
	void UpdateFramedChanged();

	std::map<string, AnimationSet*> mAllAnimationSets;
	AnimationSet& mCurrAnimationSet;

	string mCurrAnimationName;
	Animation* mCurrAnimation;	// �������mCurrAnimationSet�е�animationָ�룬�����������

	Size     mSize;
	uint32_t mFrameDelay;
	uint32_t mNextFrameDate;
	int      mFrameLoop;
	int      mCurrFrame;
	int      mFrameNum;
	int      mCurrDirection;
	bool     mFrameChanged;
	bool     mFrameFinished;
	bool     mPaused;			// ��suspended��ͬ���Ƕ����Ƿ񲥷ŵı��
};

using AnimationSpritePtr = std::shared_ptr<AnimationSprite>;

#endif