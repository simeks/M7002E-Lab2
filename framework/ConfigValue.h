/*
Copyright (c) 2013 Simon Ekström

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef __CONFIGVALUE_H__
#define __CONFIGVALUE_H__

#include <vector>
#include <map>
#include <sstream>


class ConfigValueIterator;
class ConfigValueConstIterator;

class ConfigValue
{
public:
	enum ValueType
	{
		NULL_VALUE,
		INTEGER,
		UINTEGER,
		FLOAT,
		BOOL,
		STRING,
		ARRAY,
		OBJECT
	};
	
	typedef std::map<std::string, ConfigValue> ValueMap;
	typedef ConfigValueIterator Iterator;
	typedef ConfigValueConstIterator ConstIterator;


public:
	ConfigValue();
	ConfigValue(const ConfigValue& source);
	~ConfigValue();

	ValueType Type() const;

	bool IsNull() const;
	bool IsInt() const;
	bool IsUInt() const;
	bool IsFloat() const;
	bool IsBool() const;
	bool IsString() const;
	bool IsArray() const;
	bool IsObject() const;
	
	/// @return True if value is a number, meaning either an integer, unsigned integer or float
	bool IsNumber() const;
	
	/// @brief Returns the value of this object as an integer.
	int AsInt() const;

	/// @brief Returns the value of this object as an 64bit integer.
	int64_t AsInt64() const;

	/// @brief Returns the value of this object as an unsigned integer.
	uint32_t AsUInt() const;

	/// @brief Returns the value of this object as an 64bit unsigned integer.
	uint64_t AsUInt64() const;

	/// @brief Returns the value of this object as a floating-point value.
	float AsFloat() const;

	/// @brief Returns the value of this object as a double precision float.
	double AsDouble() const;

	/// @brief Returns the value of this object as a boolean.
	bool AsBool() const;

	/// @brief Returns the value of this object as a string.
	const char* AsString() const;

	/// @brief Returns the size of this value, the number of sub elements
	/// @return Number of sub elements if either an array or an object. 
	///			If the value is a single element type this returns 1, and if
	///			the value is NULL it returns 0.
	uint32_t Size() const;


	/// @brief Sets the object value to null, releasing any previous value.
	void SetNull();

	/// @brief Sets the objects value to the specified integer.
	void SetInt(int i);

	/// @brief Sets the objects value to the specified integer.
	void SetInt(int64_t i);

	/// @brief Sets the objects value to the specified unsigned integer.
	void SetUInt(uint32_t i);

	/// @brief Sets the objects value to the specified unsigned integer.
	void SetUInt(uint64_t i);

	/// @brief Sets the objects value to the specified float.
	void SetFloat(float f);

	/// @brief Sets the objects value to the specified float.
	void SetDouble(double d);

	/// @brief Sets the objects value to the specified boolean.
	void SetBool(bool b);

	/// @brief Sets the objects value to the specified string.
	void SetString(const char* s);

	/// @brief Sets this value to an empty array
	void SetEmptyArray();

	/// @brief Sets this value to an empty object
	void SetEmptyObject();

	/// @brief Appends a value to the array
	/// @remark Assumes that this ConfigValue is an array
	ConfigValue& Append();

	/// @brief Returns an iterator for the beginning of all object elements 
	/// @remark This only works if the value is of the type OBJECT
	Iterator Begin();

	/// @brief Returns an iterator for the beginning of all object elements 
	/// @remark This only works if the value is of the type OBJECT
	ConstIterator Begin() const;

	/// @brief Returns the end iterator for the object elements
	/// @remark This only works if the value is of the type OBJECT
	Iterator End();

	/// @brief Returns the end iterator for the object elements
	/// @remark This only works if the value is of the type OBJECT
	ConstIterator End() const;

	ConfigValue& operator=(const ConfigValue& source);

	ConfigValue& operator[](const char* key);
	const ConfigValue& operator[](const char* key) const;
	
	ConfigValue& operator[](int index);
	const ConfigValue& operator[](int index) const;

private:
	
	union Value
	{
		int64_t i;
		uint64_t u;
		double d;
		bool b;
		std::string* s;
		std::vector<ConfigValue>* a;
		ValueMap* o;
	};
	
	ValueType _type;
	Value _value;
	
};

class ConfigValueIterator : public ConfigValue::ValueMap::iterator
{
public:
	ConfigValueIterator() : ConfigValue::ValueMap::iterator()  {}
	ConfigValueIterator(ConfigValue::ValueMap::iterator iter) : 
		ConfigValue::ValueMap::iterator(iter) { }
};

class ConfigValueConstIterator : public ConfigValue::ValueMap::const_iterator
{
public:
	ConfigValueConstIterator() : ConfigValue::ValueMap::const_iterator()  {}
	ConfigValueConstIterator(ConfigValue::ValueMap::const_iterator iter) : 
		ConfigValue::ValueMap::const_iterator(iter) { }
};




#endif // __CONFIGVALUE_H__
