// nlohmannTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string>

#include "json.hpp"

using namespace std;

void IterateJsonObject(nlohmann::json json);

void IterateJsonArray(nlohmann::json json)
{
	//this json object is "[{xxx:OOO, xxx:ooo}, {111:222, 333:444}]" standard json array.
	//values of this array are only JsonObject.
	for (auto iter = json.begin(); iter != json.end(); iter++)
	{
		nlohmann::json value = iter.value();
		if (value.is_object())
			IterateJsonObject(value);
		else if (value.is_array())
			IterateJsonArray(value);
		else
			continue;
	}
}
void IterateJsonObject(nlohmann::json json)
{
	//this json object is "{xxx:OOO, xxx:ooo}" standard json object.
	for (auto iter = json.begin(); iter != json.end(); iter++)
	{
		//iter of JsonObject is key-value pairs.
		//the value of iter could be data, another JsobObject, or JsonArray
		if (iter.value().is_null())
			continue;

		nlohmann::json value = iter.value();
		if (!value.is_array())
		{
			string key = iter.key();
			if (!key.empty())
			{
				printf(" Key => %s\n", key.c_str());
			}
		}

		if (value.is_primitive())
		{
			if (value.is_string())
			{
				string temp = "";
				value.get_to(temp);
				printf("%s : %s\n", iter.key().c_str(), temp.c_str());
			}
			else if (value.is_number())
			{
				UINT32 temp = 0;
				value.get_to(temp);
				printf("%s : %lu\n", iter.key().c_str(), temp);
			}
		}
		else if (value.is_array())
		{
			IterateJsonArray(value);
		}
		else if (value.is_object())
		{
			IterateJsonObject(value);
		}
	}
}

void IterateJson(nlohmann::json json)
{
	if (json.is_array())
	{
		IterateJsonArray(json);
	}
	else if (json.is_object())
	{
		IterateJsonObject(json);
	}
	else
	{
		printf("unknown json type => %d\n", json.type());
	}
}

void DoJsonTest(string jsonstr)
{
	nlohmann::json json = nlohmann::json::parse(jsonstr);

	try {
		printf("Iterate JSON:\n%s\n", jsonstr.c_str());
		IterateJson(json);
	}
	catch (std::exception& e)
	{
		printf("JSON parsing error, msg=%s", e.what());
	}
}

void DoJsonTest2(string jsonstr)
{
	nlohmann::json json = nlohmann::json::parse(jsonstr);
	try {
		printf("Iterate JSON:\n%s\n", jsonstr.c_str());
		string name = "";
		UINT32 id = 0;
		
//		for (size_t i = 0; i < json.size(); i++)
		for (auto iter : json)
		{
			//if  "id"  or "name" not exist, json throws exception.
			//json[i]["id"].get_to(id);
			iter["id"].get_to(id);
			printf("id : %lu\n", id);

			//json[i]["name"].get_to(name);
			iter["name"].get_to(name);
			printf("name : %s\n", name.c_str());
		}
	}
	catch (std::exception& e)
	{
		printf("JSON parsing error, msg=%s", e.what());
	}
}

void DoJsonRemoveKeyTest(string jsonstr, string key)
{
	nlohmann::json json = nlohmann::json::parse(jsonstr);
	try {
		printf("Iterate JSON:\n%s\n", jsonstr.c_str());
		string name = "";
		UINT32 id = 0;
		for (auto& item : json) 
		{
			item.erase(key);
		}

		string temp = json.dump(4);
		printf("result=>\n%s\n", temp.c_str());
	}
	catch (std::exception& e)
	{
		printf("JSON parsing error, msg=%s", e.what());
	}
}


int _tmain(int argc, TCHAR* argv[])
{
	//string json = "{\"id\":111, \"name\":\"Tag1\"}";
	//printf("==== Test1 ====\n");
	//DoJsonTest(json);
	
	//json = "{{\"id\":222, \"name\":\"Tag2\"}}";
	//printf("==== Test2 ====\n");
	//DoJsonTest(json);
	
	//json = "{\"1st-item\" : {\"id\":333, \"name\":\"Tag3\"}}";
	//printf("==== Test3 ====\n");
	//DoJsonTest(json);

	//json = "[{\"id\":444, \"name\":\"Tag4\"}, {\"id\":555, \"name\":\"Tag5\"}]";
	//printf("==== Test4 ====\n");
	//DoJsonTest(json);
	//DoJsonTest2(json);

	printf("==== Test1 ====\n");
	string json = "[{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":182,\"name\":\"newt1\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":188,\"name\":\"newt2\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":194,\"name\":\"newtest11\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":197,\"name\":\"newtest12\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":5636,\"name\":\"tag3\"}]";
	DoJsonRemoveKeyTest(json, string("device_classification_count"));

	printf("==== Test2 ====\n");
	json = "[{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":182,\"name\":\"newt1\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":188,\"name\":\"newt2\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":194,\"name\":\"newtest11\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":197,\"name\":\"newtest12\"},{\"description\":\"\",\"device_classification_count\":null,\"device_count\":null,\"id\":5636,\"name\":\"tag3\"}]";
	DoJsonRemoveKeyTest(json, string("abc"));
}
