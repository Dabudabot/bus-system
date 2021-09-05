
#ifdef TEST
#include "tests.h"
#endif

#include "manager.h"
#include "json.h"

#include <iostream>
#include "router.h"

using namespace std;

void Run(istream& in = cin, ostream& out = cout)
{
	Manager m;
#if 1
	out << m.Process(Json::Load(in));
#else
	stringstream input;
	string copy = "";
	string line = "";

	while (getline(in, line))
	{
		copy += line;
		copy += "\n";
	}

	input << copy;

	Manager m;
	try
	{
		out << m.Process(Json::Load(input));
	}
	catch (exception& e)
	{
		stringstream ss;
		ss << e.what() << '\n' << copy;
		throw invalid_argument(ss.str());
	}
#endif
}



int main()
{


#ifdef TEST
	TestRunner tr;
	RUN_TEST(tr, TestBuildStop);
	RUN_TEST(tr, TestProcessSettReq);
	RUN_TEST(tr, TestProcessBaseReq);
	RUN_TEST(tr, TestProcessStatReq);
	RUN_TEST(tr, TestCase0);
	RUN_TEST(tr, TestCase1);
	RUN_TEST(tr, TestCase2);
	RUN_TEST(tr, TestCase3);
	RUN_TEST(tr, TestCase4);
#endif


	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	
	Run();

	return 0;
}