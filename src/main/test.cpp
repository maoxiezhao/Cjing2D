#include"test.h"
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

const string EnumInfoTraits<TestNumber>::prettyName = "Test";
const EnumInfo<TestNumber>::nameType EnumInfoTraits<TestNumber>::names = 
{
	{TestNumber::T_ONE,"one"},
	{TestNumber::T_TWO,"two"},
	{TestNumber::T_THREE,"three"}
};

void Test()
{
	string dataPath = ".";
	if (!FileData::OpenData("", dataPath))
		Debug::Die("No data file was found int the direcion:" + dataPath);


	ResourceCache::GetInstance().LoadDefaultProgram();

	FileData::CloseData();
}