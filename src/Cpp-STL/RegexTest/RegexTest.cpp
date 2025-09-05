
#include <tchar.h>
#include <stdlib.h>
#include <string>
#include <regex>

using namespace std;

void MatchRegex(string& src, string& regex) 
{
	std::regex pattern(regex);
	bool match = std::regex_match(src, pattern);

	printf("String \"%s\" is %s to pattern \"%s\"\n", 
			src.c_str(),
			match? "matched" : "NOT matched",
			regex.c_str());
}

void ParseRegexByGroups(string& src, string& regex)
{
	std::regex pattern(regex);
	std::smatch match;
	auto iter_begin = std::sregex_iterator(src.begin(), src.end(), pattern);
	auto iter_end = std::sregex_iterator();

	printf("Finding regex\"%s\" Group in string \"%s\":\n", 
			regex.c_str(), src.c_str());
	for (std::sregex_iterator iter = iter_begin; iter != iter_end; ++iter)
	{
		std::smatch match = *iter;
		printf("\t%s\n", match.str().c_str());
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	bool ok = false;
	string data = "";
	string regex = "";

	data = "abc, def, x-y zz";
	regex = "([0-9a-zA-Z\\-\\s]+)";
	MatchRegex(data, regex);
	ParseRegexByGroups(data, regex);

	data = "abc, def, xyz";
	regex = "([0-9a-zA-Z]+)";
	ParseRegexByGroups(data, regex);
	data = "0918326408, 28825252";
	regex = "([0-9]+)";
	ParseRegexByGroups(data, regex);

	data = "Date: 2023-12-25, Time: 14:30:45";
}
