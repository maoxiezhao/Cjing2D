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
	InitIndices();
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
*	\brief ��ʼ��indices
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
*	\brief ��ʼ��VAO��VBO��VEO
*/
void Renderer::InitVAOandVBO()
{
	Debug::CheckAssertion(mVAO == 0 && mVBO == 0, "The VAO and VBO already assign.");

	glGenVertexArrays(1, &mVAO);	// VAO
	glGenBuffers(1, &mVBO);			// ��������
	glGenBuffers(1, &mVEO);			// ��������

	// �󶨶������ݻ�����
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0])*VBO_SIZE, mQuads, GL_DYNAMIC_DRAW);
	// ����vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_4CB_2TF), (GLvoid*)offsetof(V3F_4CB_2TF, vertices));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_4CB_2TF), (GLvoid*)offsetof(V3F_4CB_2TF,colors));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_4CB_2TF), (GLvoid*)offsetof(V3F_4CB_2TF, texs));
	// �󶨶�������
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVEO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0])*VBO_SIZE * 6, mIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

}

/**
*	\brief �ͷź���
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
			if (mQuadsCounts + quadCommand->GetQuadCounts() > VBO_SIZE)
			{	// ��ǰquad�����������ֵ��������Ⱦ֮ǰ�����
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
*/
void Renderer::Flush()
{
	DrawQuadBatches();
}

/**
*	\brief ����mQuadBatches����ִ�������ƴ���    
*/
void Renderer::DrawQuadBatches()
{
	if (mQuadsCounts <= 0 || mQuadBatches.empty())
		return;
	// ���·���VBO�󶨻�������С
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
		{	// �����ǰ��Ⱦ״̬���ȣ���ִ����Ⱦ
			glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(mIndices[0])*startQuad * 6));
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
		glDrawElements(GL_TRIANGLES, (GLsizei)quadToDraw * 6, GL_UNSIGNED_SHORT, (GLvoid*)(sizeof(mIndices[0])*startQuad*6));
	}

	glBindVertexArray(0);
	mQuadsCounts = 0;
	mQuadBatches.clear();
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
