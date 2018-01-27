#include "vertexArray.h"
#include "core\debug.h"

VertexBuffer::VertexBuffer():
	mMaxVertex(0)
{
}

VertexBuffer::VertexBuffer(int maxVertex):
	mMaxVertex(maxVertex)
{
}

VertexBuffer::~VertexBuffer()
{
	Quit();
}

bool VertexBuffer::InitQuadBuffer()
{
	Debug::CheckAssertion(mMaxVertex > 0,
		"Invalid max vertex in vertexArray::Init()");

	mQuads = new Quad[mMaxVertex];
	if (mQuads == nullptr)
		return false;

	mIndices = new GLushort[mMaxVertex * 6];
	if (mIndices == nullptr)
		return false;

	return true;
}

bool VertexBuffer::InitIndices()
{
	for (int i = 0; i < mMaxVertex; ++i)
	{
		mIndices[i * 6 + 0] = (GLushort)(i * 4 + 0);
		mIndices[i * 6 + 1] = (GLushort)(i * 4 + 1);
		mIndices[i * 6 + 2] = (GLushort)(i * 4 + 2);
		mIndices[i * 6 + 3] = (GLushort)(i * 4 + 2);
		mIndices[i * 6 + 4] = (GLushort)(i * 4 + 3);
		mIndices[i * 6 + 5] = (GLushort)(i * 4 + 0);
	}
	return true;
}

bool VertexBuffer::InitVertexBuffer()
{
	Debug::CheckAssertion(mVAO == 0 && mVBO == 0, "The VAO and VBO already assign.");

	glGenVertexArrays(1, &mVAO);	// VAO
	glGenBuffers(1, &mVBO);			// 顶点数据
	glGenBuffers(1, &mVEO);			// 顶点索引

	// 绑定顶点数据缓冲区
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0])*mMaxVertex, mQuads, GL_DYNAMIC_DRAW);

	// 设置vao
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_4CB_2TF_3NF), (GLvoid*)offsetof(V3F_4CB_2TF_3NF, vertices));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(V3F_4CB_2TF_3NF), (GLvoid*)offsetof(V3F_4CB_2TF_3NF, colors));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V3F_4CB_2TF_3NF), (GLvoid*)offsetof(V3F_4CB_2TF_3NF, texs));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(V3F_4CB_2TF_3NF), (GLvoid*)offsetof(V3F_4CB_2TF_3NF, normalsl));

	// 绑定顶点索引
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVEO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0])*mMaxVertex * 6, mIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	return true;
}

bool VertexBuffer::Initialize()
{
	if (!InitQuadBuffer())
	{
		Debug::Error("Faile to init quad buffer");
		return false;
	}

	if (!InitIndices())
	{
		Debug::Error("Faile to init indices buffer.");
		return false;
	}

	if (!InitVertexBuffer())
	{
		Debug::Error("Faile to init vertex buffer.");
		return false;
	}


	mInitialized = true;
	return true;
}

void VertexBuffer::Quit()
{
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mVEO);
	glDeleteVertexArrays(1, &mVAO);

	mVAO = mVBO = mVEO = 0;
	mInitialized = false;
}

void VertexBuffer::BeginDraw(int quadCount)
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mQuads[0])*quadCount, nullptr, GL_DYNAMIC_DRAW);
	void*buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	size_t allocSize = sizeof(mQuads[0])*quadCount;
	memcpy_s(buf, allocSize, mQuads, allocSize);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void VertexBuffer::EndDraw()
{
	glBindVertexArray(0);
}
