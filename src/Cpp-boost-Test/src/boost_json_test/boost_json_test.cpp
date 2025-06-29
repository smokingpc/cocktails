// boost_json_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace boost_ptree = boost::property_tree;
using namespace std;
using json_node = std::pair<const std::string, boost_ptree::ptree>;

void IterateNormalJson(const string &key, boost_ptree::ptree &pt)
{
	pt.
}

void IterateJsonArray(boost_ptree::ptree &pt)
{
	//for (auto iter : root)
	//{
	//	auto item = iter.second;
	//	if (!root.begin()->first.empty())
	//		IterateNormalJson(root);
	//	else
	//		IterateJsonArray(root);
	//}
}

void DoJsonTest(string json)
{
	boost_ptree::ptree root;
	istringstream istream(json.c_str());

	try {
		boost::property_tree::read_json(istream, root);
		for (auto iter : root)
		{
			if(iter.first.empty())
				IterateJsonArray(iter.second);
			else
				IterateNormalJson(iter.first, iter.second);
		}
	}
	catch (std::exception& e)
	{
		printf("JSON parsing error, msg=%s", e.what());
	}
}



int _tmain(int argc, TCHAR* argv[])
{
	string json = "{\"id\":666, \"name\":\"Tag6\"}";
	DoJsonTest(json);

	json = "[{\"id\":1111, \"name\":\"Tag1\"}, {\"id\":2222, \"name\":\"Tag2\"}]";
	DoJsonTest(json);
}