#include"renderer.h"
#include"renderCommand.h"
#include"quadCommand.h"
#include"debug.h"
#include"logger.h"
#include"renderCommandPool.h"
#include"video.h"
#include"resourceCache.h"

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
	mDeferredBuffer(nullptr),
	mPolygonCount(0),
	mUsedHDR(false),
	mMultyLightBatch(true),
	mDeferredProgramState(nullptr),
	mPostProcessProgramState(nullptr),
	mAreaLight(nullptr)
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
	InitDefaultProgram();
#if defined _ENABLE_DEFERRED_SHADE_
	InitGBuffer();
#endif
	InitPostBuffer();
	InitVAOandVBO();
	
	Light::Initialize();
	mAreaLight = std::make_shared<AreaLight>(Vec3i(0,0,0), 200, Color4B(255,255,255,255));

	// ��ʼ�������
	RenderCommandPool::GetInstance().Initialize();
	mInitialized = true;
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
	ResourceCache::GetInstance().LoadDefaultProgram();
	ResourceCache::GetInstance().LoadDefaultProgramState();

	mDeferredProgramState = ResourceCache::GetInstance().GetGLProgramState(
		GLProgramState::DEFAULT_DEFERRED_LIGHT_PROGRAMSTATE_NAME);

	mPostProcessProgramState = ResourceCache::GetInstance().GetGLProgramState(
		GLProgramState::DEFAULT_POST_PROCESS_PROGRAMSTATE_NAME);
}

/**
*	\brief ��ʼ��Gbuffer�������ӳ���ɫ(
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

	if (mDeferredProgramState)
	{
		mDeferredProgramState->ApplyProgram();
		mDeferredProgramState->SetSample2D("texturePosition", 0);
		mDeferredProgramState->SetSample2D("textureNormal", 1);
		mDeferredProgramState->SetSample2D("textureColor", 2);
		mDeferredProgramState->SetSample2D("textureLight", 3);
	}
}

/**
*	\brief ��ʼ��PostFramebuffer�����ں���HDR������
*/
void Renderer::InitPostBuffer()
{
	Debug::CheckAssertion(mPostProcessBuffer == 0, "The post farme buffer already assign.");

	// ��ʼ��gbuffer
	glGenFramebuffers(1, &mPostProcessBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, mPostProcessBuffer);

	// color
	const Size srcSize = Video::GetScreenSize();
	glGenTextures(1, &mPostColorBuffer);
	glBindTexture(GL_TEXTURE_2D, mPostColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, srcSize.width, srcSize.height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mPostColorBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Debug::Error("[Render] Failed to gen post frame buffer");
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
	mForwardBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(VBO_SIZE, VBO_SIZE,
	{
		{GL_FLOAT, 3},
		{GL_UNSIGNED_BYTE, 4},
		{GL_FLOAT, 2},
		{GL_FLOAT, 3},
		true
	}));
	mForwardBuffer->Initialize();

	// �ӳ���Ⱦ����
	mDeferredBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(VBO_SIZE, VBO_SIZE,
	{
		{ GL_FLOAT, 3 },
		{ GL_UNSIGNED_BYTE, 4 },
		{ GL_FLOAT, 2 },
		{ GL_FLOAT, 3 },
		true
	}));
	mDeferredBuffer->Initialize();

	// �������Ⱦ����
	mPolygonsBuffer = std::unique_ptr<VertexBuffer>(new VertexBuffer(POLYGON_SIZE, POLYGON_SIZE,
	{
		{ GL_FLOAT, 3 },
		{ GL_UNSIGNED_BYTE, 4 },
		{ GL_FLOAT, 0 },
		{ GL_FLOAT, 0 },
		true
	}));
	mPolygonsBuffer->Initialize();
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
	mPolygonsEachCount.clear();

	mForwardBuffer->Quit();
	mDeferredBuffer->Quit();
	mPolygonsBuffer->Quit();

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
				char* quads = mDeferredBuffer->GetDataBuffer() + mDeferredQuadsCounts * mDeferredBuffer->GetDataSize() * 4;
				memcpy(quads, quadCommand->GetQuads(), sizeof(Quad)*quadCommand->GetQuadCounts());
				TransformQuadsToWorld(reinterpret_cast<Quad*>(quads), quadCommand->GetQuadCounts(), quadCommand->GetTransfomr());

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
				char* quads = mForwardBuffer->GetDataBuffer() + mForwardQuadsCounts * mForwardBuffer->GetDataSize() * 4;
				memcpy(quads, quadCommand->GetQuads(), sizeof(Quad)*quadCommand->GetQuadCounts());
				TransformQuadsToWorld(reinterpret_cast<Quad*>(quads), quadCommand->GetQuadCounts(), quadCommand->GetTransfomr());

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

	mLights.clear();
	
	mPolygonCount = 0;
	mPolygonsEachCount.clear();
}

/**
*	\brief �����Ҫ���Ƶ�polygon
*
*	PushPolygonֻ����ӻ��ƵĶ���Σ������Ļ��ƹ���
*	����RenderPolygonsִ��
*/
void Renderer::PushPolygon(const PolygonVertex & polygon)
{
	if (polygon.size() <= 0)
		return;

	int polygonCount = polygon.size();
	char* data = mPolygonsBuffer->GetDataBuffer() + mPolygonCount * sizeof(V3F_C4B);
	memcpy(data, polygon.data(), polygonCount * sizeof(V3F_C4B));

	mPolygonsEachCount.push_back(polygonCount);
	mPolygonCount += polygonCount;
}

/**
*	\brief ���ƶ����
*
*	�����ʽ�̶�ΪV3F_C4B���ýӿ�Ĭ��������ģ�����
*	������ģ������Ҫ����ָ����Ӧ����ɫ�����Ҹ�ʽ����
*	V3F_C4B
*/
void Renderer::RenderPolygons()
{
	if (mPolygonsEachCount.size() == 0 || mPolygonCount <= 0)
		return;

	mPolygonsBuffer->BeginDraw(mPolygonCount);
	int polyToDraw = 0;
	for (auto polygonCount : mPolygonsEachCount)
	{
		glDrawArrays(GL_TRIANGLE_FAN, polyToDraw, polygonCount);
		polyToDraw += polygonCount;
	}

	mPolygonsBuffer->EndDraw();

	mPolygonCount = 0;
	mPolygonsEachCount.clear();
}

void Renderer::RenderCustomon(GLProgramStatePtr programState, void * data)
{
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
*	\brief ���õ�ǰ���ӳ���ɫ��״̬
*/
void Renderer::SetDeferredProgramState(GLProgramStatePtr programState)
{
	if (programState && programState != mDeferredProgramState)
	{
		Logger::Info("[Render] Change the deferred program state.");
		mDeferredProgramState = programState;
	}
}

GLProgramStatePtr Renderer::GetDeferredProgramState()
{
	return mDeferredProgramState;
}

void Renderer::SetUsingHDR(bool hdr)
{
	mUsedHDR = hdr;
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
		vertexBuffer.BeginDraw(quadCounts*4);

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

	GLuint quadVAO = 0;
	GLuint quadVBO = 0;
	// ���׵��ı��ζ�������V3F_2TF
	float SimpleQuadVertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,   1.0f, 0.0f, 1.0f, 1.0f,
		1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,
	};

	/**
	*	\brief ����һ��Quad�ı���
	*/
	void DrawPostRenderQuad()
	{
		if (quadVAO == 0)
		{
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(SimpleQuadVertices), &SimpleQuadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		}
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
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
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	RenderClear();
	// deferred pass;������Ⱦ����Ƭ����ɫ������GBUFFER
	PostRenderQuad();
}

/**
*	\brief ����mQuadBatches����ִ�����������ƴ���    
*/
void Renderer::ForwardDrawQuadBatches()
{
	//// test
	//PolygonVertex pv = {
	//	{ 0.0f,0.0f,0.0f,255,255,0,255 },
	//	{ 100.0f,0.0f,0.0f,255,255,0,255 },
	//	{ 100.0f,100.0f,0.0f,255,255,0,255 },
	//	{ 200.0f,400.0f,0.0f,255,255,0,255 },
	//	{ 100.0f,200.0f,0.0f,255,255,0,255 },
	//	{ 50.0f,400.0f,0.0f,255,255,0,255 }
	//};
	//PushPolygon(pv);

	//PolygonVertex ppv = {
	//	{ 0.0f,0.0f,0.0f,0,0,255,255 },
	//	{ 100.0f,0.0f,0.0f,0,0,255,255 },
	//	{ 100.0f,100.0f,0.0f,0,0,255,255 }
	//};
	//PushPolygon(ppv);

	//auto programState = ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_POLYGON_COLOR_PROGRAMSTATE_NAME);
	//if (programState)
	//{
	//	// �趨���ģʽ
	//	glBlendFunc(GL_ONE, GL_ONE);
	//	programState->Apply();
	//	programState->SetUniformMatrix4("projection", mCamearMatrix);

	//	RenderPolygons();
	//}

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
	// �����Ҫ����hdr��drawPass������Ҫ��gBuffer
	std::function<void(const std::vector<LightPtr>& lights, int drawCall)> DrawPassFunction = nullptr;
	if (mUsedHDR){
		DrawPassFunction = [this](const std::vector<LightPtr>& lights, int drawCall) {
			bool firstDraw = (drawCall == 0);
			DrawLightMeshTexture(lights);
			BindGBuffer(firstDraw);
			FlushAllLights(lights, PointLight::mLightTexture, firstDraw);
			DrawPostRenderQuad();
			EndGBuffer();
		};
	}else{
		DrawPassFunction = [this](const std::vector<LightPtr>& lights, int drawCall) {
			bool firstDraw = (drawCall == 0);
			DrawLightMeshTexture(lights);
			FlushAllLights(lights, PointLight::mLightTexture, firstDraw);
			DrawPostRenderQuad();
		};
	}

	// ���ս׶Σ�Ŀǰ�����е㽩Ӳ���պ���취�Ż�
	// ���ս׶θ����Ƿ񿪿�������տ��ط�Ϊ���ִ���ʽ
	// 1.ͬʱ��ഫ��32����Դ��Ϣ������Ⱦ��һ�Ź�������ͼ
	// 2.ÿ�δ���һ�����գ�ÿ�����ջ��Ƶ����Ĺ�������ͼ,ԓ�������ɣ�
	int maxRenderLightNum = 0;
	if (mMultyLightBatch)
		maxRenderLightNum = PointLight::MAX_RENDER_LIGHT_COUNT;
	else
		maxRenderLightNum = 1;

	std::vector<LightPtr> lights;
	int lightDrawCount = 0;
	int drawCall = 0;
	for(auto& light : mLights)
	{
		if (light->GetType() == LIGHT_TYPE_POINT) {
			lights.push_back(light);
			lightDrawCount++;

			if (lightDrawCount >= maxRenderLightNum)
			{
				DrawPassFunction(lights, drawCall);
				lightDrawCount = 0;
				drawCall++;
			}
		}
	}
	if (lightDrawCount >= 0)	// ��������ڹ��գ�Ҳ��Ҫһ����Ⱦ
	{
		DrawPassFunction(lights, drawCall);
		lightDrawCount = 0;
	}

	// post process
	if (mUsedHDR)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// ����׶Σ�HDR�����⣩
		mPostProcessProgramState->Apply();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mPostColorBuffer);
		DrawPostRenderQuad();
	}
}

/**
*	\brief �������й�Դ�������뵽��ɫ����
*/
void Renderer::FlushAllLights(const std::vector<LightPtr>& lights, RenderTexture& lightTexture, bool firstDraw)
{
	// ��������
	if (firstDraw)
		mAreaLight->SetLightToProgram(*mDeferredProgramState, 0);

	// ���Դ
	int lightIndex = 0;
	for (auto& light : mLights)
	{
		light->DebugRender();
		light->SetLightToProgram(*mDeferredProgramState, lightIndex);
		lightIndex++;
	}
	mDeferredProgramState->SetUniform1i("lightCount", lightIndex);

	// bind light texture
	lightTexture.Bind(GL_TEXTURE3);
}

/**
*	\brief ���ƹ��տɼ�������
*/
void Renderer::DrawLightMeshTexture(const std::vector<LightPtr>& lights)
{
	auto& lightMeshTexture = PointLight::mLightTexture;
	lightMeshTexture.BeginDraw();
	for (auto& light : lights)
	{
		light->Render();	// hide while debugging
	}

	auto programState = ResourceCache::GetInstance().GetGLProgramState(GLProgramState::DEFAULT_POLYGON_COLOR_PROGRAMSTATE_NAME);
	if (programState)
	{
		// �趨���ģʽ
		glBlendFunc(GL_ONE, GL_ONE);
		programState->Apply();
		programState->SetUniformMatrix4("projection", mCamearMatrix);

		RenderPolygons();
	}

	lightMeshTexture.EndDraw();

	// ���ڵ��δ���Ĺ����������ģ������
	if (!mMultyLightBatch)
	{
		// nothing now.
	}
}

/**
*	\brief �󶨵�ǰFrameBufferΪgbuffer
*/
void Renderer::BindGBuffer(bool clearBuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, mPostProcessBuffer);
	if (clearBuffer)glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mDeferredProgramState->SetUniform1i("firstDraw", clearBuffer ? 1 : 0);
	mDeferredProgramState->Apply();
	
	// gFrameBuffer texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mGPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mGNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mGColor);

	glBlendFunc(GL_ONE, GL_ONE);
}

void Renderer::EndGBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
void Renderer::PushLight(LightPtr light)
{
	if (light != nullptr)
	{
		mLights.push_back(light);
	}
}

/**
*	\brief ����������գ��������գ�
*/
void Renderer::SetAreaLight(std::shared_ptr<AreaLight> areaLight)
{
	if (mAreaLight != areaLight)
	{
		mAreaLight = areaLight;
	}
}

std::shared_ptr<AreaLight> Renderer::GetAreaLight()
{
	return mAreaLight;
}
