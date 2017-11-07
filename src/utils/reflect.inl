/**
*	\brief 执行函数
*/
template<typename ...Args>
inline Instance util::Meta::Call(const std::string& funcName, Args&&... args)
{
	Instance* instances[sizeof...(args)] = { &const_cast<Instance&>(args)... };

	auto funcInfo = mFunctions.find(funcName);
	if (funcInfo == mFunctions.end())
	{
		Debug::Error("Call the non-exists meta function.");
	}
	FuncType funcType = funcInfo->second.funcType;
	Instance result = funcType(instances, sizeof...(args));

	return std::move(result);
}

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
inline void RecordFunctionArgs(RetType(*f)(Args...), Meta::FuncArgVector& argVector)
{
	Implemention::FuncRecord(std::function<RetType(Args...)>(f), argVector);
}

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
*	\brief 注册函数元信息
*/
template<typename RetType, typename ...Args>
inline void Meta::Function(const std::string & funcName, RetType(*f)(Args...))
{
	Meta::FuncInfo funcInfo;
	RecordFunctionArgs(f, funcInfo.funcArgs);

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
	RecordFunctionArgs(f, funcInfo.funcArgs);

	funcInfo.funcType = [f](Instance* args[], size_t argc)->Instance {
		FuncCaller(args, argc, std::function<void(Args...)>(f));
		return Instance();
	};

	mFunctions[funcName] = funcInfo;
}
