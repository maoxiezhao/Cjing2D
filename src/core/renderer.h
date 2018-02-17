#ifndef _RENDERER_H_
#define _RENDERER_H_

#include"common\common.h"
#include"core\types.h"
#include"core\vertexArray.h"
#include"core\glProgramState.h"
#include"core\light.h"
#include"utils\matrix4.h"

#include<stack>

/**
*	1/27/2018 Ԥ���ع�������Ⱦ������PASS��ʽ��ȡ
*			  ͬʱshaderɢ�ļ�д��һ���ļ��У� ���ַ����ָ�����shader
*
*	2/16/2018 Ԥ��ʹ��opengl4.0�������ӳ���ʽ��������ɫ������
*/

class RenderCommand;
class QuadCommand;
class CustomerCommand;
/**
*	\brief ��Ⱦ���У����ڴ��RenderCommand
*/
class RenderQueue
{
public:
	void PushCommand(RenderCommand* command);
	size_t Size()const;
	void Sort();
	void Clear();
	RenderCommand* operator[](size_t index)const;

private:
	vector<RenderCommand*>mQueueNegZ;
	vector<RenderCommand*>mQueue0;
	vector<RenderCommand*>mQueuePosZ;
};


/**
*	\brief ��Ⱦģ�飬Renderer�����ʼ����ɫ����ͬʱִ��ÿ����ѭ���ύ��
*	��Ⱦ����
*/
class Renderer
{
public:
	// system method
	static Renderer& GetInstance();

	Renderer(const Renderer&other) = delete;
	Renderer&operator=(const Renderer& other) = delete;

	// system
	void Initialize(int w, int h);
	void Quit();
	bool IsInitialized();
	void PushCommand(RenderCommand* command);
	void PushCommand(RenderCommand* command, int groupIndex);
	void RenderClear();
	void Render();
	void RenderAfterClean();
	void PushPolygon(const PolygonVertex& polygon);
	void RenderPolygons();
	void RenderCustomon(GLProgramStatePtr programState, void* data);

	// light
	void PushLight(LightPtr light);
	void SetAreaLight(std::shared_ptr<AreaLight> areaLight);
	std::shared_ptr<AreaLight> GetAreaLight();

	// data
	Matrix4 GetCameraMatrix()const;
	void SetViewSize(int w, int h);
	void SetDeferredProgramState(GLProgramStatePtr programState);
	GLProgramStatePtr GetDeferredProgramState();

	void SetUsingHDR(bool hdr);

private:
	Renderer();
	~Renderer();

	void InitCamearMatrix();
	void InitDefaultProgram();
	void InitGBuffer();
	void InitPostBuffer();
	void InitVAOandVBO();

	void VisitRenderQueue(const RenderQueue& queue);
	void Flush();
	void DeferredDrawQuadBatches();
	void ForwardDrawQuadBatches();
	void TransformQuadsToWorld(Quad* mQuads, int quadCount,const Matrix4 transform);
	void PostRenderQuad();
	void FlushAllLights(const std::vector<LightPtr>& lights, RenderTexture& lightTexture, bool firstDraw);
	void DrawLightMeshTexture(const std::vector<LightPtr>& lights);

	void BindGBuffer(bool clearBuffer = false);
	void EndGBuffer();

private:
	std::vector<RenderQueue> mRenderGroups;
	std::stack<int> mRenderGroupsStack;

	bool mInitialized;
	bool mIsRenderer;
	bool mUsedHDR;	
	bool mMultyLightBatch;	

	// deferred frame buffer
	GLuint mGBuffer;	// global frame buffer
	unsigned int mGPosition, mGNormal, mGColor;
	GLProgramStatePtr mDeferredProgramState;

	// hdr post process frame buffer
	GLuint mPostProcessBuffer;
	unsigned int mPostColorBuffer;
	GLProgramStatePtr mPostProcessProgramState;

	// render data
	static const uint32_t VBO_SIZE = 65536;
	std::unique_ptr<VertexBuffer> mForwardBuffer;
	std::unique_ptr<VertexBuffer> mDeferredBuffer;

	int mForwardQuadsCounts;
	int mDeferredQuadsCounts;

	std::vector<QuadCommand*>mQuadDeferredBatches;
	std::vector<QuadCommand*>mQuadForwardBatches;

	int mViewWidth, mViewHeight;
	Matrix4 mCamearMatrix;			// ȫ��ͳһ������任����

	// light info
	std::shared_ptr<AreaLight> mAreaLight;
	std::vector<LightPtr> mLights;

	// poly render
	static const uint32_t POLYGON_SIZE = 8192;
	int mPolygonCount;
	std::vector<int> mPolygonsEachCount;
	std::unique_ptr<VertexBuffer> mPolygonsBuffer;
};

#endif