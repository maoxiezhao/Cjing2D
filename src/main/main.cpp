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
#include"utils\grid.h"
#include<future>


int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);
	Debug::SetDebugStringEnable(false);

	//Test();
	App().Run();

	// Test Grid
	//util::Grid<int> mGrid({ 640, 480}, {320,240});
	//mGrid.Add(10, { 0,0,100,100 });

	//auto& ints = mGrid.GetElements({ 0,0,500,500 });

	//system("Pause");
	return 0;
}




