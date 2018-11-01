#pragma once

#include"common\common.h"

#include<functional>
#include<array>
#include<vector>
#include<memory>

/**
*	\brief ��Ϊ����������
*
*	����Ϊ������C++��ά�������ڵ��߼�����ʼ��ʱ�ɴ�.lua�ļ�������
*	.lua�ļ��ṩ�����ڵ���ж��ڵ�Ļص���������C++��ÿ֡ȥ����
*   �����ڵ�
*/

/**
*	\brief ��Ϊ���ڵ����
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
*	\brief ��Ϊ��
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