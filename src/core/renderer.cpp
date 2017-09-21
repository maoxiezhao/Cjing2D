#include"renderer.h"
#include"renderCommand.h"
#include"quadCommand.h"
#include"debug.h"
#include"renderCommandPool.h"
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
	for (auto& command : mQueueNegZ)
	{
		if (command && command->IsAutoReleased())
			delete command;
	}
	mQueueNegZ.clear();

	for (auto& command : mQueue0)
	{
		if (command && command->IsAutoReleased())
			delete command;
	}
	mQueue0.clear();
	
	for (auto& command : mQueuePosZ)
	{
		if (command && command->IsAutoReleased())
			delete command;
	}
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
void Renderer::Initialize(int w,int h)
{
	if (mInitialized)
		return;
	// 初始化渲染状态
	SetViewSize(w, h);
	InitCamearMatrix();
	InitIndices();
	InitVAOandVBO();
	mInitialized = true;
	// 初始化命令池
	RenderCommandPool::GetInstance().Initialize();
}

/**
*	\brief 初始化默认的相机变换矩阵
*
*	默认使用正交矩阵
*/
void Renderer::InitCamearMatrix()
{
	mCamearMatrix  = Matrix4::Ortho((float)mViewWidth, (float)mViewHeight, -1.0f, 1.0f);
	mCamearMatrix *= Matrix4::Translate(Vec3f(-(float)mViewWidth / 2, (float)mViewHeight / 2,0.0f));
	mCamearMatrix *= Matrix4::Scale(1.0f, -1.0f, 1.0f);
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
		mIndices[i*6+0] = (GLushort)(i * 4 + 0);
		mIndices[i*6+1] = (GLushort)(i * 4 + 1);
		mIndices[i*6+2] = (GLushort)(i * 4 + 2);
		mIndices[i*6+3] = (GLushort)(i * 4 + 2);
		mIndices[i*6+4] = (GLushort)(i * 4 + 3);
		mIndices[i*6+5] = (GLushort)(i * 4 + 0);
	}
}

/**
*	\brief 初始化VAO、VBO、VEO
*/
void Renderer::InitVAOandVBO()
{
	Debug::CheckAssertion(mVAO == 0 && mVBO == 0, "The VAO and VBO already assign.");

	glGenVertexArrays(1, &mVAO);	// VAO
	glGenBuffers(1, &mVBO);			// 顶点数据
	glGenBuffers(1, &mVEO);			// 顶点索引

	// 绑定顶点数据缓冲区
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0])*VBO_SIZE, mQuads, GL_DYNAMIC_DRAW);
	// 设置vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_4CB_2TF), (GLvoid*)offsetof(V3F_4CB_2TF, vertices));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_4CB_2TF), (GLvoid*)offsetof(V3F_4CB_2TF,colors));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_4CB_2TF), (GLvoid*)offsetof(V3F_4CB_2TF, texs));
	// 绑定顶点索引
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVEO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0])*VBO_SIZE * 6, mIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

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
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mVEO);
	mVAO = mVBO = mVEO = 0;
	mInitialized = false;

	RenderCommandPool::GetInstance().Quit();
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
		RenderCommandPool::GetInstance().Flush();
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
		RenderCommand::CommandType commandType = command->GetCommandType();
		
		if (commandType == RenderCommand::COMMAND_QUAD)
		{
			auto quadCommand = dynamic_cast<QuadCommand*>(command);
			if (mQuadsCounts + quadCommand->GetQuadCounts() > VBO_SIZE)
			{	// 当前quad数量超过最大值，则先渲染之前保存的
				Debug::Warning("The quad counts More than the maximum value");
				Flush();
			}
			memcpy(mQuads + mQuadsCounts, quadCommand->GetQuads(), sizeof(Quad)*quadCommand->GetQuadCounts());
			TransformQuadsToWorld(mQuads + mQuadsCounts, quadCommand->GetQuadCounts(),quadCommand->GetTransfomr());
	//		TransformQuadsToWorld(mQuads + mQuadsCounts, quadCommand->GetQuadCounts(), quadCommand->GetModelView());

			mQuadBatches.push_back(quadCommand);
			mQuadsCounts += quadCommand->GetQuadCounts();
		}
		else if (commandType == RenderCommand::COMMAND_BATCH)
		{
		}
		else if (commandType == RenderCommand::COMMAND_CUSTOMER)
		{
		}
		else
			Debug::Error("Unknow render command.");
	}	
}

/**
*	\brief 渲染后清扫工作
*/
void Renderer::RenderAfterClean()
{
	for (auto& queue : mRenderGroups)
		queue.Clear();
}

Matrix4 Renderer::GetCameraMatrix() const
{
	return mCamearMatrix;
}

/**
*	\brief 设置当前渲染视口大小
*/
void Renderer::SetViewSize(int w, int h)
{
	mViewWidth = w;
	mViewHeight = h;
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
	if (mQuadsCounts <= 0 || mQuadBatches.empty())
		return;
	// 重新分配VBO绑定缓冲区大小
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0])*mQuadsCounts, nullptr, GL_DYNAMIC_DRAW);
	void*buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	size_t allocSize = sizeof(mQuads[0])*mQuadsCounts;
	memcpy_s(buf, allocSize, mQuads, allocSize);
	glUnmapBuffer(GL_ARRAY_BUFFER);

	int quadToDraw = 0;
	int startQuad = 0;
	uint32_t lastQuadState = mQuadBatches[0]->GetShadeState();
	mQuadBatches[0]->UseShade();
	for (auto& command : mQuadBatches)
	{
		uint32_t currQuadState = command->GetShadeState();
		if (quadToDraw > 0 && currQuadState != lastQuadState)
		{	// 如果当前渲染状态不等，则执行渲染
			glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(mIndices[0])*startQuad * 6));
			startQuad += quadToDraw;
			quadToDraw = 0;
			lastQuadState = currQuadState;

			// 更改为新的着色状态
			command->UseShade();
		}
		quadToDraw++;	// += command->getQuadCount();

	}
	// 绘制最后的quads
	if (quadToDraw > 0)
	{
		glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(mIndices[0])*startQuad*6));
	}

	glBindVertexArray(0);
	mQuadsCounts = 0;
	mQuadBatches.clear();
}

/**
*	\brief quad顶点指定变换操作
*/
void Renderer::TransformQuadsToWorld(Quad * mQuads, int quadCount,const Matrix4 transform)
{
	for (int index = 0; index < quadCount; ++index)
	{
		transform.Transform(mQuads[index].lt.vertices);
		transform.Transform(mQuads[index].lb.vertices);
		transform.Transform(mQuads[index].rt.vertices);
		transform.Transform(mQuads[index].rb.vertices);
	}
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
