#pragma once

#include<string>
#include<map>
#include<typeinfo>
#include<functional>
#include<vector>

#include"core\debug.h"

/**
*	\brief �򵥷�����Ƶ�ʵ��
*
*	�������ͨ����¼ÿ��ע���ʵ��ĺ��������������ķ���ֵ��Ϣ����
*	������Ϣ��
*/

namespace util
{
namespace Refl 
{

	class Instance;

	/**
	*	\brief ����Ԫ��Ϣ
	*
	*	Դ�����ṩ�˺����Ĳ�ѯ�Ͱ󶨲���
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
		*	\brief ����ָ������
		*	\param args �ú����Ĳ�����Ӧ�Ѿ���װΪinstance
		*/
		template<typename... Args>
		Instance Call(const std::string& funcName, Args&&... args);

		// ��ȡ��������
		FuncType& GetFunction(const std::string& funcName)
		{
			auto& it = mFunctions.find(funcName);
			if (it == mFunctions.end())
			{
				Debug::Error("Call the non-exists function.");
			}
			return it->second.funcType;
		}

		// ��ȡ�����б�
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
		*	\brief ע����ͨ����
		*/
		template<typename RetType, typename ...Args>
		void Function(const std::string& funcName, RetType(*f)(Args...));
		template<typename... Args>
		void Function(const std::string& funcName, void(*f)(Args...));
		/**
		*	\brief ע���Ա����
		*/
		/*	template<typename T, typename ReturnType, typename... Args>
		void Function(const std::string& funcName, ReturnType(T::*f)(Args...));*/
	private:
		std::string mName;
		const type_info& mTypeInfo;
		std::map<std::string, FuncInfo> mFunctions;
	};


	/**
	*	\brief �Զ�ע��ʵ��
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
	*	\brief ������Ԫ��Ϣ
	*/
	template<typename T>
	class ClassMeta : public Meta
	{
	public:
		ClassMeta() {}

		/**
		*	\brief �Ƿ��Զ�ע����
		*
		*	����ͨ��static variable�ķ�ʽ�ڳ������ʱ�Զ�ע��
		*/
		template<bool Enable = true>
		void EnableAutoRegister()
		{
			AutoRegisterClass<T>::Activate();
		}

		// ����ķ���Ĭ���ڹ���ʱ����
		/**
		*	\brief �Ƿ���Դ���
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
	*	\brief Ԫ���ݹ�����
	*
	*	ÿ��ע�������ClassMetaʵ������
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

		std::map<size_t, Meta> mMetas;				// type_info hascode��Ԫ����ӳ���
		std::map<std::string, size_t> mClassNames;	// ������type_info hascode��ӳ���
	};


	/**
	*	\brief �洢������ʵֵ
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
	*	\brief ����ʵ��
	*
	*	����ʵ�����������ݵ�ֵ��meta��Ϣ
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
	*	\brief ParamType ��������
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
	*	\brief ReturnType ����ֵ����
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