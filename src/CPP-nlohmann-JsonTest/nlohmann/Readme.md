# C++ JSON library
C++ JSON library from https://github.com/nlohmann/json  
Version: v3.11.2  
MIT license  

## single_include files only
We only need the files in single_include/nlohmann/
single_include/nlohmann/json.hpp  
single_include/nlohmann/json_fwd.hpp  

Replace "std::snprintf" by "::snprintf" to fix:  
error C2039: '_snprintf': is not a member of 'std'


## Compare with boost::property_tree::ptree
boost::property_tree::ptree is used widely to parse and compose JSON.
ptree is ok for JSON parse, but not so good for JSON composer.
For exampe ptree does not support number value.

This library has intuitive JSON syntax, trivial integration and serious testing.
It's a better alternative for ptree.
