#pragma once

inline Instance util::Meta::Apply(Instance* args[], size_t argc, const std::string& funcName)
{
	auto funcInfo = mFunctions.find(funcName);
	if (funcInfo == mFunctions.end())
	{
		Debug::Error("Call the non-exists meta function.");
	}
	FuncType funcType = funcInfo->second.funcType;
	Instance result = funcType(args, argc);

	return std::move(result);
}

namespace Implemention
{
	inline void FuncParamWrapper(std::vector<Instance>& argVector, size_t index) {}

	template<typename Head, typename... Args>
	inline void FuncParamWrapper(std::vector<Instance>& argVector, size_t index, Head&& head, Args&&... args)
	{
		argVector.push_back(Instance::Make<Head>(std::forward<Head>(head)));
		FuncParamWrapper(argVector, index + 1, std::forward<Args>(args)...);
	}
}

template<typename ...Args>
inline Instance util::Meta::Call(const std::string& funcName, Args&&... args)
{
	std::vector<Instance> mInstances;
	std::vector<Instance*> mPointerInstance;
	Implemention::FuncParamWrapper(mInstances, 0, std::forward<Args>(args)...);
	for (auto& instance : mInstances)
	{
		mPointerInstance.push_back(&instance);
	}
	return Apply(mPointerInstance.data(), mInstances.size(), funcName);
}

template<typename ...Args>
inline Instance util::Meta::CallByInstances(const std::string& funcName, Args&&... args)
{
	Instance* instances[sizeof...(args)] = { &const_cast<Instance&>(args)... };
	return Apply(instances, sizeof...(args), funcName);
}

///////////////////////////////////////////////////////////////////

namespace Implemention
{
	template<typename Ret>
	inline void FuncRecord(std::function<Ret()> f, Meta::FuncArgVector& argVector) {}

	template<typename Ret, typename Head, typename... Args>
	inline void FuncRecord(std::function<Ret(Head, Args...)> f, Meta::FuncArgVector& argVector)
	{
		Meta& meta = MetaManager::GetInstance().GetMeta<Head>();
		argVector.push_back(&meta);

		FuncRecord(std::function<Ret(Args...)>(), argVector);
	}
}

/**
*	\brief 记录函数的参数信息
*/
template<typename RetType, typename... Args>
inline void RecordFunctionArgs(std::function<RetType(Args...)> f, Meta::FuncArgVector& argVector)
{
	Implemention::FuncRecord(f, argVector);
}

///////////////////////////////////////////////////////////////////

/**
*	\brief 函数Caller，用于对每一个instance转换成对应的参数值
*/
template<typename RetType>
inline RetType FuncCaller(Instance* args[], size_t argc, std::function<RetType()> f)
{
	return f();
}

/**
*	\brief 函数Caller，用于对每一个instance转换成对应的参数值
*
*	通过递归调用funcCaller,给源函数添加一层wrapper，把每个参数
*	做一个instance转换
*/
template<typename RetType, typename T1, typename... T>
inline RetType FuncCaller(Instance* args[], size_t argc, std::function<RetType(T1, T...)> f)
{
	if (argc != sizeof...(T)+1)
	{
		Debug::Error("The param count is un-matched.");
	}

	ParamType<T1> curParam;

	return FuncCaller(args + 1, argc - 1, std::function<RetType(T...)>(
		[&](T... rest)->RetType {
		return f(std::forward<T1>(curParam.Convert(*args[0])),
			std::forward<T>(rest)...);
	}
	));

}

/**
*	\brief 注册普通函数元信息
*/
template<typename RetType, typename ...Args>
inline void Meta::Function(const std::string & funcName, RetType(*f)(Args...))
{
	Meta::FuncInfo funcInfo;
	RecordFunctionArgs(std::function<RetType(Args...)>(f), funcInfo.funcArgs);

	funcInfo.funcType = [f](Instance* args[], size_t argc)->Instance {
		ReturnType<RetType> returnType;
		return returnType.Convert(
			FuncCaller(args, argc, std::function<RetType(Args...)>(f)));
	};

	mFunctions[funcName] = funcInfo;
}

template<typename ...Args>
inline void util::Meta::Function(const std::string & funcName, void(*f)(Args...))
{
	Meta::FuncInfo funcInfo;
	RecordFunctionArgs(std::function<void(Args...)>(f), funcInfo.funcArgs);

	funcInfo.funcType = [f](Instance* args[], size_t argc)->Instance {
		FuncCaller(args, argc, std::function<void(Args...)>(f));
		return Instance();
	};

	mFunctions[funcName] = funcInfo;
}

/**
*	\brief 注册成员通函数元信息
*
*	对于成员函数来讲,相比普通函数，其可变参数列表中第一个参数
*	为类对象，std::function<RetType(T&t, Args...)>(f),当调用函数
*	时则通过(t.*f)(args...)调用
*/
template<typename T, typename ReturnType, typename ...Args>
inline void util::Meta::Function(const std::string & funcName, ReturnType(T::* f)(Args...))
{
}


template<typename T, typename... Args>
inline void util::Meta::Function(const std::string& funcName, void(T::*f)(Args...))
{
	Meta::FuncInfo funcInfo;
	RecordFunctionArgs(std::function<void(T&t, Args...)>(f), funcInfo.funcArgs);

	funcInfo.funcType = [f](Instance* args[], size_t argc)->Instance {
		FuncCaller(args, argc, std::function<void(T&t, Args...)>(
			[&args, f](T&t, Args... a) {
			(t.*f)(std::forward<Args>(a)...);
		}));
		return Instance();
	};

	mFunctions[funcName] = funcInfo;
}