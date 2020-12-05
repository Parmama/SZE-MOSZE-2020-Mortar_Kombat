/**
 * @class JSON
 * @brief Reads simple data maps in .json format.
 * @brief Stores and handles a map.
*/

#ifndef JSON_H
#define JSON_H

#include <map>
#include <string>
#include <any>
#include <fstream>
#include <streambuf>
#include <unordered_set>
#include <list>
#include <variant>

class JSON
{
private:
	std::map<std::string, std::any> map;
	JSON(std::map<std::string, std::any> _map) : map(_map) {}
public:
    /**
	* @brief String or file parse.
	* @param String to parse or path to file.
    * @param Bool indicating whether or not the string is a path to file.
	* @return Map containing the keys and their values.
    * INSTRUCTIONS: Provide parseble string (default) or text file path (isFile must be set true)!
	*/
    static std::map<std::string, std::any> parse(std::string inputString = "", bool isFile = false);
    /**
	* @brief Stream parse.
	* @param Stream to parse.
	* @return Map containing the keys and their values.
	*/
    static std::map<std::string, std::any> parse(std::istream &stream);
    /**
	* @brief File parse to own map.
	* @param Path to file.
	* @return JSON with defined map.
	*/
	static JSON parseFromFile(std::string fileName);
    /**
	* @brief Key counter in own map.
	* @param Key to count.
	* @return Number of occurrences.
	*/
	const int count(const std::string &key) const { return map.count(key); }
    /**
	* @brief Getter for own map.
    * @tparam Format to return in.
	* @param Key of the value to return.
	* @return Value of the provided key.
	*/

	template <typename T> //using "JSON::" scope to differenciate from "std::get"
	      	T get(const std::string &key) { return std::any_cast<T>(map[key]); }
	

	class ParseException : public std::exception {}; ///< Class member for exception handling.
	
	typedef std::list<std::variant<std::string>> list;

};

#endif
