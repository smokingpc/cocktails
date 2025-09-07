#include <tchar.h>
#include <stdlib.h>
#include <string>
#include <vector>>
#include "../nlohmann/nlohmann/json.hpp"

using namespace std;

int _tmain(int argc, TCHAR* argv[])
{
	nlohmann::json jobj;
	vector<string> vector = {"aaa", "bbb", "ccc"};

	for (auto& item : vector)
	{
		jobj.push_back(item);
	}
	string output = jobj.dump(4, 32, false);
	printf("JSON output:\n%s\n", output.c_str());

	nlohmann::json jobj2 = nlohmann::json::parse(output);
	string output2 = jobj2.dump(4, 32, false);
	printf("JSON output2:\n%s\n", output.c_str());

}
