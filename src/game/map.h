#ifndef _MAP_H_
#define _MAP_H_

#include"common\common.h"
#include"core\inputEvent.h"

/**
*	\brief map¿‡
*/
class Map
{
public:
	Map();
	~Map();

	void Update();
	void Draw();
	bool NotifyInput(const InputEvent & ent);

private:

};


#endif