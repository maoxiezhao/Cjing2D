#include"renderer.h"
#include"renderCommand.h"
#include"debug.h"
#include<algorithm>

namespace
{
	bool CompareRenderCommand(RenderCommand* command1, RenderCommand* command2)
	{
		return command1->GetGlobalOrder() < command2->GetGlobalOrder();
	}
	
	const int DEFAULT_RENDER_QUEUE = 0;
	const int QUAD_BATHCHES_RESERVE = 64;
}

/**
*	\brief 在任务队列中添加RenderCommand
*
*	根据command的globalOrder决定其放置于哪一队列
*/
void RenderQueue::PushCommand(RenderCommand * command)
{
	float globalOrder = command->GetGlobalOrder();
	if (globalOrder < 0.0f)
		mQueueNegZ.push_back(command);
	else if (globalOrder > 0.0f)
		mQueuePosZ.push_back(command);
	else
		mQueue0.push_back(command);
}

size_t RenderQueue::Size() const
{
	return mQueue0.size() + mQueueNegZ.size() + mQueuePosZ.size();
}

/**
*	\brief 根据RenderCommand的globalOrder进行排序
*/
void RenderQueue::Sort()
{
	std::sort(mQueueNegZ.begin(), mQueueNegZ.end(), CompareRenderCommand);
	std::sort(mQueuePosZ.begin(), mQueuePosZ.end(), CompareRenderCommand);
}

void RenderQueue::Clear()
{
	mQueuePosZ.clear();
	mQueue0.clear();
	mQueuePosZ.clear();
}

RenderCommand * RenderQueue::operator[](size_t index)const
{
	if (index < mQueueNegZ.size())
		return mQueueNegZ[index];
	index -= mQueueNegZ.size();
	if (index <  mQueue0.size())
		return mQueue0[index];
	index -= mQueue0.size();
	if(index  < mQueuePosZ.size())
		return mQueuePosZ[index ];
	throw std::range_error("Ivaild: the index in render queue.");
	return nullptr;
}


/**
*	\brief 获取唯一实例
*/
Renderer& Renderer::GetInstance()
{
	static Renderer instance;
	return instance;
}

Renderer::Renderer() :
	mInitialized(false),
	mVAO(0),
	mVBO(0),
	mQuadsCounts(0)
{
	mRenderGroupsStack.push(DEFAULT_RENDER_QUEUE);
	RenderQueue defaultRenderQeueue;
	mRenderGroups.push_back(defaultRenderQeueue);
	mQuadBatches.reserve(QUAD_BATHCHES_RESERVE);
}

Renderer::~Renderer()
{
	Quit();
}

/**
*	\brief 初始化函数
*
*   初始化GLProgram,VAO,VBO
*/
void Renderer::Initialize()
{
	if (mInitialized)
		return;
	InitDefaultProgram();
	InitIndices();
	//InitVAOandVBO();
	mInitialized = true;
}

/**
*	\brief 初始化默认着色器程序
*/
void Renderer::InitDefaultProgram()
{
}

/**
*	\brief 初始化indices
*/
void Renderer::InitIndices()
{
	for (int i = 0; i < VBO_SIZE; ++i)
	{
		mIndices[i]   = (GLushort)(i * 4 + 0);
		mIndices[i+1] = (GLushort)(i * 4 + 1);
		mIndices[i+2] = (GLushort)(i * 4 + 2);
		mIndices[i+3] = (GLushort)(i * 4 + 2);
		mIndices[i+4] = (GLushort)(i * 4 + 3);
		mIndices[i+5] = (GLushort)(i * 4 + 0);
	}
}

/**
*	\brief 初始化VAO 和 VBO
*/
void Renderer::InitVAOandVBO()
{
	Debug::CheckAssertion(mVAO == 0 && mVBO == 0, "The VAO and VBO already assign.");
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBindVertexArray(0);
}

/**
*	\brief 释放函数
*/
void Renderer::Quit()
{
	if (!mInitialized)
		return;
	mRenderGroups.clear();
	mQuadBatches.clear();
	mVAO = mVBO = 0;
	mInitialized = false;
}

/**
*	\brief 清除当前视口
*
*	默认使用黑色清屏
*/
void Renderer::RenderClear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
*	\brief 执行绘制
*/
void Renderer::Render()
{
	mIsRenderer = true;
	if (IsInitialized())
	{
		for (auto& queue : mRenderGroups)
			queue.Sort();
		VisitRenderQueue(mRenderGroups[0]);
		Flush();
	}
	RenderAfterClean();
	mIsRenderer = false;
}

/**
*	\brief 对commandqueue 执行深度遍历
*/
void Renderer::VisitRenderQueue(const RenderQueue & queue)
{
	size_t queueSize = queue.Size();
	for (size_t queueIndex = 0; queueIndex < queueSize; ++queueIndex)
	{
		const auto& command = queue[queueIndex];
		switch (command->GetCommandType())
		{
		case RenderCommand::COMMAND_QUAD:

			break;
		case RenderCommand::COMMAND_CUSTOMER:
			break;
		case RenderCommand::COMMAND_BATCH:
			break;
		default:
			Debug::Error("Unknow render command.");
			break;
		}
	}
		
}

/**
*	\brief 渲染后清扫工作
*/
void Renderer::RenderAfterClean()
{
}

/**
*	\brief 执行批绘制任务
*
*	在VisitRenderQueue之后已经得到全部的quanCommand放置于
*	mQuadBathces中，调用DrawQuadBathches，执行真正渲染功能
*/
void Renderer::Flush()
{
	DrawQuadBatches();
}

/**
*	\brief 遍历mQuadBatches对其执行批绘制处理    
*/
void Renderer::DrawQuadBatches()
{
	
}

bool Renderer::IsInitialized()
{
	return mInitialized;
}

/**
*	\brief 将RenderCommand添加到最上层
*/
void Renderer::PushCommand(RenderCommand * command)
{
	int groupIndex = mRenderGroupsStack.top();
	PushCommand(command, groupIndex);
}

/**
*	\brief 将RenderCommand添加到任意层
*/
void Renderer::PushCommand(RenderCommand * command, int groupIndex)
{
	Debug::CheckAssertion(groupIndex >= 0, "Invaild:render group index.");
	Debug::CheckAssertion(command->GetCommandType() != RenderCommand::COMMAND_UNKNOW, "Invalild: Unknow render command.");
	mRenderGroups[groupIndex].PushCommand(command);
}
