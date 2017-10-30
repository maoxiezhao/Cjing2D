#ifndef _ENUMINFO_H_
#define _ENUMINFO_H_

#include"common\common.h"
#include<map>

/**
*	\brief �����ṩö�ٱ������ַ���ӳ��İ취�����Դ�string
*	�õ�enum�����Ǵ�enum�õ�string
*/

template<typename E>
class EnumInfo
{
public:
	using nameType = std::map<E, string >;
};

/**
*	\brief ������Ҫ��ʵ��ӳ���ʵ������Ľṹ
*
*	\const static string prettyName;
*	\const static nameType names;
*
*   static const string prettyName;
*	static const EnumInfo<E>::nameType names;
*/
template<typename E>
struct EnumInfoTraits;

/**
*	\brief ��enum�õ�string
*/
template<typename E>
string EnumToString(E value);

/**
*	\brief ��enum�õ�string
*
*	 ����ҵ��򷵻��ṩdefaultValue
*/
template<typename E>
string EnumToString(E value, const string& defaultValue);

/**
*	\brief ��string�õ�enum
*/
template<typename E>
E StringToEnum(const string& name);

/**
*	\brief ��string�õ�enum
*
*	 ����ҵ��򷵻��ṩdefaultValue
*/
template<typename E>
E StringToEnum(const string& name,E defaultValue);

#include"enumInfo.inl"

#endif