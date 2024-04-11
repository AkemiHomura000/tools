#pragma once

#include "rapidjson/document.h"  


/*
* rapidjson::Value是RapidJSON库中的核心数据结构，用于表示JSON值。一个Value可以是以下类型之一：
kNullType：表示空值。
kFalseType：表示布尔值false。
kTrueType：表示布尔值true。
kObjectType：表示一个JSON对象。
kArrayType：表示一个JSON数组。
kStringType：表示一个字符串。
kNumberType：表示一个数字。
*/
class Serializable
{
public:
	virtual const  rapidjson::Document serialize()=0;
	virtual void deserialize(const rapidjson::Value& value) = 0;
private:
	//unsigned int m_id=0; 可以在此处用hasmap来自动生成所有对象id,或者uuid
};