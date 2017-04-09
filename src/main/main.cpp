#include<iostream>
#include<Windows.h>
#include"core\debug.h"
#include"game\app.h"

int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);

	App().Run();

	system("Pause");
	return 0;
}



