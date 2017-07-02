#include"types.h"

const string EnumInfoTraits<GLenum>::prettyName = "Blend mode";

const EnumInfo<GLenum>::nameType EnumInfoTraits<GLenum>::names = {
	{GL_ZERO, "GL_ZERO"},
	{GL_ONE, "GL_ONE"},
	{GL_SRC_ALPHA, "GL_SRC_ALPHA"},
	{GL_DST_ALPHA, "GL_DST_ALPHA"},
	{GL_SRC_COLOR, "GL_SRC_COLOR"},
	{GL_DST_COLOR, "GL_DST_COLOR"},
	{GL_ONE_MINUS_DST_ALPHA, "GL_ONE_MINUS_DST_ALPHA"},
	{GL_ONE_MINUS_DST_COLOR, "GL_ONE_MINUS_DST_COLOR"},
	{GL_ONE_MINUS_SRC_ALPHA, "GL_ONE_MINUS_SRC_ALPHA"}
};