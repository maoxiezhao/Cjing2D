#include<iostream>
#include<Windows.h>
#include"core\debug.h"
#include"game\app.h"
#include"core\random.h"
#include"core\inputEvent.h"
#include"utils\size.h"

#include"test.h"
#include"core\types.h"
#include"utils\vec2.h"
#include"core\resourceCache.h"
#include"core\fileData.h"
#include"utils\rectangle.h"
#include"utils\matrix4.h"

#include"core\asyncLoader.h"
#include<future>

/**
*	\brief
*
*	10xxxxxx
*	110xxxxx 10xxxxxx
*	1110xxxx 10xxxxxx 10xxxxxx
*	11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
int ParsedUtf8(unsigned int& outChar, const  char* textBegin, const  char* textEnd)
{
	unsigned int result = (unsigned int)-1;
	const unsigned char* curChar = (const unsigned char*)textBegin;

	// 单字节
	if ((*curChar & 0xc0) == 0x80)
	{
		result = (unsigned int)(*curChar++);
		outChar = result;
		return 1;
	}
	// 2字节 
	if ((*curChar & 0xe0) == 0xc0)
	{
		result = (unsigned int)((*curChar++ & 0x1F) << 6);
		if ((*curChar & 0xc0) != 0x80)
			return 2;
		result += (unsigned int)(*curChar++ & 0x3f);
		outChar = result;
		return 2;
	}

	// 3字节
	if ((*curChar & 0xf0) == 0xe0)
	{
		result = (unsigned int)((*curChar++ & 0x0F) << 12);
		if ((*curChar & 0xc0) != 0x80)
			return 3;
		result += (unsigned int)((*curChar++ & 0x3f) << 6);
		if ((*curChar & 0xc0) != 0x80)
			return 3;
		result += (unsigned int)(*curChar++ & 0x3f);
		outChar = result;
		return 3;
	}

	// 4字节
	if ((*curChar & 0xf8) == 0xf0)
	{
		result = (unsigned int)((*curChar++ & 0x07) << 18);
		if ((*curChar & 0xc0) != 0x80)
			return 4;
		result += (unsigned int)((*curChar++ & 0x3f) << 12);
		if ((*curChar & 0xc0) != 0x80)
			return 4;
		result += (unsigned int)((*curChar++ & 0x3f) << 6);
		if ((*curChar & 0xc0) != 0x80)
			return 4;
		result += (unsigned int)(*curChar++ & 0x3f);
		outChar = result;
		return 4;
	}
	outChar = 0;
	return 0;
}

void PrintText(const  char* textBegin, const char* textEnd = nullptr)
{
	if (textEnd == nullptr)
	{
		textEnd = textBegin + strlen(textBegin);
	}

	const char* curCharPtr = textBegin;
	while (curCharPtr < textEnd)
	{
		unsigned int curChar = (unsigned int)*curCharPtr;
		if (curChar < 0x80)
		{
			curCharPtr++;
		}
		else
		{
			curCharPtr += ParsedUtf8(curChar, curCharPtr, textEnd);
		}
		std::cout << curChar << std::endl;
	}

}

int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);

	//Test();
	App().Run();

	// Test parsed utf8
	string str = u8"测";
	PrintText(str.c_str());



	system("Pause");
	return 0;
}




