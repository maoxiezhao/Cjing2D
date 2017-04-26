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

int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);

	//Test();
	App().Run();

	system("Pause");
	return 0;
}



