#pragma once

#include"common\common.h"
#include"font.h"

namespace font {

const string DEFAULT_FONT_NAME = "msyh.ttf";
const string DEFAULT_FONT_EN_NAME = "arial.ttf";

/**
*	\brief ����������
*/
struct FontConfig
{
	string name;
	int   fontSize;
	int   letterSpacing;	// �ּ��
	int	  lineHeight;		// �м��

	FontConfig() :name(""), fontSize(0), letterSpacing(0), lineHeight(0) {}
};

/**
*	\brief ���弯��
*/
class FontAtlas
{
public:
	static FontAtlas& GetInstance();

	FontAtlas();
	~FontAtlas();

	void LoadDefaultFont();
	void LoadDefaultFontEn();
	void LoadFontFromTTF(const string& filename, const std::vector<Font::FontCodeRange> & range, const FontConfig* fontConfig = nullptr);

	FontPtr GetDefaultFont();
	FontPtr GetFontByName(const string& name);
	string GetNameByFont(const FontPtr& fontPtr);

private:
	std::map<string, FontPtr> mFonts;

};

}
