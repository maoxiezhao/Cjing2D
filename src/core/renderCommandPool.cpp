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
*	\brief ˢ�µ�ǰ״̬
*
*	�ú���Ӧ��renderģ���flush����ʱ����
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
*	\brief ��ȡquad����
*
*	getQuadCommand�������ݵ�ǰ�Ƿ���isManagerCommandLRU,���δ������ֱ��
*	��������أ����������ڽ���ʱ���٣��������������洢��LRUtable��
*/
QuadCommand * RenderCommandPool::GetQuadCommand(float globalOrder, GLProgramStatePtr programState, GLuint textureID, 
		Quad quads, int quadCounts, const BlendFunc & blendFunc, 
		const Matrix4 & transfomr, const Matrix4 & modelView)
{
	QuadCommand* quadCommand = nullptr;
	if (mIsManagerCommandLRU)
	{
		// �������������Ȼ�ȡcommandΨһhashֵ
		uint32_t commandState = GenerateCommandState(programState, textureID, quads, blendFunc);
	}
	else
	{
		// δ����LRU������ֱ�Ӵ�����ʼ��command
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
*	\brief ˢ��sprite��quadCommands��
*
*	���뱣֤sprite��update�н���updateһ��
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

			// ����ɾ��ʱ����hash��
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
*	\brief ���ɵ�ǰcommandΨһ״̬��־
*
*   �ñ�־��command shader state��ͬ�������ǵ�quad
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