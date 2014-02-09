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
#ifndef __JSON_H__
#define __JSON_H__

#include <string>

class ConfigValue;

namespace json
{
	class Reader
	{
	public:
		Reader();
		~Reader();

		/// Parses a JSON document into ConfigValues
		///	@param doc JSON docoument
		///	@param root This is going to be the root node
		///	@return True if the parsing was successful, else false
		bool Read(const char* doc, int64_t length, ConfigValue& root);
		
		/// Returns an error message if the last call to Parse failed.
		const std::string& GetErrorMessage();

	private:
		const char* _begin;
		const char* _cur; 
		const char* _end;

		std::string _error;

		void Error(const char* msg);
		void GetCurrentPosition(int& line, int& column);

		bool ParseValue(ConfigValue& value);
		bool ParseNumber(ConfigValue& value);
		bool ParseDouble(ConfigValue& value);
		bool ParseArray(ConfigValue& value);
		bool ParseObject(ConfigValue& value);

		bool ParseString(std::string& str);
		void SkipSpaces();

	};

	class Writer
	{
	public:
		Writer();
		~Writer();

		/// @brief Generates JSON from the specified ConfigValue
		/// @param root Root config node
		/// @param out The generated JSON will be stored in this variable
		/// @param format Should we use any formatting. Formatting makes it more 
		///			human readable, otherwise everything is just printed on one line.
		void Write(const ConfigValue& root, std::stringstream& out, bool format);

	private:
		bool _format;
		int _ilevel; // Indent level

		void WriteValue(const ConfigValue& node, std::stringstream& out);
	};
};


#endif // __JSON_H__
