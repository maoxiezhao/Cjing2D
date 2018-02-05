#ifndef _TYPES_H_
#define _TYPES_H_

#include"common\common.h"
#include"utils\color.h"
#include"utils\vec3.h"
#include"game\enumInfo.h"

/******************************************
*	\brief 存放用于渲染的结构体
*******************************************/

/**
*	\brief 纹理坐标结构
*/
struct Tex2
{
	float u, v;
	Tex2() :u(.0f), v(.0f) {}
};

struct V3F_C4B
{
	Vec3f vertices;
	Color4B colors;

	V3F_C4B(float v1, float v2, float v3, GLubyte c1, GLubyte c2, GLubyte c3, GLubyte c4) :
		vertices(v1, v2, v3), colors(c1, c2, c3, c4) {}
};

/**
*	\brief 暂时设定多边形为V3F_C4B顶点格式集合
*/
using PolygonVertex = std::vector<V3F_C4B>;

/**
*	\brief 用于绘制的顶点结构
*/
struct V3F_4CB_2TF
{
	Vec3f   vertices;
	Color4B colors;
	Tex2    texs;
};

/**
*	\brief 用于绘制的顶点结构,带法向量
*/
struct V3F_4CB_2TF_3NF
{
	Vec3f   vertices;
	Color4B colors;
	Tex2    texs;
	Vec3f	normalsl;
};

/**
*	\brief quad
*/
struct Quad
{
	//V3F_4CB_2TF lt;
	//V3F_4CB_2TF lb;
	//V3F_4CB_2TF rb;
	//V3F_4CB_2TF rt;

	V3F_4CB_2TF_3NF lt;
	V3F_4CB_2TF_3NF lb;
	V3F_4CB_2TF_3NF rb;
	V3F_4CB_2TF_3NF rt;
};

/**
*	\brief 混合模式结构,结构体中提供对src和dst颜色的混合方法的
*	enum值
*/
struct BlendFunc
{
	GLenum srcBlend;
	GLenum dstBlend;
	BlendFunc() :srcBlend(GL_SRC_ALPHA), dstBlend(GL_ONE_MINUS_SRC_ALPHA) {}
	BlendFunc(GLenum src, GLenum dst) :srcBlend(src), dstBlend(dst) {}
};

constexpr bool operator == (const BlendFunc& lhs, const BlendFunc& rhs)
{
	return lhs.dstBlend == rhs.dstBlend && lhs.srcBlend == rhs.srcBlend;
}

constexpr bool operator != (const BlendFunc& lhs, const BlendFunc& rhs)
{
	return lhs.dstBlend != rhs.dstBlend || lhs.srcBlend != rhs.srcBlend;
}

// 用于对opengl的blend枚举值对应的字符
template<>
struct EnumInfoTraits<GLenum>
{
	static const string prettyName;
	static const EnumInfo<GLenum>::nameType names;
};

// 2D光照类型

#endif