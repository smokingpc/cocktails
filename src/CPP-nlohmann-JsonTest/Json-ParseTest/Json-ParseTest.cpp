#include <tchar.h>
#include <stdlib.h>
#include <string>
#include <vector>>
#include "../nlohmann/nlohmann/json.hpp"

using namespace std;

int _tmain(int argc, TCHAR* argv[])
{
	string jsonstr = "{\"success\":true,\"data\":{\"data\":[{\"id\":170,\"name\":\"tag1\",\"description\":\"\",\"device_count\":null,\"device_classification_count\":null},{\"id\":173,\"name\":\"tag2\",\"description\":\"\",\"device_count\":null,\"device_classification_count\":null},{\"id\":176,\"name\":\"tag3\",\"description\":\"\",\"device_count\":null,\"device_classification_count\":null}],\"total_count\":3,\"offset\":0,\"limit\":20}} ";
	nlohmann::json json = nlohmann::json::parse(jsonstr);

	auto obj = json["data"]["data"];
	string result = obj.dump();
}
