#pragma once

// gui core/8.8.2017 zy

#include<functional>
#include<queue>
#include<list>
#include<map>

#include"common\common.h"
#include"core\inputEvent.h"
#include"utils\point.h"
#include"utils\size.h"
#include"gui\core\handler.h"
#include"gui\core\message.h"

namespace gui
{

class Widget;
class Dispatcher;

/** �¼���Ӧ�Ļص����� */
using SignalFunction = std::function<void(Dispatcher& dispatcher, 
										const ui_event event, 
										bool&handle, 
										bool& halt)>;

using SignalFunctionMouse = std::function<void(Dispatcher& dispatcher, 
												const ui_event event, 
												bool&handle,
												bool& halt ,
												const Point2& coords)>;

using SignalFunctionKeyboard = std::function<void(Dispatcher& dispatcher,
												const ui_event event,
												bool& handle,
												bool& halt,
												const InputEvent::KeyboardKey key,
												const string & unicode)>;

using SignalFunctionMessage = std::function<void(Dispatcher& dispatcher,
												const ui_event event,
												bool& handle,
												bool& halt,
												Message& message)>;

using SignalFunctionNotification = std::function<void(Dispatcher& dispatcher,
												const ui_event event,
												bool& handle,
												bool& halt,
											    void*)>;
/**
*	\brief widget�Ļ���
*/
class Dispatcher
{
	friend struct Dispatcher_implementation;
public:
	Dispatcher();
	~Dispatcher();

	void Connect();

	virtual bool IsAt(const Point2& pos)const;

	/** ��괦����Ϊ,������dispathcer�����event����Ӧ��Ϊ*/
	enum mouse_behavior
	{
		all,
		hit,
		none,
	};

	void CaptureMouse()
	{
		gui::CaptureMouse(this);
	}

	void ReleaseMouse()
	{
		gui::ReleaseMouse(this);
	}

	/******  set/get *******/

	bool GetWantKeyboard()const
	{
		return mWantKeyboard;
	}
	void SetWantKeyboard(bool wantKeyboard)
	{
		mWantKeyboard = wantKeyboard;
	}

	mouse_behavior GetMouseBehavior()const
	{
		return  mMouseBehavior;
	}

	void SetMouseBehavior(const mouse_behavior mouseBehavior)
	{
		mMouseBehavior = mouseBehavior;
	}

	/******  ���������¼� *******/

	/** ͨ���¼� */
	bool Fire(const ui_event event, Widget& widget);

	/** ����¼� */
	bool Fire(const ui_event event, Widget& widget, const Point2& pos);

	/** �����¼� */
	bool Fire(const ui_event event, Widget& widget, const InputEvent::KeyboardKey key, const string & unicode);

	/** ��Ϣ�¼� */
	bool Fire(const ui_event event, Widget& widget, Message& message);

	/** ֪ͨ��Ӧ�¼� */
	bool Fire(const ui_event event, Widget& widget, void*data);

	/**
	*	\brief �ź���ӵ��źŶ����е�λ��
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

	/**
	*	\brief �źŶ�������
	*/
	enum event_queue_type
	{
		pre = 1,
		child = 2,
		post = 4
	};

	/** ��Ӧ�źŵ����Ӻ��� */

	/**
	*	\brief set event �źŴ���
	*/
	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEvent, int_<E> >::value>
	ConnectSignal(const SignalFunction& signal, const queue_position position = back_child)
	{
		mQueueSignal.ConnectSignal(E, position, signal);
	}

	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEvent, int_<E> >::value>
	DisconnectSignal(const SignalFunction& signal, const queue_position position = back_child)
	{
		mQueueSignal.DisconnectSignal(E, position, signal);
	}
	/**
	*	\brief mouse event �źŴ���
	*/
	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventMouse, int_<E> >::value>
		ConnectSignal(const SignalFunctionMouse& signal, const queue_position position = back_child)
	{
		mQueueSignalMouse.ConnectSignal(E, position, signal);
	}

	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventMouse, int_<E> >::value>
		DisconnectSignal(const SignalFunctionMouse& signal, const queue_position position = back_child)
	{
		mQueueSignalMouse.DisconnectSignal(E, position, signal);
	}

	template<typename T>
	std::enable_if_t<std::is_same<setEventMouse, T>::value, bool>
		HasSignal( event_queue_type type = event_queue_type::child)
	{
		return mQueueSignalMouse.HasSignal(type);
	}

	/**
	*	\brief keyborad event �źŴ���
	*/
	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventKeyboard, int_<E> >::value>
		ConnectSignal(const SignalFunctionKeyboard& signal, const queue_position position = back_child)
	{
		mQueueSignalKeyboard.ConnectSignal(E, position, signal);
	}

	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventKeyboard, int_<E> >::value>
		DisconnectSignal(const SignalFunctionKeyboard& signal, const queue_position position = back_child)
	{
		mQueueSignalKeyboard.DisconnectSignal(E, position, signal);
	}

	template<typename T>
	std::enable_if_t<std::is_same<setEventKeyboard, T>::value, bool>
		HasSignal( event_queue_type type = event_queue_type::child)
	{
		return mQueueSignalKeyboard.HasSignal(type);
	}

	/**
	*	\brief message event �źŴ���
	*/
	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventMessage, int_<E> >::value>
	ConnectSignal(const SignalFunctionMessage& signal, const queue_position position = back_child)
	{
		mQueueSignalMessage.ConnectSignal(E, position, signal);
	}
	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventMessage, int_<E> >::value>
	DisconnectSignal(const SignalFunctionMessage& signal, const queue_position position = back_child)
	{
		mQueueSignalMessage.DisconnectSignal(E, position, signal);
	}

	/**
	*	\brief notification event �źŴ���
	*/
	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventNotification, int_<E> >::value>
		ConnectSignal(const SignalFunctionNotification& signal, const queue_position position = back_child)
	{
		mQueueSignalNotification.ConnectSignal(E, position, signal);
	}
	template<ui_event E>
	std::enable_if_t<util::typeset_exist<setEventNotification, int_<E> >::value>
		DisconnectSignal(const SignalFunctionNotification& signal, const queue_position position = back_child)
	{
		mQueueSignalNotification.DisconnectSignal(E, position, signal);
	}

	/**
	*	\brief �ź�����
	*
	*	�ź����ͱ����и��ź����͵�3���ص�����
	*/
	template<typename T>
	struct SignalType
	{
		SignalType() :mPreChild(), mChild(), mPostChild(){}

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

		/** ���һ����Ӧ�źŵĻص� */
		void ConnectSignal(const ui_event event, const queue_position position, const T& signal)
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
				mQueue[event].mChild.push_front(signal);
				break;
			case back_child:
				mQueue[event].mChild.push_back(signal);
				break;
			case front_post_child:
				mQueue[event].mPostChild.push_front(signal);
				break;
			case back_post_child:
				mQueue[event].mPostChild.push_back(signal);
				break;
			}
		}

		/** ɾ��һ����Ӧ�źŵĻص� */
		void DisconnectSignal(const ui_event event, const queue_position position, const T& singal)
		{
			switch (position)
			{
			case front_pre_child:
			case back_pre_child:
				mQueue[event].mPreChild.remove_if(
					[&singal](T& element){return singal.target_type() == element.target_type(); });
				break;
			case front_child:
			case back_child:
				mQueue[event].mChild.remove_if(
					[&singal](T& element){return singal.target_type() == element.target_type(); });
				break;
			case front_post_child:
			case back_post_child:
				mQueue[event].mPostChild.remove_if(
					[&singal](T& element){return singal.target_type() == element.target_type(); });
				break;
			}
		}

		/** �ж��Ƿ�����ź� */
		bool HasSignal(event_queue_type type)
		{
			bool result = false;
			switch (type)
			{
			case gui::Dispatcher::pre:
				for (auto& kvp : mQueue)
				{
					auto& eventList = kvp.second;
					if (!eventList.mPreChild.empty())
					{
						result = true;
						break;
					}
				}
				break;
			case gui::Dispatcher::child:
				for (auto& kvp : mQueue)
				{
					auto& eventList = kvp.second;
					if (!eventList.mChild.empty())
					{
						result = true;
						break;
					}
				}
				break;
			case gui::Dispatcher::post:
				for (auto& kvp : mQueue)
				{
					auto& eventList = kvp.second;
					if (!eventList.mPostChild.empty())
					{
						result = true;
						break;
					}
				}
				break;
			}
			return result;
		}
	};

	bool HasEvent(const ui_event event, const event_queue_type type);
private:
	bool mIsConnected;

	bool mWantKeyboard;

	mouse_behavior mMouseBehavior;

	/** ��Ӧ�źŵ��¼����� */
	SignalQueue<SignalFunction> mQueueSignal;

	SignalQueue<SignalFunctionMouse> mQueueSignalMouse;

	SignalQueue<SignalFunctionKeyboard> mQueueSignalKeyboard;

	SignalQueue<SignalFunctionMessage> mQueueSignalMessage;

	SignalQueue<SignalFunctionNotification> mQueueSignalNotification;
};


inline void ConnectSignalHandleNotifyModified(Dispatcher& dispatcher, const SignalFunctionNotification& signal)
{
	dispatcher.ConnectSignal<ui_event::UI_EVENT_NOTIFY_MODIFIED>(signal);
}

}
