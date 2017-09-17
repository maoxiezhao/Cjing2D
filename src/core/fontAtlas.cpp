#include"fontAtlas.h"
#include"core\logger.h"
#include"core\fileData.h"

namespace font{

FontAtlas & FontAtlas::GetInstance()
{
	static FontAtlas fontAtlas;
	return fontAtlas;
}

FontAtlas::FontAtlas()
{
}

FontAtlas::~FontAtlas()
{
	mFonts.clear();
}

/**
*	\brief ����Ĭ����������
*/
void FontAtlas::LoadDefaultFont()
{
	FontConfig defaultFontConfig;
	defaultFontConfig.name = DEFAULT_FONT_NAME;
	defaultFontConfig.fontSize = 16;
	defaultFontConfig.letterSpacing = 1;
	defaultFontConfig.lineHeight = 16;

	std::vector<Font::FontCodeRange> codeRanges;
	// ȫ������Ĭ�ϼ���ȫ�������ַ��������ַ�
	codeRanges.emplace_back(Font::FontCodeRange(Font::FONT_CODE_LATIN_START,  Font::FONT_CODE_LATIN_END));
	codeRanges.emplace_back(Font::FontCodeRange(Font::FONT_CODE_CHINESE_START,Font::FONT_CODE_CHINESE_END));

	// ���⴦�������������
	codeRanges.emplace_back(Font::FontCodeRange(0x3002, 0x3002));	// ����
	codeRanges.emplace_back(Font::FontCodeRange(0xFF0C, 0xFF0C));	// ���

	LoadFontFromTTF(DEFAULT_FONT_NAME, codeRanges, &defaultFontConfig);
}

/**
*	\brief ����Ĭ��Ӣ������
*/
void FontAtlas::LoadDefaultFontEn()
{
	FontConfig defaultFontConfig;
	defaultFontConfig.name = DEFAULT_FONT_EN_NAME;
	defaultFontConfig.fontSize = 16;
	defaultFontConfig.letterSpacing = 0;
	defaultFontConfig.lineHeight = 16;

	std::vector<Font::FontCodeRange> codeRanges;
	codeRanges.emplace_back(Font::FontCodeRange(Font::FONT_CODE_LATIN_START, Font::FONT_CODE_LATIN_END));
	LoadFontFromTTF(DEFAULT_FONT_EN_NAME, codeRanges, &defaultFontConfig);
}


void FontAtlas::LoadFontFromTTF(const string & filename, const std::vector<Font::FontCodeRange>& range, const FontConfig * fontConfig)
{
	if (mFonts.find(filename) != mFonts.end())
	{
		Debug::Warning("The Font loaded has already existed.");
		return;
	}

	string fontfile = "fonts/" + filename;
	if (!FileData::IsFileExists(fontfile))
	{
		Debug::Error("Font file '" + fontfile + "' is not existed.");
	}
	string data = FileData::ReadFile(fontfile);
	if (data.empty())
	{
		Debug::Error("Failed to open font file.");
	}
	auto font = std::make_shared<font::Font>(fontfile);

	// �������������ļ�������������
	if (fontConfig)
	{
		font->SetFontSize(fontConfig->fontSize);
		font->SetLineHeight(fontConfig->lineHeight);
		font->SetLetterSpacing(fontConfig->letterSpacing);
	}
	font->LoadFont(data, range);
	mFonts[filename] = font;
}

/**
*	\brief ����Ĭ�����õ�����
*/
FontPtr FontAtlas::GetDefaultFont()
{
	return GetFontByName(DEFAULT_FONT_NAME);
}

FontPtr FontAtlas::GetFontByName(const string & name)
{
	auto itor = mFonts.find(name);
	if (itor != mFonts.end())
	{
		return itor->second;
	}
	return nullptr;
}

const string & FontAtlas::GetNameByFont(const FontPtr & fontPtr)
{
	for (auto& kvp : mFonts)
	{
		if (kvp.second == fontPtr)
			return kvp.first;
	}
	return string("");
}

}