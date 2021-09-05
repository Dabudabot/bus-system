
#ifdef DAULET
#include "test_runner.h"
#include "profile.h"
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

#ifdef DAULET
#define MAKE_TEST_FUNCTION(n, c) void n() { c m; m.n(); }

MAKE_TEST_FUNCTION(TestBuildStop, TestManager)
MAKE_TEST_FUNCTION(TestProcessSettReq, TestManager)
MAKE_TEST_FUNCTION(TestProcessBaseReq, TestManager)
MAKE_TEST_FUNCTION(TestProcessStatReq, TestManager)
void TestCase0()
{
	stringstream input;
	input << "{\"base_requests\": [{\"type\": \"Stop\", \"name\": \"Tolstopaltsevo\", \"latitude\": 55.611087, \"longitude\": 37.20829, \"road_distances\": {\"Marushkino\": 3900}}, {\"type\": \"Stop\", \"name\": \"Marushkino\", \"latitude\": 55.595884, \"longitude\": 37.209755, \"road_distances\": {\"Rasskazovka\": 9900}}, {\"type\": \"Bus\", \"name\": \"256\", \"stops\": [\"Biryulyovo Zapadnoye\", \"Biryusinka\", \"Universam\", \"Biryulyovo Tovarnaya\", \"Biryulyovo Passazhirskaya\", \"Biryulyovo Zapadnoye\"], \"is_roundtrip\": true}, {\"type\": \"Bus\", \"name\": \"750\", \"stops\": [\"Tolstopaltsevo\", \"Marushkino\", \"Rasskazovka\"], \"is_roundtrip\": false}, {\"type\": \"Stop\", \"name\": \"Rasskazovka\", \"latitude\": 55.632761, \"longitude\": 37.333324, \"road_distances\": {}}, {\"type\": \"Stop\", \"name\": \"Biryulyovo Zapadnoye\", \"latitude\": 55.574371, \"longitude\": 37.6517, \"road_distances\": {\"Biryusinka\": 1800, \"Universam\": 2400, \"Rossoshanskaya ulitsa\": 7500}}, {\"type\": \"Stop\", \"name\": \"Biryusinka\", \"latitude\": 55.581065, \"longitude\": 37.64839, \"road_distances\": {\"Universam\": 750}}, {\"type\": \"Stop\", \"name\": \"Universam\", \"latitude\": 55.587655, \"longitude\": 37.645687, \"road_distances\": {\"Biryulyovo Tovarnaya\": 900, \"Rossoshanskaya ulitsa\": 5600}}, {\"type\": \"Stop\", \"name\": \"Biryulyovo Tovarnaya\", \"latitude\": 55.592028, \"longitude\": 37.653656, \"road_distances\": {\"Biryulyovo Passazhirskaya\": 1300}}, {\"type\": \"Stop\", \"name\": \"Biryulyovo Passazhirskaya\", \"latitude\": 55.580999, \"longitude\": 37.659164, \"road_distances\": {\"Biryulyovo Zapadnoye\": 1200}}, {\"type\": \"Bus\", \"name\": \"828\", \"stops\": [\"Biryulyovo Zapadnoye\", \"Universam\", \"Rossoshanskaya ulitsa\", \"Biryulyovo Zapadnoye\"], \"is_roundtrip\": true}, {\"type\": \"Stop\", \"name\": \"Rossoshanskaya ulitsa\", \"latitude\": 55.595579, \"longitude\": 37.605757, \"road_distances\": {}}, {\"type\": \"Stop\", \"name\": \"Prazhskaya\", \"latitude\": 55.611678, \"longitude\": 37.603831, \"road_distances\": {}}], \"stat_requests\": [{\"id\": 597682239, \"type\": \"Bus\", \"name\": \"256\"}, {\"id\": 269021385, \"type\": \"Bus\", \"name\": \"750\"}, {\"id\": 1781040762, \"type\": \"Bus\", \"name\": \"751\"}, {\"id\": 435292125, \"type\": \"Stop\", \"name\": \"Samara\"}, {\"id\": 921390765, \"type\": \"Stop\", \"name\": \"Prazhskaya\"}, {\"id\": 2042538691, \"type\": \"Stop\", \"name\": \"Biryulyovo Zapadnoye\"}]}";
	stringstream ideal;
	ideal << "[{\"request_id\": 597682239, \"stop_count\": 6, \"unique_stop_count\": 5, \"route_length\": 5950, \"curvature\": 1.3612391947315796}, {\"request_id\": 269021385, \"stop_count\": 5, \"unique_stop_count\": 3, \"route_length\": 27600, \"curvature\": 1.3180841159439354}, {\"request_id\": 1781040762, \"error_message\": \"not found\"}, {\"request_id\": 435292125, \"error_message\": \"not found\"}, {\"request_id\": 921390765, \"buses\": []}, {\"request_id\": 2042538691, \"buses\": [\"256\", \"828\"]}]";
	Json::Document jexpected = Json::Load(ideal);
	stringstream expected;
	expected << jexpected;
	
	stringstream output;

	Run(input, output);

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase1()
{
	stringstream input;
	input << "{\"routing_settings\": {\"bus_wait_time\": 6,\"bus_velocity\": 40},\"base_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryulyovo Zapadnoye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"stops\": [        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Prazhskaya\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Prazhskaya\": 4650,        \"Biryulyovo Tovarnaya\": 1380,        \"Biryulyovo Zapadnoye\": 2500      },      \"longitude\": 37.645687,      \"name\": \"Universam\",      \"latitude\": 55.587655    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Universam\": 890      },      \"longitude\": 37.653656,      \"name\": \"Biryulyovo Tovarnaya\",      \"latitude\": 55.592028    },    {      \"type\": \"Stop\",      \"road_distances\": {},      \"longitude\": 37.603938,      \"name\": \"Prazhskaya\",      \"latitude\": 55.611717    }  ],  \"stat_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"id\": 1    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"id\": 2    },    {      \"type\": \"Stop\",      \"name\": \"Universam\",      \"id\": 3    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Universam\",      \"id\": 4    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Prazhskaya\",      \"id\": 5    }  ]}";
	stringstream ideal;
	ideal << "[    {        \"curvature\": 1.42963,        \"unique_stop_count\": 3,        \"stop_count\": 4,        \"request_id\": 1,        \"route_length\": 5990    },    {        \"curvature\": 1.30156,        \"unique_stop_count\": 3,        \"stop_count\": 5,        \"request_id\": 2,        \"route_length\": 11570    },    {        \"request_id\": 3,        \"buses\": [            \"297\",            \"635\"        ]    },    {        \"total_time\": 11.235,        \"items\": [            {                \"time\": 6,                \"type\": \"Wait\",                \"stop_name\": \"Biryulyovo Zapadnoye\"            },            {                \"span_count\": 2,                \"bus\": \"297\",                \"type\": \"Bus\",                \"time\": 5.235            }        ],        \"request_id\": 4    },    {        \"total_time\": 24.21,        \"items\": [            {                \"time\": 6,                \"type\": \"Wait\",                \"stop_name\": \"Biryulyovo Zapadnoye\"            },            {                \"span_count\": 2,                \"bus\": \"297\",                \"type\": \"Bus\",                \"time\": 5.235            },            {                \"time\": 6,                \"type\": \"Wait\",                \"stop_name\": \"Universam\"            },            {                \"span_count\": 1,                \"bus\": \"635\",                \"type\": \"Bus\",                \"time\": 6.975            }        ],        \"request_id\": 5    }]";
	Json::Document jexpected = Json::Load(ideal);
	stringstream expected;
	expected << jexpected;

	stringstream output;

	Run(input, output);

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase2()
{
	stringstream input;
	input << "{  \"routing_settings\": {    \"bus_wait_time\": 2,    \"bus_velocity\": 30  },  \"base_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryusinka\",        \"Apteka\",        \"Biryulyovo Zapadnoye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"stops\": [        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryusinka\",        \"TETs 26\",        \"Pokrovskaya\",        \"Prazhskaya\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Bus\",      \"name\": \"828\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"TETs 26\",        \"Biryusinka\",        \"Universam\",        \"Pokrovskaya\",        \"Rossoshanskaya ulitsa\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600,        \"TETs 26\": 1100      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryusinka\": 760,        \"Biryulyovo Tovarnaya\": 1380,        \"Pokrovskaya\": 2460      },      \"longitude\": 37.645687,      \"name\": \"Universam\",      \"latitude\": 55.587655    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Universam\": 890      },      \"longitude\": 37.653656,      \"name\": \"Biryulyovo Tovarnaya\",      \"latitude\": 55.592028    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Apteka\": 210,        \"TETs 26\": 400      },      \"longitude\": 37.64839,      \"name\": \"Biryusinka\",      \"latitude\": 55.581065    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Zapadnoye\": 1420      },      \"longitude\": 37.652296,      \"name\": \"Apteka\",      \"latitude\": 55.580023    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 2850      },      \"longitude\": 37.642258,      \"name\": \"TETs 26\",      \"latitude\": 55.580685    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Rossoshanskaya ulitsa\": 3140      },      \"longitude\": 37.635517,      \"name\": \"Pokrovskaya\",      \"latitude\": 55.603601    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 3210      },      \"longitude\": 37.605757,      \"name\": \"Rossoshanskaya ulitsa\",      \"latitude\": 55.595579    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 2260      },      \"longitude\": 37.603938,      \"name\": \"Prazhskaya\",      \"latitude\": 55.611717    },    {      \"type\": \"Bus\",      \"name\": \"750\",      \"stops\": [        \"Tolstopaltsevo\",        \"Rasskazovka\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Rasskazovka\": 13800      },      \"longitude\": 37.20829,      \"name\": \"Tolstopaltsevo\",      \"latitude\": 55.611087    },    {      \"type\": \"Stop\",      \"road_distances\": {},      \"longitude\": 37.333324,      \"name\": \"Rasskazovka\",      \"latitude\": 55.632761    }  ],  \"stat_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"id\": 1    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"id\": 2    },    {      \"type\": \"Bus\",      \"name\": \"828\",      \"id\": 3    },    {      \"type\": \"Stop\",      \"name\": \"Universam\",      \"id\": 4    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Apteka\",      \"id\": 5    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Pokrovskaya\",      \"id\": 6    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Tovarnaya\",      \"to\": \"Pokrovskaya\",      \"id\": 7    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Tovarnaya\",      \"to\": \"Biryulyovo Zapadnoye\",      \"id\": 8    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Tovarnaya\",      \"to\": \"Prazhskaya\",      \"id\": 9    },    {      \"type\": \"Route\",      \"from\": \"Apteka\",      \"to\": \"Biryulyovo Tovarnaya\",      \"id\": 10    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Tolstopaltsevo\",      \"id\": 11    }  ]}";
	stringstream ideal;
	ideal << "[    {        \"stop_count\": 6,        \"route_length\": 5880,        \"unique_stop_count\": 5,        \"curvature\": 1.36159,        \"request_id\": 1    },    {        \"stop_count\": 11,        \"route_length\": 14810,        \"unique_stop_count\": 6,        \"curvature\": 1.12195,        \"request_id\": 2    },    {        \"stop_count\": 11,        \"route_length\": 15790,        \"unique_stop_count\": 6,        \"curvature\": 1.31245,        \"request_id\": 3    },    {        \"buses\": [            \"297\",            \"635\",            \"828\"        ],        \"request_id\": 4    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Zapadnoye\",                \"type\": \"Wait\"            },            {                \"time\": 3,                \"bus\": \"828\",                \"span_count\": 2,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Biryusinka\",                \"type\": \"Wait\"            },            {                \"time\": 0.42,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 5,        \"total_time\": 7.42    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Zapadnoye\",                \"type\": \"Wait\"            },            {                \"time\": 9.44,                \"bus\": \"828\",                \"span_count\": 4,                \"type\": \"Bus\"            }        ],        \"request_id\": 6,        \"total_time\": 11.44    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Tovarnaya\",                \"type\": \"Wait\"            },            {                \"time\": 1.78,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Universam\",                \"type\": \"Wait\"            },            {                \"time\": 4.92,                \"bus\": \"828\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 7,        \"total_time\": 10.7    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Tovarnaya\",                \"type\": \"Wait\"            },            {                \"time\": 6.56,                \"bus\": \"297\",                \"span_count\": 4,                \"type\": \"Bus\"            }        ],        \"request_id\": 8,        \"total_time\": 8.56    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Tovarnaya\",                \"type\": \"Wait\"            },            {                \"time\": 14.32,                \"bus\": \"635\",                \"span_count\": 5,                \"type\": \"Bus\"            }        ],        \"request_id\": 9,        \"total_time\": 16.32    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Apteka\",                \"type\": \"Wait\"            },            {                \"time\": 2.84,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Zapadnoye\",                \"type\": \"Wait\"            },            {                \"time\": 5.2,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 10,        \"total_time\": 12.04    },    {        \"error_message\": \"not found\",        \"request_id\": 11    }]";
	Json::Document jexpected = Json::Load(ideal);
	stringstream expected;
	expected << jexpected;

	stringstream output;

	Run(input, output);

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase3()
{
	stringstream input;
	input << "{  \"routing_settings\": {    \"bus_wait_time\": 2,    \"bus_velocity\": 30  },  \"base_requests\": [    {      \"type\": \"Bus\",      \"name\": \"289\",      \"stops\": [        \"Zagorye\",        \"Lipetskaya ulitsa 46\",        \"Lipetskaya ulitsa 40\",        \"Lipetskaya ulitsa 40\",        \"Lipetskaya ulitsa 46\",	        \"Moskvorechye\",        \"Zagorye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Lipetskaya ulitsa 46\": 230      },      \"longitude\": 37.68372,      \"name\": \"Zagorye\",      \"latitude\": 55.579909    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Lipetskaya ulitsa 40\": 390,        \"Moskvorechye\": 12400      },      \"longitude\": 37.682205,      \"name\": \"Lipetskaya ulitsa 46\",      \"latitude\": 55.581441    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Lipetskaya ulitsa 40\": 1090,        \"Lipetskaya ulitsa 46\": 380      },      \"longitude\": 37.679133,      \"name\": \"Lipetskaya ulitsa 40\",      \"latitude\": 55.584496    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Zagorye\": 10000      },      \"longitude\": 37.638433,      \"name\": \"Moskvorechye\",      \"latitude\": 55.638433    }  ],  \"stat_requests\": [    {      \"type\": \"Bus\",      \"name\": \"289\",      \"id\": 1    },    {      \"type\": \"Route\",      \"from\": \"Zagorye\",      \"to\": \"Moskvorechye\",      \"id\": 2    },    {      \"type\": \"Route\",      \"from\": \"Moskvorechye\",      \"to\": \"Zagorye\",      \"id\": 3    },    {      \"type\": \"Route\",      \"from\": \"Lipetskaya ulitsa 40\",      \"to\": \"Lipetskaya ulitsa 40\",      \"id\": 4    }  ]}";
	
	stringstream ideal;
	ideal << "[    {        \"unique_stop_count\": 4,        \"stop_count\": 7,        \"request_id\": 1,        \"curvature\": 1.63414,        \"route_length\": 24490    },    {        \"total_time\": 29.26,        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Zagorye\",                \"type\": \"Wait\"            },            {                \"time\": 0.46,                \"bus\": \"289\",                \"span_count\": 1,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Lipetskaya ulitsa 46\",                \"type\": \"Wait\"            },            {                \"time\": 24.8,                \"bus\": \"289\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 2    },    {        \"total_time\": 22,        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Moskvorechye\",                \"type\": \"Wait\"            },            {                \"time\": 20,                \"bus\": \"289\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 3    },    {        \"total_time\": 0,        \"items\": [],        \"request_id\": 4    }]";
		
	Json::Document jexpected = Json::Load(ideal);
	stringstream expected;
	expected << jexpected;

	stringstream output;

	Run(input, output);

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase4()
{
	stringstream input;
	
	stringstream ideal;
	
	Json::Document jexpected = Json::Load(ideal);
	stringstream expected;
	expected << jexpected;

	stringstream output;

	{
		LOG_DURATION("total");
		Run(input, output);
	}
	ASSERT_EQUAL(output.str(), expected.str());
}
#endif

int main()
{


#ifdef DAULET
	TestRunner tr;
	//RUN_TEST(tr, TestProfile);
	RUN_TEST(tr, TestBuildStop);
	RUN_TEST(tr, TestProcessSettReq);
	RUN_TEST(tr, TestProcessBaseReq);
	RUN_TEST(tr, TestProcessStatReq);
	RUN_TEST(tr, TestCase0);
	RUN_TEST(tr, TestCase1);
	RUN_TEST(tr, TestCase2);
	RUN_TEST(tr, TestCase3);
#endif


	ios::sync_with_stdio(false);
	cin.tie(nullptr);
	
	Run();

	return 0;
}