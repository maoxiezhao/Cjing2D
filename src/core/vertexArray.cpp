#include"vertexArray.h"
#include"utils\reflect.h"

namespace {
	size_t GetTypeSizeByEnum(int v)
	{
		auto& meta = util::refel::MetaManager::GetInstance().GetMeta(v);
		return meta.GetSize();
	}

	/**
	*	\brief 计算按照结构体对齐后的数据大小
	*/
	struct Attribute { size_t size; int count; };
	size_t GetStructAlignSize(const std::vector<Attribute>& attributes, bool align, VertexBuffer::QuadTypeInfo& info)
	{
		size_t size = 0;
		size_t alignSize = 1;
		for (auto& attr : attributes)
		{
			if (attr.count > 0)
			{
				alignSize = std::max(attr.size, alignSize);
				size_t curSize = attr.size * attr.count;
				size += curSize;
				size = align && (size & (alignSize - 1)) > 0 ? size - (size & (alignSize - 1)) + alignSize : size;
				info.offset.push_back(size - curSize);
			}
		}
		info.dataSize = size;
		return size;
	}

	/**
	*	\brief 计算自定义顶点格式下的大小
	*/
	size_t CalucateDataSize(const VertexFormat& vf, VertexBuffer::QuadTypeInfo& info)
	{
		std::vector<Attribute> attributes;
		attributes.push_back({ GetTypeSizeByEnum(vf.mPosType), vf.mPosCount });
		attributes.push_back({ GetTypeSizeByEnum(vf.mColorType), vf.mColorCount });
		attributes.push_back({ GetTypeSizeByEnum(vf.mTextureType), vf.mTextureCount });
		attributes.push_back({ GetTypeSizeByEnum(vf.mNormalType), vf.mNormalCount });
		return GetStructAlignSize(attributes, vf.mAlign, info);
	}
}

VertexBuffer::VertexBuffer(int maxVertex, int maxIndices, const VertexFormat& vf) :
	mMaxVertex(maxVertex),
	mMaxIndices(maxIndices),
	mVAO(0),
	mVBO(0),
	mVEO(0),
	mVertexFormat(vf),
	mData(nullptr),
	mIndices(nullptr)
{
}

VertexBuffer::~VertexBuffer()
{
	Quit();
}

void VertexBuffer::Quit()
{
	if (!mInitialized)
		return;

	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mVEO);
	glDeleteVertexArrays(1, &mVAO);

	mVAO = mVBO = mVEO = 0;
	mInitialized = false;

	//if (mQuads)
	//	delete mQuads;
	if (mIndices)
		delete[] mIndices;
	if (mData)
		delete[] mData;
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


bool VertexBuffer::InitQuadBuffer()
{
	Debug::CheckAssertion(mMaxVertex > 0,
		"Invalid max vertex in vertexArray::Init()");

	//mQuads = new Quad[mMaxVertex];
	//if (mQuads == nullptr)
	//	return false;

	CalucateDataSize(mVertexFormat, mDataInfo);
	if (mDataInfo.dataSize <= 0)
		return false;

	mData = new char[mDataInfo.dataSize * mMaxVertex];
	if (mData == nullptr)
		return false;

	return true;
}

bool VertexBuffer::InitIndices()
{
	if (mMaxIndices > 0)
	{
		mIndices = new GLushort[mMaxIndices * 6];
		if (mIndices == nullptr)
			return false;

		for (int i = 0; i < mMaxIndices; ++i)
		{
			mIndices[i * 6 + 0] = (GLushort)(i * 4 + 0);
			mIndices[i * 6 + 1] = (GLushort)(i * 4 + 1);
			mIndices[i * 6 + 2] = (GLushort)(i * 4 + 2);
			mIndices[i * 6 + 3] = (GLushort)(i * 4 + 2);
			mIndices[i * 6 + 4] = (GLushort)(i * 4 + 3);
			mIndices[i * 6 + 5] = (GLushort)(i * 4 + 0);
		}
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
	glBufferData(GL_ARRAY_BUFFER, mDataInfo.dataSize*mMaxVertex, mData, GL_DYNAMIC_DRAW);

	// 设置vao
	auto& offset = mDataInfo.offset;
	int dataSize = mDataInfo.dataSize;
	int index = 0;
	if (mVertexFormat.mPosCount > 0)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, mVertexFormat.mPosCount, mVertexFormat.mPosType, GL_FALSE, dataSize, (GLvoid*)(offset[index]));
		index++;
	}
	if (mVertexFormat.mColorCount > 0)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, mVertexFormat.mColorCount, mVertexFormat.mColorType, GL_TRUE, dataSize, (GLvoid*)(offset[index]));
		index++;
	}
	if (mVertexFormat.mTextureCount > 0)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, mVertexFormat.mTextureCount, mVertexFormat.mTextureType, GL_FALSE, dataSize, (GLvoid*)(offset[index]));
		index++;
	}
	if (mVertexFormat.mNormalCount > 0)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, mVertexFormat.mNormalCount, mVertexFormat.mNormalType, GL_FALSE, dataSize, (GLvoid*)(offset[index]));
	}

	// 绑定顶点索引
	if (mIndices > 0 && mIndices != nullptr)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVEO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mIndices[0])*mMaxIndices * 6, mIndices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);

	return true;
}

void VertexBuffer::BeginDraw(int vertexCount)
{
	size_t allocSize = mDataInfo.dataSize*vertexCount;
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	glBufferData(GL_ARRAY_BUFFER, allocSize, nullptr, GL_DYNAMIC_DRAW);
	void*buf = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	memcpy_s(buf, allocSize, mData, allocSize);
	glUnmapBuffer(GL_ARRAY_BUFFER);
}

void VertexBuffer::EndDraw()
{
	glBindVertexArray(0);
}
