#include"renderer.h"
#include"renderCommand.h"
#include"quadCommand.h"
#include"debug.h"
#include"renderCommandPool.h"
#include"video.h"
#include<algorithm>

#define _ENABLE_DEFERRED_SHADE_

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
	mForwardQuadsCounts(0),
	mDeferredQuadsCounts(0),
	mForwardBuffer(nullptr),
	mDeferredBuffer(nullptr)
{
	mRenderGroupsStack.push(DEFAULT_RENDER_QUEUE);
	RenderQueue defaultRenderQeueue;
	mRenderGroups.push_back(defaultRenderQeueue);

	mQuadForwardBatches.reserve(QUAD_BATHCHES_RESERVE);
	mQuadDeferredBatches.reserve(QUAD_BATHCHES_RESERVE);
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
#if defined _ENABLE_DEFERRED_SHADE_
	InitGBuffer();
#endif
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
*	\brief 初始化Gbuffer，用于延迟着色(后处理)
*/
void Renderer::InitGBuffer()
{
	Debug::CheckAssertion(mGBuffer == 0, "The gBuffer already assign.");

	const Size srcSize = Video::GetScreenSize();
	// 初始化gbuffer
	glGenFramebuffers(1, &mGBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);

	// position
	glGenTextures(1, &mGPosition);
	glBindTexture(GL_TEXTURE_2D, mGPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, srcSize.width, srcSize.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGPosition, 0);

	// normal
	glGenTextures(1, &mGNormal);
	glBindTexture(GL_TEXTURE_2D, mGNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, srcSize.width, srcSize.height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, mGNormal, 0);

	// color
	glGenTextures(1, &mGColor);
	glBindTexture(GL_TEXTURE_2D, mGColor);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, srcSize.width, srcSize.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, mGColor, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::Error("[Render] Failed to gen frame buffer");
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
*	\brief 初始化VAO、VBO、VEO
*/
void Renderer::InitVAOandVBO()
{
	// 前向渲染缓冲
	mForwardBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(VBO_SIZE));
	mForwardBuffer->Initialize();

	// 延迟渲染缓冲
	mDeferredBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(VBO_SIZE));
	mDeferredBuffer->Initialize();
}

/**
*	\brief 释放函数
*/
void Renderer::Quit()
{
	if (!mInitialized)
		return;
	mRenderGroups.clear();
	mQuadForwardBatches.clear();
	mQuadDeferredBatches.clear();

	mForwardBuffer->Quit();
	mDeferredBuffer->Quit();

	mGBuffer = 0;
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
			if (quadCommand->IsDeferredShade())	// 是否重新定义为RenderCommand的派生类
			{
				if (mDeferredQuadsCounts + quadCommand->GetQuadCounts() > VBO_SIZE)
				{	// 当前quad数量超过最大值，则先渲染之前保存的
					Debug::Warning("The quad counts More than the maximum value");
					DeferredDrawQuadBatches();
				}
				Quad* quads = mDeferredBuffer->GetQuadBuffer() + mDeferredQuadsCounts;
				memcpy(quads, quadCommand->GetQuads(), sizeof(Quad)*quadCommand->GetQuadCounts());
				TransformQuadsToWorld(quads, quadCommand->GetQuadCounts(), quadCommand->GetTransfomr());

				mQuadDeferredBatches.push_back(quadCommand);
				mDeferredQuadsCounts += quadCommand->GetQuadCounts();
			}
			else
			{
				if (mForwardQuadsCounts + quadCommand->GetQuadCounts() > VBO_SIZE)
				{	
					Debug::Warning("The quad counts More than the maximum value");
					ForwardDrawQuadBatches();
				}
				Quad* quads = mForwardBuffer->GetQuadBuffer() + mForwardQuadsCounts;
				memcpy(quads, quadCommand->GetQuads(), sizeof(Quad)*quadCommand->GetQuadCounts());
				TransformQuadsToWorld(quads, quadCommand->GetQuadCounts(), quadCommand->GetTransfomr());

				mQuadForwardBatches.push_back(quadCommand);
				mForwardQuadsCounts += quadCommand->GetQuadCounts();
			}
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
*	先执行延迟着色，在执行正向着色
*/
void Renderer::Flush()
{
#if defined _ENABLE_DEFERRED_SHADE_
	DeferredDrawQuadBatches();
#endif
	ForwardDrawQuadBatches();
}

namespace
{
	/**
	*	\brief 绘制QuadBatches
	*/
	void DrawQuadBatches(VertexBuffer& vertexBuffer, int quadCounts, const std::vector<QuadCommand*>& quadBatches)
	{
		vertexBuffer.BeginDraw(quadCounts);

		GLushort* indices = vertexBuffer.GetIndices();
		int quadToDraw = 0;
		int startQuad = 0;
		uint32_t lastQuadState = quadBatches[0]->GetShadeState();
		quadBatches[0]->UseShade();
		for (auto& command : quadBatches)
		{
			uint32_t currQuadState = command->GetShadeState();
			if (quadToDraw > 0 && currQuadState != lastQuadState)
			{	// 如果当前渲染状态不等，则执行渲染
				glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(indices[0])*startQuad * 6));
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
			glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(indices[0])*startQuad * 6));
		}

		vertexBuffer.EndDraw();
	}
}

/**
*	\brief 遍历mQuadBatches对其执行延迟批绘制处理
*/
void Renderer::DeferredDrawQuadBatches()
{
	if (mDeferredQuadsCounts <= 0 || mQuadDeferredBatches.empty())
		return;
	// geometry pass
	glBindFramebuffer(GL_FRAMEBUFFER, mGBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawQuadBatches(*mDeferredBuffer, mDeferredQuadsCounts, mQuadDeferredBatches);
	mDeferredQuadsCounts = 0;
	mQuadDeferredBatches.clear();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// lighting pass
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGColor);

	// 遍历光源信息
	FlushAllLights();
	
	// 后处理渲染，在片段着色器处理GBUFFER
	PostRenderQuad();
}

/**
*	\brief 遍历mQuadBatches对其执行正向批绘制处理    
*/
void Renderer::ForwardDrawQuadBatches()
{
	if (mForwardQuadsCounts <= 0 || mQuadForwardBatches.empty())
		return;

	DrawQuadBatches(*mForwardBuffer, mForwardQuadsCounts, mQuadForwardBatches);

	mForwardQuadsCounts = 0;
	mQuadForwardBatches.clear();
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

/**
*	\brief 后处理渲染
*
*	当开启延迟着色时的最后阶段调用
*/
void Renderer::PostRenderQuad()
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

/**
*	\brief 添加光源信息
*/
void Renderer::PushLight()
{
}

/**
*	\brief 遍历所有光源，并传入到着色器中
*/
void Renderer::FlushAllLights()
{
}
