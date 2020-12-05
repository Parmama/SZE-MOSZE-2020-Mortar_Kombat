#include "JSON.h"

std::unordered_set<char> spacingChars = { ' ', '\n', '\t', '\r', '\f', '\b' }; //whitespace + nonprintables
std::unordered_set<char> pairEndingChars = { ',', '}' };
std::unordered_set<char> backslashChars = { '"', '\\' };

//INSTRUCTIONS: Specify mode = "inclusive" for an allowed character set. Exclusive is default.
std::string::size_type findNext(std::string &s, char target, std::unordered_set<char> set, std::string mode = "")
{
    double pos = s.find(target);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("1: Expected token not found!");
        return -1;
    }

    if (mode == "inclusive")
    {
        for (char c : s.substr(0, pos))
        {
            if (set.find(c) == set.end())
            {
                throw std::runtime_error("2: Unexpected token!");
                return -1;
            }
        }
    }
    else
    {
        for (char c : s.substr(0, pos))
        {
            if (set.find(c) != set.end())
            {
                throw std::runtime_error("3: Unexpected token!");
                return -1;
            }
        }
    }
    return pos;
}

void checkString(std::string& s)
{
	if (backslashChars.find(s[0]) != backslashChars.end()) //first character is backslashchar --> error
	{
		throw std::runtime_error("5A: Unrecognized value!");
	}
    for (std::string::size_type pos = 1; pos < s.length(); pos++)
    {
        if (backslashChars.find(s[pos]) != backslashChars.end() && s[pos - 1] != '\\') //backslashchar has no '\' before it and ...
        {
			if (s[pos] == '\\' && pos < s.length() - 1 && s[pos + 1] != '\\') //...no double '\' with the following character either --> error
			{
            	throw std::runtime_error("5B: Unrecognized value!");
			}
        }
    }
}

std::variant<std::string, std::list<std::variant<std::string>>, bool, nullptr_t, float > string2variant(std::string& s)
{
	if (s[0] == '"') //starts with '"' --> string
	{
		s.erase(0, 1).erase(s.length() - 1); //remove '"' signs
		checkString(s);
		return s; //is string
	}

	else if (s[0] == '[') //starts with "[" --> list
	{
		s.erase(0, 1).erase(s.length() - 1); //remove square bracket signs
		checkString(s);
		bool inString = false;
		std::string word = "";
		std::list<std::variant<std::string>> outputList;

		findNext(s, '"', spacingChars, "inclusive"); //check, if there is anything unexpected before the first '"' sign
		for (char c : s)
		{
			if (c == '"')
			{
				if (inString) outputList.push_back(word);
				else
				{
					for (char c2 : word)
					{
						if (spacingChars.find(c2) == spacingChars.end() && c2 != ',') //other than spacing characters or ',' sign is found between two values
						{
							throw std::runtime_error("15A: Unexpected character inside list!");
						}	
					}
					if (std::count(word.begin(), word.end(), ',') != 1) //more than
					{
						throw std::runtime_error("16: Unexpected number of commas (not 1)!");
					}
				}
				inString = !inString;
				word = "";
			}
			else word += c;
		}
		for (char c2 : word)
		{
			if (spacingChars.find(c2) == spacingChars.end()) //other than spacing characters found after all values
			{
				throw std::runtime_error("15B: Unexpected character inside list!");
			}
		}
		return outputList;
		//!!! NOTE: Lists are expected to contain only strings. Other types are not accepted inside lists. !!!
	}

	else if (s == "true") return true; //is boolean
	else if (s == "false") return false; //is boolean
	else if (s == "null") return nullptr; //is null pointer

    else //all correct non-numeric types covered --> can be numeric
    {
        std::unordered_set<char> numChars = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '-', '+', 'e', 'E'};
        for (char c : s.substr(1, s.length() - 1))
        {
            if (numChars.find(c) == numChars.end()) //has non numeric chars -> error
            {
                throw std::runtime_error("8: Unrecognized value!");
            }
        }
        return stof(s); //is numeric
		//!!! NOTE: stof accepts some synthax that the official json does not. !!!
    }
}

std::map<std::string, std::any> parseString(std::string& s)
{
    std::map<std::string, std::any> map;
    std::string key, valueString;
    std::any value;
    std::string::size_type pos;

    s.erase(0, findNext(s, '{', spacingChars, "inclusive") + 1); //remove first '{'
    bool thereIsMore;
    do
    {
        s.erase(0, findNext(s, '"', spacingChars, "inclusive") + 1); //remove key starting '"'
        pos = findNext(s, '"', {'{', '}'}); //find key closing '"'
        key = s.substr(0, pos); //pass key
        checkString(key);
        if (map.find(key) != map.end()) //key already exists
        {
            throw std::runtime_error("9: Duplicate keys!");
        }
        s.erase(0, pos + 1); //remove key

        s.erase(0, findNext(s, ':', spacingChars, "inclusive") + 1); //remove ':' operator
        pos = 0; //reset position
        while(spacingChars.find(s[pos]) != spacingChars.end() && pos < s.length()) pos++; //find value begginging (or incorrect content end)
        if (pairEndingChars.find(s[pos]) != pairEndingChars.end()) //':' is followed directly by ',' or '}'
        {
            throw std::runtime_error("10: No value found!");
        }
        s.erase(0, pos); //remove everything before value
		
		if (s[0] == '"') //starts as a string
		{
			pos = s.substr(1, s.length()).find('"'); //skip inside of string
			if (pos == std::string::npos) //does not end with '"' --> error
			{
				throw std::runtime_error("13A: Expected '\"'!");
			}
		}
		else if (s[0] == '[') //starts as list
		{
			pos = s.substr(1, s.length()).find(']'); //skip inside of list
			if (pos == std::string::npos) //does not end with ']' --> error
			{
				throw std::runtime_error("13A: Expected ']'!");
			}
		}
		else
		{
			pos = 0; //just reset position
		}
		while (pairEndingChars.find(s[pos]) == pairEndingChars.end() && pos < s.length()) pos++; //find value end if non-string (or incorrect content end)
		valueString = s.substr(0, pos); //pass value string
		s.erase(0, pos); //remove everything before value

		while(spacingChars.find(valueString[valueString.length() - 1]) != spacingChars.end()) //last character is spacing character
		{
			valueString = valueString.substr(0, valueString.size() - 1); //remove last character
		}
		value = string2variant(valueString); //give value the real type

		map.insert(make_pair(key, value)); //add pair to map

		thereIsMore = s[0] == ',' ? true : false; //check for more data (',')
		if (thereIsMore)
		{
			s.erase(0, 1); //remove ','
		}
    } while (thereIsMore); //listing continues

    s.erase(0, findNext(s, '}', spacingChars, "inclusive") + 1); //find closing '}'
    pos = 0;
    while (s[pos] != '\0') //check for non correct chars in remaining part
    {
        if (spacingChars.find(s[pos]) == spacingChars.end())
        {
            throw std::runtime_error("11: Unexpected token!");
        }
        pos++;
    }
    return map;
}
    
std::map<std::string, std::any> JSON::parse(std::string inputString, bool isFile)
{
    if (isFile)
    {
        std::ifstream file(inputString);
        if (file.good())
        {
            std::string line;
            inputString = "";
            while (getline(file, line))
            {
                inputString += line;
            }
        }
        else
        {
            throw std::runtime_error("12: Unable to open file!");
        }
    }
    return parseString(inputString);
}

std::map<std::string, std::any> JSON::parse(std::istream &stream)
{
    std::istreambuf_iterator<char> eos;
    std::string s(std::istreambuf_iterator<char>(stream), eos);
    return parse(s);
}

JSON JSON::parseFromFile(std::string fileName)
{
	return(JSON(JSON::parse(fileName, true)));
}

