#include"enumInfo.h"
#include"core\debug.h"

template<typename E>
string EnumToString(E value)
{
	const auto& it = EnumInfoTraits<E>::names.find(value);
	if (it == EnumInfoTraits<E>::names.end())
		Debug::Die(std::string("Invalid") + EnumInfoTraits<E>::prettyName + " number:" +
			std::to_string(static_cast<int>(value)));
	return it->second;
}

template<typename E>
E StringToEnum(const string& name)
{
	for (auto&it : EnumInfoTraits<E>::names)
		if (it.second == name)
			return it.first;
	Debug::Die(std::string("Invalid") + EnumInfoTraits<E>::prettyName + " name:" + name);
}

template<typename E>
E StringToEnum(const string& name, E defaultValue)
{
	for (auto&it : EnumInfo<E>::names)
		if (it.second == name)
			return it.first;
	return defaultValue;
}