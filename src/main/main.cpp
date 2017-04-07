#include<iostream>
#include<Windows.h>
#include"common\common.h"
#include"lua\luaContext.h"
#include"core\fileData.h"
#include"core\logger.h"
#include"core\debug.h"
int main()
{
//	LuaContext l;
//	l.Initialize();
	FileData::OpenData("",".");

	LuaContext l;
	l.Initialize();

	Logger::Error("The Name is Nil.");
	Logger::Warning("the time losed much than 2 times");

	Debug::CheckAssertion(2 > 10, "the first number must much than second.");
	

	FileData::CloseData();
	system("Pause");
	MessageBox(NULL, TEXT("hello world"), NULL, MB_OK);
	return 0;
}



