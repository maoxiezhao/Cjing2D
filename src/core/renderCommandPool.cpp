#include "renderCommandPool.h"
#include "utils\xxhash.h"

RenderCommandPool & RenderCommandPool::GetInstance()
{
	static RenderCommandPool renderCommandPool;
	return renderCommandPool;
}

void RenderCommandPool::Initialize()
{
}

/**
*	\brief 刷新当前状态
*
*	该函数应在render模块的flush结束时调用
*/
void RenderCommandPool::Flush()
{
	FlushQuadCommands();
}

void RenderCommandPool::Quit()
{
	ClearQuadCommands();
}

void RenderCommandPool::SetIsManagerCommandLRU(bool lrued)
{
	mIsManagerCommandLRU = lrued;
}

/**
*	\brief 获取quad命令
*
*	getQuadCommand方法根据当前是否开启isManagerCommandLRU,如果未开启则直接
*	创建命令返回，并保存以在结束时销毁，如果开启管理则存储到LRUtable中
*/
QuadCommand * RenderCommandPool::GetQuadCommand(float globalOrder, GLProgramStatePtr programState, GLuint textureID, 
		Quad quads, int quadCounts, const BlendFunc & blendFunc, 
		const Matrix4 & transfomr, const Matrix4 & modelView)
{
	QuadCommand* quadCommand = nullptr;
	if (mIsManagerCommandLRU)
	{
		// 开启管理，则首先获取command唯一hash值
		uint32_t commandState = GenerateCommandState(programState, textureID, quads, blendFunc);
	}
	else
	{
		// 未开启LRU管理，则直接创建初始化command
		auto quadCommand = new QuadCommand();
		quadCommand->Init(globalOrder, programState,
			textureID, quads, 1, blendFunc, transfomr, modelView);

		mQuadCommmandToRemoves.push_back(quadCommand);
	}
	return nullptr;
}

RenderCommandPool::RenderCommandPool():
	mIsManagerCommandLRU(false)
{
}

/**
*	\brief 刷新sprite的quadCommands表
*
*	必须保证sprite在update中仅会update一次
*/
void RenderCommandPool::FlushQuadCommands()
{
	if (mIsManagerCommandLRU)
	{
		for (auto& kvp : mTimesQuadCommands)
		{
			auto timeQuadCommand = kvp.second;
			uint32_t&times = timeQuadCommand.times;
			times--;
			if (times <= 0)
			{
				mQuadCommmandToRemoves.push_back(timeQuadCommand.command);
			}
		}
		for (auto& removedCommand : mQuadCommmandToRemoves)
		{
			auto it =  mQuadCommandState.find(removedCommand);
			Debug::CheckAssertion(it != mQuadCommandState.end(), "The Invalid command without state hash key.");

			// 依次删除时间表和hash表
			mTimesQuadCommands.erase(it->second);
			mQuadCommandState.erase(removedCommand);
			if (removedCommand != nullptr)
			{
				delete removedCommand;
			}
		}
		mQuadCommmandToRemoves.clear();
	}
	else
	{
		for (auto& removedCommand : mQuadCommmandToRemoves)
		{
			if (removedCommand != nullptr)
			{
				delete removedCommand;
			}
		}
	}
}

void RenderCommandPool::ClearQuadCommands()
{
	for (auto& kvp : mQuadCommandState)
	{
		QuadCommand* command = kvp.first;
		if (command != nullptr)
		{
			delete command;
		}
	}
	mTimesQuadCommands.clear();
	mQuadCommandState.clear();
	mQuadCommmandToRemoves.clear();
}

/**
*	\brief 生成当前command唯一状态标志
*
*   该标志与command shader state不同，还考虑到quad
*/
uint32_t RenderCommandPool::GenerateCommandState(GLProgramStatePtr programState, GLuint textureID,
				const Quad& quads, const BlendFunc & blendFunc )const
{
	/*int program = programState->GetProgram();
	int posX = quads.lt.vertices.x;
	int posY = quads.lt.vertices.y;
	int width = quads.rt.vertices.x - posX;
	int height = quads.rb.vertices.y - posY;

	int hashArray[10] = { program,(int)textureID,(int)blendFunc.srcBlend ,(int)blendFunc.dstBlend };

	uint32_t mShadeState = XXH32((const void*)hashArray, sizeof(hashArray), 0);*/

	return 0;
}