#pragma once

#include"common\common.h"

#include<functional>
#include<array>
#include<vector>
#include<memory>

/**
*	\brief 行为树功能试做
*
*	该行为树在在C++端维护各个节点逻辑，初始化时可从.lua文件构建，
*	.lua文件提供条件节点和行动节点的回调函数，在C++中每帧去处理
*   各个节点
*/

/**
*	\brief 行为树节点基类
*/
class BevNode
{
public:
	BevNode();
	~BevNode();

	static const int MaxChildrenCount = 32;
private:
	std::array<std::shared_ptr<BevNode>, MaxChildrenCount> mChilds;

};
using BevNodePtr = std::shared_ptr<BevNode>;

class SelectionNode
{

};

class DequeueNode
{

};

//////////////////////////////////////////////////////////

/**
*	\brief 行为树
*/
class BevTree
{
public:
	BevTree();
	~BevTree();


private:
	BevNodePtr mRoot;
};

#include"utils\bevTree.inl"