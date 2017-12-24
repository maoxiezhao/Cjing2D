#pragma once

#include"common\common.h"
#include"utils\objectPool.h"
#include"quadCommand.h"

/**
*	\brief QuadCommand 渲染命令池
*
*	使用LRU方式管理命令，命令在这里维护和销毁
*/
class RenderCommandPool
{
public:
	static RenderCommandPool& GetInstance();

	void Initialize();
	void Flush();
	void Quit();

	void SetIsManagerCommandLRU(bool lrued);

	/** render command manager */
	QuadCommand* GetQuadCommand(float globalOrder, GLProgramStatePtr programState, GLuint textureID, 
		Quad quads, int quadCounts, const BlendFunc&blendFunc, const Matrix4& transfomr, const Matrix4& modelView);

private:
	RenderCommandPool();

	struct QuadCommandTimes
	{
		uint32_t times;
		QuadCommand* command;

		QuadCommandTimes() :times(0), command(nullptr) {}
	};

	void FlushQuadCommands();
	void ClearQuadCommands();
	
	uint32_t GenerateCommandState(GLProgramStatePtr programState, GLuint textureID,
		const Quad& quads, const BlendFunc & blendFunc)const;

private:
	bool mIsManagerCommandLRU;

	std::map<QuadCommand*, uint32_t> mQuadCommandState;
	std::map<uint32_t, QuadCommandTimes> mTimesQuadCommands;
	std::vector<QuadCommand* > mQuadCommmandToRemoves;

};