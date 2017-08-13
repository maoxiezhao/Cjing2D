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

//#include"gui\core\dispatcher.h"
#include"utils\typeSet.h"

enum test_event
{
	event_1,
	event_2,
	event_3,
	event_4
};
template<typename T, test_event V>
using int_ = std::integral_constant<T, V>;

using MouseEventSet = util::typeset<int_<int, event_1>,
	int_<int, event_2>,
	int_<int, event_3 >> ;

int main(int argc,char** argv)
{
	Debug::SetAbortOnDie(true);

	//Test();
	App().Run();

	// test gui
	//gui::Dispatcher mDispatcher;


	//system("Pause");
	return 0;
}



