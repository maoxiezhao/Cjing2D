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

int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);

	App().Run();

	system("Pause");
	return 0;
}



