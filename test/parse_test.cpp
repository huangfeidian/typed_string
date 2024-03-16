

#include <iostream>
#include <unordered_map>

#include "typed_string_desc.h"

using namespace std;
using namespace spiritsaway;
using namespace spiritsaway::container;
bool test_type_parse()
{
    vector<string> valid_inputs = {
		"any",
        "int",
        "str",
        "bool",
        "uint",
        "float",
        R"({"str": ["red", "blue"] })",
		R"({"int": [0,1,2]})",
		R"({"str": ["red", "blue", "green"] })",
        R"(["int", 2])",
		R"(["str", 2])",
		R"(["str", 0])",
		R"(["str", "int"])",
		R"(["str", "str"])",
		R"([{"int": [0,1,2]}, 3])",
		R"([["int", 3], 3])",
		R"([["int", 3], 0])",
		R"([["int", 0], 3])",
        R"([["int", "int"], ["str", "str"]])",
		R"([["int", 3], ["str", 3]])",
		R"([{"str": ["red", "blue", "green"] }, 3])",
		R"([{"str": ["red", "blue", "green"] }, {"str": ["red", "blue", "green"] }])",
    };
	bool failed = false;

    for(const auto& i : valid_inputs)
    {
        auto current_type = typed_string_desc::get_type_from_str(string_view(i));
		if (current_type)
		{
			cout << *current_type << endl;
		}
		else
		{
			cout << "fail for " << i << endl;
			failed = true;
		}
        
    }
	return failed;
}



int main()
{

	test_type_parse();
	

	return 0;
}