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
#include "Common.h"

#include "Json.h"
#include "ConfigValue.h"


//-------------------------------------------------------------------------------

namespace json_internal
{
	void WriteTabs(int ilevel, std::stringstream& out)
	{
		for(int i = 0; i < ilevel; ++i)
		{
			out << "\t";
		}
	}

	/// @param quotes Specifies whetever the string is supposed to be surrounded by quotes
	void WriteString(const char* str, std::stringstream& out, bool quotes = true)
	{
		// We need to escape any special characters before writing them to the JSON doc
		for(const char* c = str; *c != '\0'; ++c)
		{
			switch(*c)
			{
			case '\\':
				out << "\\\\";
				break;
			case '\"':
				out << "\\\"";
				break;
			case '\n':
				out << "\\n";
				break;
			case '\r':
				out << "\\r";
				break;
			case '\t':
				out << "\\t";
				break;
			case ' ':
				if(!quotes) // If string isn't supposed to be surrouneded by quotes, we escape spaces
					out << "\\ ";
				else
					out << " ";
				break;
			default:
				out << *c;
				break;

			}
		}
	}

}
//-------------------------------------------------------------------------------
json::Reader::Reader()
{
	_cur = _end = _begin = 0;
}
json::Reader::~Reader()
{
}
//-------------------------------------------------------------------------------
const std::string& json::Reader::GetErrorMessage()
{
	return _error;
}
void json::Reader::Error(const char* msg)
{
	int line, column;
	GetCurrentPosition(line, column);

	std::stringstream ss;
	ss << "(Line: " << line << ", Column: " << column << ") Error: " << msg;
	_error = ss.str();
}
void json::Reader::GetCurrentPosition(int& line, int& column)
{
	line = 0; column = 0;

	const char* current = _begin;
	const char* line_start = _begin;
	while(current < _cur)
	{
		if(*current == '\r')
		{
			if(*(current+1) == '\n')
				++current;
			
			line_start = current;
			++line;
		}
		else if(*current == '\n')
		{
			line_start = current;
			++line;
		}
		++current;
	}
	++line;
	column = (int)(_cur - line_start) - 1;
}
//-------------------------------------------------------------------------------
bool json::Reader::Read(const char* doc, int64_t length, ConfigValue& root)
{
	_cur = _begin = doc;
	_end = doc + length;

	SkipSpaces();
	if(*_cur == '{')
		return ParseObject(root);
	
	// Assume root is an object
	root.SetEmptyObject();

	std::string name;
	while(1)
	{
		SkipSpaces();
		if(_cur == _end)
			break;


		name = "";
		if(!ParseString(name))
		{
			Error("Failed to parse string");
			return false;
		}

		SkipSpaces();
		if(*_cur != '=' && *_cur != ':')
		{
			Error("Expected '=' or ':'");
			return false;
		}
		_cur++;
		
		ConfigValue& elem = root[name.c_str()];
		if(!ParseValue(elem))
		{
			return false; // Failed to parse value
		}

		SkipSpaces();

		char c = *_cur;
		if(c == ',') // Separator between elements (Optional)
		{
			_cur++;
			continue;
		}
	}

	return true;
}

void json::Reader::SkipSpaces()
{
	while(_cur != _end)
	{
		char c = *_cur;
		if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
		{
			++_cur;
		}
		else
			break;
	}
}

bool json::Reader::ParseString(std::string& str)
{
	// Typically a string is surrounded by quotes but we also support strings without quotes
	//	a string without quotes is considered to end at the first whitespace character (E.g. ' ' or '\t')

	// Count length first
	const char* str_end = _cur;
	bool quotes = (*_cur == '"'); // Keep track if this string is surrounded by quotes
	if(quotes)
		++str_end; // Skip starting "
	while(str_end != _end)
	{
		char c = *str_end++;
		if(c == '\\')
			str_end++; // Skip checking next character	
		else if((quotes && (c == '"')) || (!quotes && (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '=' || c == ':')))
			break;
	}
	if(quotes)
	{
		_cur += 1; // Skip starting "
	}
	str_end -= 1; // Skip trailing " or any trailing whitespace

	while(_cur != str_end)
	{
		char c = *_cur++;
		if(c == '\\')
		{
			char esc = *_cur++;
			switch(esc)
			{
			case '"':
				c = '"';
				break;
			case '/':
				c = '/';
				break;
			case '\\':
				c = '\\';
				break;
			case '\n':
				c = '\n';
				break;
			case '\r':
				c = '\r';
				break;
			case '\t':
				c = '\t';
				break;
			default:
				c = esc;
			};
		}

		str.append(1, c);
	}
	if(quotes)
		_cur++; // Trailing "

	return true;
}

bool json::Reader::ParseObject(ConfigValue& value)
{
	value.SetEmptyObject();
	
	_cur++; // Skip '{'
	SkipSpaces();
	if(*_cur == '}') // Empty object
	{
		_cur++;
		return true;
	}	

	std::string name;
	while(1)
	{
		SkipSpaces();

		name = "";
		if(!ParseString(name))
		{
			Error("Failed to parse string");
			break; // Failed to parse string
		}

		SkipSpaces();
		if(*_cur != '=' && *_cur != ':')
		{
			Error("Expected '=' or ':'");
			return false;
		}
		_cur++;

		ConfigValue& elem = value[name.c_str()];
		if(!ParseValue(elem))
			break; // Failed to parse value

		SkipSpaces();

		char c = *_cur;
		if(c == ',') // Separator between elements (Optional)
		{
			_cur++;
			continue;
		}
		if(c == '}') // End of object
		{
			_cur++;
			return true;
		}		
	}	

	return false;
}
bool json::Reader::ParseArray(ConfigValue& value)
{
	value.SetEmptyArray();
	
	_cur++; // Skip '['
	SkipSpaces();
	if(*_cur == ']')
	{
		_cur++;
		return true;
	}
	while(1)
	{
		ConfigValue& elem = value.Append();
		
		if(!ParseValue(elem))
			return false;
		
		SkipSpaces();

		char c = *_cur;
		if(c == ',') // Separator between elements (Optional)
		{
			_cur++;
			continue;
		}
		if(c == ']') // End of array
		{
			_cur++;
			break;
		}
	}
	return true;
}


bool json::Reader::ParseDouble(ConfigValue& value)
{
	char* number_end;
	double number = std::strtod(_cur, &number_end);
	value.SetDouble(number);
	_cur = number_end;
	return true;
}

bool json::Reader::ParseNumber(ConfigValue& value)
{
	bool integer = true; // Number is either integer or float
	for(const char* c = _cur; c != _end; ++c)
	{
		if((*c >= '0' && *c <= '9') || ((*c == '-' || *c == '+') && (c == _cur))) // Allow a negative sign at the start for integers
			continue;
		else if(*c == '.' || *c == 'e' || *c == 'E' || *c == '+') 
		{
			integer = false;
			break;
		}
		else
			break;
	}
	if(!integer)
		return ParseDouble(value);


	bool negative = (*_cur == '-');
	if(negative)
		_cur++;

	uint64_t number = 0;
	while(_cur != _end)
	{
		if(*_cur >= '0' && *_cur <= '9')
		{
			uint32_t digit = *_cur - '0';
			number = number * 10 + digit;
			_cur++;
		}
		else
			break;
	}
	if(negative)
	{
		value.SetInt(-int64_t(number));
	}
	else if(number <= INT64_MAX)
	{
		value.SetInt(int64_t(number));
	}
	else
	{
		value.SetUInt(number);
	}

	return true;
}

bool json::Reader::ParseValue(ConfigValue& value)
{
	SkipSpaces();
	bool b = true;
	char c = *_cur;
	switch(c)
	{
	case '{':
		b = ParseObject(value);
		break;
	case '[':
		b = ParseArray(value);
		break;
	case '"':
		{
			std::string str;
			b = ParseString(str);
			if(b)
				value.SetString(str.c_str());
			else
				Error("Failed to parse string");
		}
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '-':
		b = ParseNumber(value);
		break;
	case 't': // true
		if(*(++_cur) != 'r' || *(++_cur) != 'u' || *(++_cur) != 'e')
		{
			Error("Expected \"true\"");
			return false;
		}
		++_cur;
		value.SetBool(true);
		break;
	case 'f': // false
		if(*(++_cur) != 'a' || *(++_cur) != 'l' || *(++_cur) != 's' || *(++_cur) != 'e')
		{
			Error("Expected \"false\"");
			return false;
		}
		++_cur;
		value.SetBool(false);
		break;
	case 'n': // null
		if(*(++_cur) != 'u' || *(++_cur) != 'l' || *(++_cur) != 'l')
		{
			Error("Expected \"null\"");
			return false;
		}
		++_cur;
		value.SetNull();
		break;
	default:
		b = false;
	};
	return b;
}


//-------------------------------------------------------------------------------
json::Writer::Writer() : _ilevel(0)
{
}
json::Writer::~Writer()
{
}
void json::Writer::Write(const ConfigValue& root, std::stringstream& out, bool format)
{
	_format = format;
	WriteValue(root, out);
	out << "\n";
}
void json::Writer::WriteValue(const ConfigValue& node, std::stringstream& out)
{
	switch(node.Type())
	{
	case ConfigValue::NULL_VALUE:
		out << "null";
		break;
	case ConfigValue::BOOL:
		out << node.AsBool();
		break;
	case ConfigValue::INTEGER:
		out << node.AsInt64();
		break;
	case ConfigValue::UINTEGER:
		out << node.AsUInt64();
		break;
	case ConfigValue::FLOAT:
		out << node.AsDouble();
		break;
	case ConfigValue::STRING:
		out << "\"";
		json_internal::WriteString(node.AsString(), out);
		out << "\"";
		break;
	case ConfigValue::ARRAY:
		{
			out << "[";

			_ilevel++;
			int size = node.Size(); 
			for(int i = 0; i < size; ++i)
			{
				if(i != 0)
					out << ",";

				if(_format)
				{
					out << "\n";
					json_internal::WriteTabs(_ilevel, out);
				}
				WriteValue(node[i], out);
			}
			if(_format)
				out << "\n";
			_ilevel--;
			if(_format)
				json_internal::WriteTabs(_ilevel, out);
			out << "]";
		}
		break;
	case ConfigValue::OBJECT:
		{
			out << "{";
			_ilevel++;
			ConfigValue::ConstIterator it, end;
			it = node.Begin(); end = node.End();
			for( ; it != end; ++it)
			{
				if(it != node.Begin())
					out << ",";

				if(_format)
				{
					out << "\n";
					json_internal::WriteTabs(_ilevel, out);
				}
				out << "\"";
				json_internal::WriteString(it->first.c_str(), out);
				out << "\": ";
				WriteValue(it->second, out);
			}
			if(_format)
				out << "\n";
			_ilevel--;
			if(_format)
				json_internal::WriteTabs(_ilevel, out);
			out << "}";
		}
		break;
	};
}



