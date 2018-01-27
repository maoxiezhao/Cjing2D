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
*	\brief ��������������RenderCommand
*
*	����command��globalOrder�������������һ����
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
*	\brief ����RenderCommand��globalOrder��������
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
*	\brief ��ȡΨһʵ��
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
*	\brief ��ʼ������
*
*   ��ʼ��GLProgram,VAO,VBO
*/
void Renderer::Initialize(int w,int h)
{
	if (mInitialized)
		return;
	// ��ʼ����Ⱦ״̬
	SetViewSize(w, h);
	InitCamearMatrix();
#if defined _ENABLE_DEFERRED_SHADE_
	InitGBuffer();
#endif
	InitVAOandVBO();
	mInitialized = true;
	// ��ʼ�������
	RenderCommandPool::GetInstance().Initialize();
}

/**
*	\brief ��ʼ��Ĭ�ϵ�����任����
*
*	Ĭ��ʹ����������
*/
void Renderer::InitCamearMatrix()
{
	mCamearMatrix  = Matrix4::Ortho((float)mViewWidth, (float)mViewHeight, -1.0f, 1.0f);
	mCamearMatrix *= Matrix4::Translate(Vec3f(-(float)mViewWidth / 2, (float)mViewHeight / 2,0.0f));
	mCamearMatrix *= Matrix4::Scale(1.0f, -1.0f, 1.0f);
}

/**
*	\brief ��ʼ��Ĭ����ɫ������
*/
void Renderer::InitDefaultProgram()
{
}

/**
*	\brief ��ʼ��Gbuffer�������ӳ���ɫ(����)
*/
void Renderer::InitGBuffer()
{
	Debug::CheckAssertion(mGBuffer == 0, "The gBuffer already assign.");

	const Size srcSize = Video::GetScreenSize();
	// ��ʼ��gbuffer
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
*	\brief ��ʼ��VAO��VBO��VEO
*/
void Renderer::InitVAOandVBO()
{
	// ǰ����Ⱦ����
	mForwardBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(VBO_SIZE));
	mForwardBuffer->Initialize();

	// �ӳ���Ⱦ����
	mDeferredBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(VBO_SIZE));
	mDeferredBuffer->Initialize();
}

/**
*	\brief �ͷź���
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
*	\brief �����ǰ�ӿ�
*
*	Ĭ��ʹ�ú�ɫ����
*/
void Renderer::RenderClear()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/**
*	\brief ִ�л���
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
*	\brief ��commandqueue ִ����ȱ���
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
			if (quadCommand->IsDeferredShade())	// �Ƿ����¶���ΪRenderCommand��������
			{
				if (mDeferredQuadsCounts + quadCommand->GetQuadCounts() > VBO_SIZE)
				{	// ��ǰquad�����������ֵ��������Ⱦ֮ǰ�����
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
*	\brief ��Ⱦ����ɨ����
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
*	\brief ���õ�ǰ��Ⱦ�ӿڴ�С
*/
void Renderer::SetViewSize(int w, int h)
{
	mViewWidth = w;
	mViewHeight = h;
}

/**
*	\brief ִ������������
*
*	��VisitRenderQueue֮���Ѿ��õ�ȫ����quanCommand������
*	mQuadBathces�У�����DrawQuadBathches��ִ��������Ⱦ����
*	��ִ���ӳ���ɫ����ִ��������ɫ
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
	*	\brief ����QuadBatches
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
			{	// �����ǰ��Ⱦ״̬���ȣ���ִ����Ⱦ
				glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(indices[0])*startQuad * 6));
				startQuad += quadToDraw;
				quadToDraw = 0;
				lastQuadState = currQuadState;

				// ����Ϊ�µ���ɫ״̬
				command->UseShade();
			}
			quadToDraw++;	// += command->getQuadCount();

		}
		// ��������quads
		if (quadToDraw > 0)
		{
			glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(indices[0])*startQuad * 6));
		}

		vertexBuffer.EndDraw();
	}
}

/**
*	\brief ����mQuadBatches����ִ���ӳ������ƴ���
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

	// ������Դ��Ϣ
	FlushAllLights();
	
	// ������Ⱦ����Ƭ����ɫ������GBUFFER
	PostRenderQuad();
}

/**
*	\brief ����mQuadBatches����ִ�����������ƴ���    
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
*	\brief quad����ָ���任����
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
*	\brief ������Ⱦ
*
*	�������ӳ���ɫʱ�����׶ε���
*/
void Renderer::PostRenderQuad()
{
}

bool Renderer::IsInitialized()
{
	return mInitialized;
}

/**
*	\brief ��RenderCommand��ӵ����ϲ�
*/
void Renderer::PushCommand(RenderCommand * command)
{
	int groupIndex = mRenderGroupsStack.top();
	PushCommand(command, groupIndex);
}

/**
*	\brief ��RenderCommand��ӵ������
*/
void Renderer::PushCommand(RenderCommand * command, int groupIndex)
{
	Debug::CheckAssertion(groupIndex >= 0, "Invaild:render group index.");
	Debug::CheckAssertion(command->GetCommandType() != RenderCommand::COMMAND_UNKNOW, "Invalild: Unknow render command.");
	mRenderGroups[groupIndex].PushCommand(command);
}

/**
*	\brief ��ӹ�Դ��Ϣ
*/
void Renderer::PushLight()
{
}

/**
*	\brief �������й�Դ�������뵽��ɫ����
*/
void Renderer::FlushAllLights()
{
}
