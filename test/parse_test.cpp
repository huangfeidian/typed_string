

#include <iostream>
#include <unordered_map>

#include <arena_typed_string.h>
#include <arena_typed_string_parser.h>

using namespace std;
using namespace spiritsaway;
using namespace spiritsaway::container;
bool test_type_parse()
{
    vector<string> valid_inputs = {
        "int",
        "str",
        "bool",
        "uint32",
        "int",
        "uint64",
        "int64",
        "float",
        "double",
        "choice_str(red, blue)",
		"choice_int(0,1,2)",
		"choice_str(red, blue , green )",
        "list(int, 2)",
        "list(str, 2, #)",
        "list(str, 0, #)"
        "tuple(int, str)",
        "tuple(str, str, #)",
        "list(choice_int(0,1,2), 3)",
        "list(list(int, 3), 3, #)",
        "list(list(int, 0), 3, #)",
        "list(list(int, 0), 0, #)",
        "tuple(tuple(int, int), tuple(str, str), #)",
        "tuple(tuple(int, int), tuple(str, str), #)",
        "tuple(list(int, 3), list(str, 3), #)",
        "tuple(choice_str(red, blue, green), choice_str(red, blue, green), choice_str(red, blue, green))",
        "list(tuple(choice_int(0,1,2), choice_int(0,1,2), #), 3, ?)",
    };
	bool failed = false;
	memory::arena cur_arena(4 * 1024);

    for(const auto& i : valid_inputs)
    {
        auto current_type = typed_string_desc::get_type_from_str(&cur_arena, string_view(i));
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

bool test_type_value_parse()
{
	memory::arena cur_arena(4 * 1024);
	arena_typed_string_parser cur_parser(cur_arena);
	unordered_map<string, vector<string>> typed_values = {
		{"int", {"0", "1", "-1", "-1314598", "314159"}},
		{"uint32", {"0", "1", "10", "314159"}},
		{"uint64", {"0", "1", "200000000000"}},
		{"int64", {"0", "-1", "1", "20000000000000", "-20000000000000"}},
		{"float", {"0.0", "-0.0", "1.0", "-1.0", "3.14159", "-3141519"}},
		{"double", {"0.0", "-0.0", "3.141592657385", "-3.141592657384"}},
		{"choice_str(red, blue, green)", {"blue", "green", "red"}},
		{ "choice_int(0,1,2,3)",{ "1", "2", "3" } },
		{"tuple(int, str)", {"(3, 5)", "(4, hehe)", "(5, -1)", "(0, fail)"}},
		{"tuple(str, str, #)", {"(wala , #hehe)", "(wawa,#hehe)", "( wawa, # hehe)", "(	wawa	, #    hehe )"} },
		{"list(choice_str(red, blue, green), 3)", {"(red,red, red)", "(	blue , blue, green)"} },
		{"list(int, 3)", {"(1,2,3)"}},
		{"list(list(int, 3), 3, #)", {"((1,2,3)#(2,3,4)#(3,4,5))"}},
		{"list(list(int, 0), 3, #)", {"((1)#(1,2)#(1,2,3))"}},
		{"list(list(int, 0), 0, #)",{ "((1)#(1,2)#(1,2,3)#(1,2,3,4))" } },
		{"tuple(tuple(int,int), tuple(str,str), #)", {"((1,2)#(h, k))"}},
		{"tuple(tuple(int,int), tuple(str, str), #)", {"((1,2)#(h,k))"}},
		{"tuple(list(int, 3), list(int, 3), #)", {"((1,2,3)#(2,3,4))"}},
		{"tuple(choice_int(0,1), choice_int(2, 3), choice_int(4, 5))", {"(1, 2, 5)"}},
		{"list(tuple(choice_int(0,1), choice_int(1,2), #),3,?)", {"((1#1)?(0#1)?(1#2))"}}
	};
	bool failed = false;
	for (const auto& i : typed_values)
	{
		auto type_str = i.first;
		auto value_strs = i.second;
		auto current_type = typed_string_desc::get_type_from_str(&cur_arena, string_view(type_str));
		for (const auto & one_value : value_strs)
		{
			auto current_value = cur_parser.parse_value_with_type(current_type, string_view(one_value));
			if (current_value)
			{
				cout << "parse type "<<*current_type<<" with value "<< one_value<<" match with result "<<*current_value << endl;
			}
			else
			{
				cout << "fail for type " << *current_type << " with value " << one_value << endl;
				failed = true;
				return false;
			}
		}
		
	}
	return true;
}


int main()
{

	test_type_value_parse();
	test_type_parse();
	

	return 0;
}