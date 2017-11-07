#pragma once

#include<string>
#include<map>
#include<typeinfo>
#include<functional>
#include<vector>

#include"core\debug.h"

/**
*	\brief 简单反射机制的实现
*
*	反射机制通过记录每个注册的实体的函数，包括函数的返回值信息，和
*	参数信息。
*/

namespace util
{
namespace Refl 
{

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
		*	\brief 调用指定函数
		*	\param args 该函数的参数，应已经包装为instance
		*/
		template<typename... Args>
		Instance Call(const std::string& funcName, Args&&... args);

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
		/*	template<typename T, typename ReturnType, typename... Args>
		void Function(const std::string& funcName, ReturnType(T::*f)(Args...));*/
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
			/*		Function( std::string("_Init_") + std::to_string(sizeof...(Args)),
			[&](Args&&
			);*/
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
		template<typename T>
		static Instance Make(T o)
		{
			Instance i;
			i.mMeta = &MetaManager::GetInstance().GetMeta<T>();
			i.mValue.reset(new TypeInstanceValue<T>(std::move(o)));

			return std::move(i);
		}

		Instance() :mMeta(nullptr) {}
		Instance(Instance&& i) :
			mMeta(i.mMeta), mValue(std::move(i.mValue)) {}
		Instance(const Instance& i) = delete;

		Meta* GetMeta() { return mMeta; }

		template<typename T>
		T& GetValue()
		{
			if (mMeta->IsSame<T>())
			{
				auto p = dynamic_cast<TypeInstanceValue<T>*>(mValue.get());
				return p->mData;
			}
		}

	private:
		Meta* mMeta;
		std::unique_ptr<InstanceValue> mValue;
	};


	/**
	*	\brief ParamType 参数类型
	*/
	template<typename T>
	class ParamType
	{
	public:
		T& Convert(Instance& instance)
		{
			Meta* meta = instance.GetMeta();
			if (meta != nullptr && meta->IsSame<T>())
			{
				return instance.GetValue<T>();
			}
		}
	};

	/**
	*	\brief ReturnType 返回值类型
	*/
	template<typename T>
	class ReturnType
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