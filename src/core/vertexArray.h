#pragma once

#include"common\common.h"
#include"core\types.h"
#include"utils\matrix4.h"

/**
*	\brief 顶点缓冲
*
*	顶点缓冲包含了VAO,VBO,EBO,封装了相关的bind和使用过程
*/
class VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(int maxVertex);
	~VertexBuffer();

	VertexBuffer(const VertexBuffer& rth) = delete;
	VertexBuffer& operator=(const VertexBuffer& rth) = delete;

	Quad* GetQuadBuffer() { return mQuads; };
	GLushort* GetIndices() { return mIndices; };

	bool Initialize();
	void Quit();
	void BeginDraw(int quadCount);
	void EndDraw();

private:
	bool InitQuadBuffer();
	bool InitIndices();
	bool InitVertexBuffer();

	GLuint mVAO;
	GLuint mVBO;
	GLuint mVEO;

	int mMaxVertex;
	bool mInitialized;

	Quad* mQuads;	
	GLushort* mIndices;

};