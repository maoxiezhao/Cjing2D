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
*	1/27/2018 预计重构，将渲染过程以PASS方式提取
			  同时shader散文件写到一个文件中， 以字符串分隔各个shader
*/

class RenderCommand;
class QuadCommand;
class CustomerCommand;
/**
*	\brief 渲染队列，用于存放RenderCommand
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
*	\brief 渲染模块，Renderer负责初始化着色器，同时执行每次主循环提交的
*	渲染命令
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

	// light
	void PushLight(LightPtr light);
	void FlushAllLights();

	// data
	Matrix4 GetCameraMatrix()const;
	void SetViewSize(int w, int h);
	void SetDeferredProgramState(GLProgramStatePtr programState);
	GLProgramStatePtr GetDeferredProgramState();

private:
	Renderer();
	~Renderer();

	void InitCamearMatrix();
	void InitDefaultProgram();
	void InitGBuffer();
	void InitVAOandVBO();

	void VisitRenderQueue(const RenderQueue& queue);
	void Flush();
	void DeferredDrawQuadBatches();
	void ForwardDrawQuadBatches();
	void TransformQuadsToWorld(Quad* mQuads, int quadCount,const Matrix4 transform);
	void PostRenderQuad();

private:
	std::vector<RenderQueue> mRenderGroups;
	std::stack<int> mRenderGroupsStack;

	bool mInitialized;
	bool mIsRenderer;

	GLuint mGBuffer;	// global frame buffer
	unsigned int mGPosition, mGNormal, mGColor;
	GLProgramStatePtr mDeferredProgramState;

	// render data
	static const uint32_t VBO_SIZE = 65536;
	std::unique_ptr<VertexBuffer> mForwardBuffer;
	std::unique_ptr<VertexBuffer> mDeferredBuffer;

	int mForwardQuadsCounts;
	int mDeferredQuadsCounts;

	std::vector<QuadCommand*>mQuadDeferredBatches;
	std::vector<QuadCommand*>mQuadForwardBatches;

	int mViewWidth, mViewHeight;
	Matrix4 mCamearMatrix;			// 全局统一的相机变换矩阵

	std::vector<LightPtr> mLights;

	static const uint32_t POLYGON_SIZE = 8192;
	int mPolygonCount;
	std::vector<int> mPolygonsEachCount;
	std::unique_ptr<VertexBuffer> mPolygonsBuffer;

	//GLuint mVAO;
	//GLuint mVBO;
	//GLuint mVEO;
	//GLuint mGBuffer;

	//Quad mForwardQuads[VBO_SIZE];	// 前向渲染quad顶点数据
	//Quad mDeferredQuads[VBO_SIZE];	// 延迟渲染quad顶点数据
	//GLushort mIndices[VBO_SIZE * 6];
};

#endif