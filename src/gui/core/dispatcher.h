#pragma once

#include<functional>
#include<queue>
#include<list>
#include<map>

#include"common\common.h"
#include"utils\point.h"
#include"utils\size.h"
#include"gui\core\handler.h"

namespace gui
{

class Widget;
class Dispatcher;

/** �¼���Ӧ�Ļص����� */
using SignalFunction = std::function<void(Dispatcher& dispatcher, const ui_event event, bool&handle, bool& hald)>;

using SignalMouseFunction = std::function<void(Dispatcher& dispatcher, const ui_event event, bool&handle, bool& hald ,
											const Point2& coords)>;

/**
*	\brief widget�Ļ���
*/
class Dispatcher
{
public:
	Dispatcher();
	~Dispatcher();

	void Connect();

	/**  ���������¼� */

	void Fire(const ui_event event, Widget& widget);

	/**
	*	\brief �ź����ӵ��źŶ����е�λ��
	*/
	enum queue_position
	{
		front_pre_child,
		back_pre_child,
		front_child,
		back_child,
		front_post_child,
		back_post_child
	};

	/** ��Ӧ�źŵ����Ӻ��� */
	template<ui_event E>
	void ConnectSignal(const SignalFunction& signal, const queue_position position = back_child)
	{
		mSignalQueue.ConnectSignal(E, position, signal);
	}

	template<ui_event E>
	void DisconnectSignal(const SignalFunction& signal, const queue_position position = back_child)
	{
		mSignalQueue.DisconnectSignal(E, position, signal);
	}

	/**
	*	\brief �ź�����
	*
	*	�ź����ͱ����и��ź����͵�3���ص�����
	*/
	template<typename T>
	struct SignalType
	{
		SignalType() :mPostChild(), mChild(), mPostChild(){}

		std::list<T> mPreChild;
		std::list<T> mChild;
		std::list<T> mPostChild;
	};

	/**
	*	\brief �ź����͵��¼�����
	*
	*	�洢һ���ź����Ͷ������¼��Ļض���
	*/
	template<typename T>
	struct SignalQueue
	{
		SignalQueue() :mQueue()
		{
		}

		std::map<ui_event, SignalType<T> > mQueue;

		/** ����һ����Ӧ�źŵĻص� */
		void ConnectSignal(const ui_event event, const queue_position position, const T& singal)
		{
			switch (position)
			{
			case front_pre_child:
				mQueue[event].mPreChild.push_front(signal);
				break;
			case back_pre_child:
				mQueue[event].mPreChild.push_back(signal);
				break;
			case front_child:
				mQueue[event].mPreChild.push_front(signal);
				break;
			case back_child:
				mQueue[event].mPreChild.push_back(signal);
				break;
			case front_post_child:
				mQueue[event].mPreChild.push_front(signal);
				break;
			case back_post_child:
				mQueue[event].mPreChild.push_back(signal);
				break;
			}
		}

		/** ɾ��һ����Ӧ�źŵĻص� */
		void DisconnectSignal(const ui_event event, const queue_position position, const T& singal)
		{
			SignalType<T>& signalType = queue[event];
			switch (position)
			{
			case front_pre_child:
			case back_pre_child:
				mQueue[event].mPreChild.remove_if(
					[&singal](T& element){return singal == element; });
				break;
			case front_child:
			case back_child:
				mQueue[event].mChild.remove_if(
					[&singal](T& element){return singal == element; });
				break;
			case front_post_child:
			case back_post_child:
				mQueue[event].mPostChild.remove_if(
					[&singal](T& element){return singal == element; });
				break;
			}
		}
	};


private:
	/** ��Ӧ�źŵ��¼����� */
	SignalQueue<SignalFunction> mSignalQueue;

	SignalQueue<SignalMouseFunction> mSignalMouseQueue;

	bool mIsConnected;

};


}