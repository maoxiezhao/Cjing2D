#pragma once

#include"common\common.h"
#include"core\types.h"
#include "core\debug.h"
#include"utils\matrix4.h"

#include<map>

struct VertexType
{
	int type;
	int count;
};

/**
*	\brief �����ʽ˵��
*/
struct VertexFormat
{
	VertexFormat() :
		mPosCount(0),
		mColorCount(0),
		mTextureCount(0),
		mNormalCount(0),
		mAllAtrributeCount(0)
	{
	}

	VertexFormat(VertexType pc, VertexType cc, VertexType tc, VertexType nc, bool align) :
		mPosCount(pc.count), 
		mPosType(pc.type),
		mColorCount(cc.count),
		mColorType(cc.type),
		mTextureCount(tc.count),
		mTextureType(tc.type),
		mNormalCount(nc.count),
		mNormalType(nc.type),
		mAllAtrributeCount(pc.count + cc.count + nc.count + tc.count),
		mAlign(align)
	{
	}

	int mPosCount;
	int mPosType;
	int mColorCount;
	int mColorType;
	int mTextureCount;
	int mTextureType;
	int mNormalCount;
	int mNormalType;

	int mAllAtrributeCount;
	bool mAlign;
};

/**
*	\brief ���㻺��
*
*	���㻺�������VAO,VBO,EBO,��װ����ص�bind��ʹ�ù���
*/
class VertexBuffer
{
public:
	VertexBuffer(int maxVertex, int maxIndices, const VertexFormat& vf);
	~VertexBuffer();

	VertexBuffer(const VertexBuffer& rth) = delete;
	VertexBuffer& operator=(const VertexBuffer& rth) = delete;

	Quad* GetQuadBuffer() { return mQuads; };	// �ʂ�U��
	GLushort* GetIndices() { return mIndices; };
	size_t GetDataSize() { return mDataInfo.dataSize; }
	char* GetDataBuffer() { return mData; }

	bool Initialize();
	void Quit();
	void BeginDraw(int quadCount);
	void EndDraw();

	/**
	*	\brief �Զ��嶥���ʽ�µ���������
	*/
	struct QuadTypeInfo
	{
		size_t dataSize;
		std::vector<int> offset;

		QuadTypeInfo() :dataSize(0) {}
	};

private:
	bool InitQuadBuffer();
	bool InitIndices();
	bool InitVertexBuffer();

	GLuint mVAO;
	GLuint mVBO;
	GLuint mVEO;

	int mMaxVertex;
	int mMaxIndices;
	bool mInitialized;

	Quad* mQuads;	
	GLushort* mIndices;
	const VertexFormat& mVertexFormat;

	QuadTypeInfo mDataInfo;
	char* mData;
};
