#include"test.h"

const string EnumInfoTraits<TestNumber>::prettyName = "Test";
const EnumInfo<TestNumber>::nameType EnumInfoTraits<TestNumber>::names = 
{
	{TestNumber::T_ONE,"one"},
	{TestNumber::T_TWO,"two"},
	{TestNumber::T_THREE,"three"}
};