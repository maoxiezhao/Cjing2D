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

/** 事件对应的回调函数 */
using SignalFunction = std::function<void(Dispatcher& dispatcher, const ui_event event, bool&handle, bool& hald)>;

using SignalMouseFunction = std::function<void(Dispatcher& dispatcher, const ui_event event, bool&handle, bool& hald ,
											const Point2& coords)>;

/**
*	\brief widget的基类
*/
class Dispatcher
{
public:
	Dispatcher();
	~Dispatcher();

	void Connect();

	virtual bool IsAt(const Point2& pos)const = 0;

	/**  触发各类事件 */

	void Fire(const ui_event event, Widget& widget);

	void Fire(const ui_event event, Widget& widget, const Point2& pos);

	/**
	*	\brief 信号添加到信号队列中的位置
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

	/** 对应信号的链接函数 */
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
	*	\brief 信号类型
	*
	*	信号类型保存有该信号类型的3个回调队列
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
	*	\brief 信号类型的事件队列
	*
	*	存储一个信号类型对所有事件的回队列
	*/
	template<typename T>
	struct SignalQueue
	{
		SignalQueue() :mQueue()
		{
		}

		std::map<ui_event, SignalType<T> > mQueue;

		/** 添加一个对应信号的回调 */
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

		/** 删除一个对应信号的回调 */
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


	/** 鼠标处理行为,决定该dispathcer对鼠标event的响应行为*/
	enum mouse_behavior
	{
		all,
		none,
	};

private:
	mouse_behavior mMouseBehavior;

	/** 对应信号的事件队列 */
	SignalQueue<SignalFunction> mSignalQueue;

	SignalQueue<SignalMouseFunction> mSignalMouseQueue;

	bool mIsConnected;

};


}
