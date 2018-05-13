#pragma once

// gui core/8.8.2017 zy

#include"common\common.h"
#include"core\inputEvent.h"
#include"core\debug.h"
#include"utils\typeSet.h"
#include"gui\core\message.h"

#include<type_traits>

namespace gui
{
	struct Dispatcher_implementation
	{
		/**
		*	\brief 这里使用宏来实现所有的事件信号类型SignalType
		*/
#define IMPLEMENT_EVENT_SIGNA(SET,FUNCTION,QUEUE)						\
		/**																\
		*	\brief 获取对应dispatcher的SignalType						\
		*	\param K	 事件集											\
		*	\param event 传入事件										\
		*/																\
		template<typename K>											\
		static std::enable_if_t<util::typeset_exist<SET, K>::value, Dispatcher::SignalType<FUNCTION> >& \
			EventSinal(Dispatcher& dispatcher, const ui_event event)	\
		{																\
			return dispatcher.QUEUE.mQueue[event];						\
		}																\
		/**																\
		*	\brief 获取对应dispatcher的SignalType						\
		*	\param F 信号函数											\
		*	\param event 传入事件										\
		*/																\
		template<typename F>											\
		static std::enable_if_t<std::is_same<F, FUNCTION>::value, Dispatcher::SignalType<FUNCTION> >& \
		EventSinal(Dispatcher& dispatcher, const ui_event event)		\
		{																\
			return dispatcher.QUEUE.mQueue[event];						\
		}	

		IMPLEMENT_EVENT_SIGNA(setEvent, SignalFunction, mQueueSignal)

#define IMPLEMENT_EVENT_SIGNAL_WRAPPER(TYPE)		 \
			IMPLEMENT_EVENT_SIGNA(setEvent##TYPE,	   \
								  SignalFunction##TYPE, \
								  mQueueSignal##TYPE)

			/**
			*	\brief 添加的事件需要在这里实现
			*/
			IMPLEMENT_EVENT_SIGNAL_WRAPPER(Mouse)
			IMPLEMENT_EVENT_SIGNAL_WRAPPER(Keyboard)
			IMPLEMENT_EVENT_SIGNAL_WRAPPER(Notification)
			IMPLEMENT_EVENT_SIGNAL_WRAPPER(Message)

#undef IMPLEMENT_EVENT_SIGNAL_WRAPPER
#undef IMPLEMENT_EVENT_SIGNA

		/**
		*	\brief HasHandler以伪函数形式用于帮组查找一个dispahcer
		*	是否能够操作一个事件，即该singal_queue的信号中是否不为空
		*/
		class HasHandler
		{
		public:
			HasHandler(const Dispatcher::event_queue_type type, Dispatcher& dispatcher) :
				mEventQueueType(type),
				mDispatcher(dispatcher)
			{
			}

			/**
			*	\brief 检测是否存在事件
			*	\param T 信号函数
			*	\param event 传入的事件
			*/
			template<typename T>
			bool operator()(ui_event event)
			{
				if ((mEventQueueType & Dispatcher::pre) &&
					!EventSinal<T>(mDispatcher,event).mPreChild.empty())
				{
					return true;
				}

				if ((mEventQueueType & Dispatcher::child) &&
					!EventSinal<T>(mDispatcher, event).mChild.empty())
				{
					return true;
				}

				if ((mEventQueueType & Dispatcher::post) &&
					!EventSinal<T>(mDispatcher, event).mPostChild.empty())
				{
					return true;
				}

				return false;
			}

		private:
			Dispatcher::event_queue_type mEventQueueType;
			Dispatcher& mDispatcher;
		};
	};


	/**
	*	\brief 模板特例化实现
	*/
	namespace implementation
	{
		// 递归终结
		template<bool done = true>
		struct Find
		{
			template<typename EventSet, typename itor, typename end, class E, class F>
			static bool execute(itor*, end*, const ui_event event, F function)
			{
				return false;
			}
		};

		// 循环遍历typeset
		template<>
		struct Find<false>
		{
			template<typename EventSet, typename itor, typename end, class E, class F>
			static bool execute(itor*, end*, const ui_event event, F function)
			{
				using currItor = util::typeset_next<EventSet, itor>::type;
				if (currItor::value == event)
				{
					return function.operator()<currItor>(event);
				}
				else
				{
					return implementation::Find<std::is_same<currItor, end>::value>
						::execute<EventSet, currItor, end, E, F>(
							static_cast<currItor*>(nullptr),
							static_cast<end*>(nullptr),
							event,
							function);
				}
			}
		};
	}

	/**
	*	\brief 通用的事件查找函数，条件为F的函数
	*/
	template<typename EventSet, class E, class F>
	bool Find(E event, F func)
	{
		using begin = util::typeset_begin<EventSet>::type;
		using end   = util::typeset_end<EventSet>::type;

		if (begin::value == event)
		{
			return func.operator()<begin>(event);
		}
		else
		{
			return implementation::Find<std::is_same<begin, end>::value>
				::execute<EventSet, begin, end, E, F>(
				static_cast<begin*>(nullptr),
				static_cast<end*>(nullptr), 
				event, 
				func);
		}
	}

	namespace implementation
	{
		/**
		*	\brief 获取一个当前widge事件传递链
		*	\param event 传递的event
		*	\param dispathcer 最上层的dispathcer
		*	\param w 事件的接受者
		*
		*	事件链从该widget开始，向父类传递直到dispatcher停止
		*/
		template<typename T>
		inline std::vector<std::pair<Widget*, ui_event> > 
			BuildEventChain(const ui_event event, Widget*dispatcher, Widget* w)
		{
			Debug::CheckAssertion(dispatcher != nullptr);
			Debug::CheckAssertion(w != nullptr);

			std::vector<std::pair<Widget*, ui_event> > eventChain;

			while (true)
			{
				if (w->HasEvent(event, Dispatcher::event_queue_type(Dispatcher::pre | Dispatcher::post)) )
				{
					eventChain.emplace_back(w, event);
				}

				if (w == dispatcher)
				{
					break;
				}

				w = w->GetParent();
				if (w == nullptr)
				{
					break;
				}
			}

			return eventChain;
		}

		template<typename T, typename... F>
		inline bool FireEvent(const ui_event event, std::vector<std::pair<Widget*, ui_event> >& eventChain, 
					Widget*dispatcher, Widget* w, F&&... params)
		{
			bool handle = false;		// 拦截标记
			bool halt = false;			// 停止标记

			std::vector<std::pair<Widget*, ui_event> > reverseEventChain = eventChain;
			std::reverse(reverseEventChain.begin(), reverseEventChain.end());

			/****** pre ****/
			/**
			*	\从顶端开始往下遍历处理preChild的信号
			*/
			for (auto& widget_kvp : reverseEventChain)
			{
				Dispatcher::SignalType<T>& signalType =
					Dispatcher_implementation::EventSinal<T>(*widget_kvp.first, widget_kvp.second);

				for (auto& preFunc : signalType.mPreChild)
				{
					// Func Define:
					// std::function<void(Dispatcher& dispatcher,const ui_event event,bool&handle,bool& halt)>;
					preFunc(*dispatcher, widget_kvp.second, handle, halt, std::forward<F>(params)...);
					if (halt)
					{
						break;
					}
				}

				if (handle)
				{
					return true;
				}
			}

			/****** child ****/
			if (w->HasEvent(event, Dispatcher::child))
			{
				Dispatcher::SignalType<T>& signalType = 
					Dispatcher_implementation::EventSinal<T>(*w, event);

				for (auto& func : signalType.mChild)
				{
					// Func Define:
					// std::function<void(Dispatcher& dispatcher,const ui_event event,bool&handle,bool& halt)>;
					func(*dispatcher, event, handle, halt, std::forward<F>(params)...);
					if (halt)
					{
						break;
					}
				}
				if (handle)
				{
					return true;
				}
			}

			/****** post ****/
			for (auto& widget_kvp : eventChain)
			{
				Dispatcher::SignalType<T>& signalType =
					Dispatcher_implementation::EventSinal<T>(*widget_kvp.first, widget_kvp.second);

				for (auto& postFunc : signalType.mPostChild)
				{
					// Func Define:
					// std::function<void(Dispatcher& dispatcher,const ui_event event,bool&handle,bool& halt)>;
					postFunc(*dispatcher, widget_kvp.second, handle, halt, std::forward<F>(params)...);
					if (halt)
					{
						break;
					}
				}
				if (handle)
				{
					return true;
				}
			}
			return false;
		}
	}

	template<typename T, typename... F>
	inline bool FireEvent(const ui_event event, Dispatcher*dispatcher, Widget* w, F&&... params)
	{
		if (dispatcher == nullptr || w == nullptr)
			return false;
		
		Widget* dispatcherW = dynamic_cast<Widget*>(dispatcher);
		std::vector<std::pair<Widget*, ui_event> > eventChain =
			implementation::BuildEventChain<T>(event, dispatcherW, w);

		return implementation::FireEvent<T>(event, eventChain, dispatcherW, w, std::forward<F>(params)...);
	}

}