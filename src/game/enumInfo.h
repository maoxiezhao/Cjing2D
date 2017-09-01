#ifndef _ENUMINFO_H_
#define _ENUMINFO_H_

#include"common\common.h"
#include<map>

/**
*	\brief 这里提供枚举变量和字符串映射的办法，可以从string
*	得到enum或者是从enum得到string
*/

template<typename E>
class EnumInfo
{
public:
	using nameType = std::map<E, string >;
};

/**
*	\brief 所有需要的实现映射的实现下面的结构
*
*	\const static string prettyName;
*	\const static nameType names;
*/
template<typename E>
struct EnumInfoTraits;

/**
*	\brief 从enum得到string
*/
template<typename E>
string EnumToString(E value);

/**
*	\brief 从enum得到string
*
*	 如果找到则返回提供defaultValue
*/
template<typename E>
string EnumToString(E value, const string& defaultValue);

/**
*	\brief 从string得到enum
*/
template<typename E>
E StringToEnum(const string& name);

/**
*	\brief 从string得到enum
*
*	 如果找到则返回提供defaultValue
*/
template<typename E>
E StringToEnum(const string& name,E defaultValue);

#include"enumInfo.inl"

#endif