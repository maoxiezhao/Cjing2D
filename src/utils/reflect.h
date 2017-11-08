#pragma once

#include<string>
#include<map>
#include<typeinfo>
#include<functional>
#include<vector>
#include<type_traits>
#include<memory>

#include"core\debug.h"

/**
*	\brief 简单反射机制的实现
*
*	反射机制通过记录每个注册的实体的函数，包括函数的返回值信息，和
*	参数信息。
*/

namespace util
{
	namespace refel{
	class Instance;

	/**
	*	\brief 数据元信息
	*
	*	源数据提供了函数的查询和绑定操作
	*/
	class Meta {
	public:
		using FuncType = std::function<Instance(Instance*[], size_t argc)>;
		using FuncArgVector = std::vector<Meta*>;
		struct FuncInfo
		{
			FuncType funcType;
			FuncArgVector funcArgs;
		};
		Meta(const std::string& name, const type_info& typeInfo) :
			mName(name), mTypeInfo(typeInfo) {};

		/** status */
		template<typename T>
		bool IsSame()const
		{
			return typeid(T).hash_code() == mTypeInfo.hash_code();
		}

		const type_info& GetTypeInfo() { return mTypeInfo; }

		bool HasFunction(const std::string& funcName)const
		{
			auto& it = mFunctions.find(funcName);
			if (it == mFunctions.end())
			{
				return false;
			}
			return true;
		}

		/**
		*	\brief 执行指定函数
		*	\param args 该函数的参数，应已经包装为instance
		*/
		Instance Apply(Instance* args[], size_t argc, const std::string& funcName);

		/**
		*	\brief 调用指定函数
		*
		*	该函数处理参数并传递给apply
		*/
		template<typename... Args>
		Instance Call(const std::string& funcName, Args&&... args);

		/**
		*	\brief 调用指定函数
		*
		*	该函数处理参数并传递给apply,保证传入的参数全部
		*	已经instance
		*/
		template<typename... Args>
		Instance CallByInstances(const std::string& funcName, Args&&... args);

		// 获取函数类型
		FuncType& GetFunction(const std::string& funcName)
		{
			auto& it = mFunctions.find(funcName);
			if (it == mFunctions.end())
			{
				Debug::Error("Call the non-exists function.");
			}
			return it->second.funcType;
		}

		// 获取参数列表
		FuncArgVector& GetFuncArgs(const std::string& funcName)
		{
			auto& it = mFunctions.find(funcName);
			if (it == mFunctions.end())
			{
				Debug::Error("Call the non-exists function.");
			}
			return it->second.funcArgs;
		}

		/**
		*	\brief 注册普通函数
		*/
		template<typename RetType, typename ...Args>
		void Function(const std::string& funcName, RetType(*f)(Args...));
		template<typename... Args>
		void Function(const std::string& funcName, void(*f)(Args...));
		/**
		*	\brief 注册成员函数
		*/
		template<typename T, typename ReturnType, typename... Args>
		void Function(const std::string& funcName, ReturnType(T::*f)(Args...));
		template<typename T, typename... Args>
		void Function(const std::string& funcName, void(T::*f)(Args...));

	private:
		std::string mName;
		const type_info& mTypeInfo;
		std::map<std::string, FuncInfo> mFunctions;
	};


	/**
	*	\brief 自动注册实现
	*/
	template<typename T>
	struct AutoRegisterClass
	{
		AutoRegisterClass()
		{
			//std::cout << "Auto Register." << std::endl;
		}

		virtual void DyActivate() {};
		static void Activate() { instance;/*instance.DyActivate();*/ };

		static AutoRegisterClass instance;
	};

	template<typename T>
	AutoRegisterClass<T> AutoRegisterClass<T>::instance;

	/**
	*	\brief 类数据元信息
	*/
	template<typename T>
	class ClassMeta : public Meta
	{
	public:
		ClassMeta() {}

		/**
		*	\brief 是否自动注册类
		*
		*	该类通过static variable的方式在程序加载时自动注册
		*/
		template<bool Enable = true>
		void EnableAutoRegister()
		{
			AutoRegisterClass<T>::Activate();
		}

		// 下面的方法默认在构造时调用
		/**
		*	\brief 是否可以创建
		*/
		template<typename... Args>
		void EnableConstruct()
		{
			Function(std::string("_Init_") + std::to_string(sizeof...(Args)),
				Init<Args...>);
		}

	private:
		template<typename... Args>
		static T Init(Args... args)
		{
			return T(std::forward<Args>(args)...);
		}
	};


	/**
	*	\brief 元数据管理器
	*
	*	每个注册的类以ClassMeta实例保存
	*/
	class MetaManager
	{
	public:
		MetaManager(const MetaManager& other) = delete;
		MetaManager& operator=(const MetaManager& other) = delete;

		void Initialize()
		{
			RegisterClass<int>("int");
			RegisterClass<double>("double");
			RegisterClass<bool>("bool");
			RegisterClass<std::string>("string");
			RegisterClass<void*>("pointer");
		}

		template<typename T>
		ClassMeta<T>& RegisterClass(const std::string& className)
		{
			auto i = mClassNames.find(className);
			if (i != mClassNames.end())
			{
				Debug::Error("The class'" + className + "' has already registered.");
			}
			size_t hashCode = typeid(T).hash_code();
			mClassNames[className] = hashCode;

			return static_cast<ClassMeta<T>&>(mMetas.insert(
				std::make_pair(hashCode, Meta(className, typeid(T)))).first->second);
		}

		template<typename T>
		Meta& GetMeta()
		{
			size_t hashCode = typeid(T).hash_code();
			auto it = mMetas.find(hashCode);
			if (it == mMetas.end())
			{
				Debug::Error("The class has not registered.");
			}
			return it->second;
		}

		Meta& GetMeta(const std::string& metaName)
		{
			// get hash code
			auto it = mClassNames.find(metaName);
			if (it == mClassNames.end())
			{
				Debug::Error("The class has not registered.");
			}

			// get meta
			size_t hashCode = it->second;
			auto metaIt = mMetas.find(hashCode);
			if (metaIt == mMetas.end())
			{
				Debug::Error("The class has not registered.");
			}
			return metaIt->second;
		}

		static MetaManager& GetInstance()
		{
			static MetaManager metaManager;
			return metaManager;
		}

	private:
		MetaManager() {}

		std::map<size_t, Meta> mMetas;				// type_info hascode和元数据映射表
		std::map<std::string, size_t> mClassNames;	// 类名和type_info hascode的映射表
	};

	///////////////////////////////////////////////////////////////////
	/**
	*	\brief 存储数据真实值
	*/
	class InstanceValue
	{
	public:
		virtual ~InstanceValue() {};
	};

	template<typename T>
	class TypeInstanceValue : public InstanceValue
	{
	public:
		virtual ~TypeInstanceValue() {}
		TypeInstanceValue(TypeInstanceValue&& rhs) :mData(std::move(rhs.mData)) {}
		TypeInstanceValue(const TypeInstanceValue& rhs) :mData(rhs.mData) {}
		TypeInstanceValue(T && rhs) :mData(std::move(rhs)) {}
		TypeInstanceValue(const T& rhs) :mData(rhs) {}

		T mData;
	};
	/**
	*	\brief 数据实例
	*
	*	数据实例包含了数据的值和meta信息
	*/
	class Instance
	{
	public:
		using InstancePtr = std::unique_ptr<Instance>;
		// 创建非指针数据instance
		template<typename T,
			typename Enable = typename std::enable_if<!std::is_pointer<T>::value>::type >
			static Instance Make(T o)
		{
			Instance i;
			i.mMeta = &MetaManager::GetInstance().GetMeta<T>();
			i.mValue.reset(new TypeInstanceValue<
				typename std::remove_reference<T>::type>(std::move(o)));

			return std::move(i);
		}

		// 创建指针数据instance,mMeta为void*
		template<typename T>
		static Instance Make(const T* p)
		{
			Instance i = Instance::Make<void*, void>(const_cast<T*>(p));
			i.mPointerMeta = &MetaManager::GetInstance().GetMeta<T>();
			return std::move(i);
		}


		Instance() : mMeta(nullptr), mPointerMeta(nullptr), mValue(nullptr) {}
		Instance(Instance&& i) :
			mMeta(i.mMeta), mPointerMeta(i.mPointerMeta), mValue(std::move(i.mValue)) {}
		Instance(const Instance& i) = delete;

		Meta* GetMeta() { return mMeta; }
		Meta* GetPointerMeta() { return mPointerMeta; }
		bool IsPointer()const { return mMeta->IsSame<void*>(); }

		/**
		*	\brief 获取当前instance值
		*/
		template<typename T>
		T& GetValue()
		{
			// 非指针
			if (mMeta->IsSame<T>())
			{
				auto p = dynamic_cast<TypeInstanceValue<T>*>(mValue.get());
				return p->mData;
			}

			// 指针数据，且数据可能
			if (!IsPointer())
			{
				Debug::Error("Unable to get the value");
			}
			auto p = dynamic_cast<TypeInstanceValue<void*>*>(mValue.get());
			return *static_cast<T*>(p->mData);
		}

	private:
		Meta* mMeta;	    // 类型信息，如果是指针则为void*
		Meta* mPointerMeta;	// 指针类型信息
		std::unique_ptr<InstanceValue> mValue;
	};

	///////////////////////////////////////////////////////////////////
	/**
	*	\brief ParamType 参数类型
	*/
	template<typename T>
	struct ParamType
	{
	public:
		using ParamT = typename std::remove_cv<T>::type;
		ParamT& Convert(Instance& instance)
		{
			const Meta* meta = instance.GetMeta();
			if (meta != nullptr && meta->IsSame<ParamT>())
			{
				return instance.GetValue<ParamT>();
			}
		}
	};

	template<typename T>
	struct ParamType<T*>
	{
	public:
		using ParamT = typename std::remove_cv<T>::type;
		ParamT*& Convert(Instance& instance)
		{
			const Meta* meta = instance.GetMeta();
			if (meta == nullptr || !instance.IsPointer())
			{
				Debug::Error("Convert un-pointer instance.");
			}

			const Meta* pMeta = instance.GetPointerMeta();
			if (pMeta != nullptr && pMeta->IsSame<ParamT>())
			{
				return *reinterpret_cast<ParamT**>(&instance.GetValue<void*>());
			}
		}
	};

	template<typename T>
	struct ParamType<T&> : ParamType<T>, ParamType<T*>
	{
	public:
		using ParamT = typename std::remove_cv<T>::type;
		ParamT& Convert(Instance& instance)
		{
			if (instance.IsPointer())
			{
				return *ParamType<T*>::Convert(instance);
			}
			else
			{
				return ParamType<T>::Convert(instance);
			}
		}
	};

	/**
	*	\brief ReturnType 返回值类型
	*/
	template<typename T>
	struct ReturnType
	{
	public:
		Instance Convert(T t)
		{
			return Instance::Make<T>(t);
		}
	};

#include"utils\reflect.inl"
	}
}