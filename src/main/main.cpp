#include<iostream>
#include<Windows.h>
#include"core\debug.h"
#include"game\app.h"
#include"core\random.h"
#include"core\inputEvent.h"
#include"utils\size.h"
int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);

	App().Run();

	system("Pause");
	return 0;
}



