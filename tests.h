#pragma once

#include "manager.h"
#include <sstream>
#include <fstream>

#ifdef TEST_PROFILE
#include "profile.h"
#endif
#ifdef TEST
#include "test_runner.h"
#include <chrono>
#endif
#ifdef TEST
class TestManager : public Manager
{
public:
	void TestBuildStop()
	{
		using namespace std;
		stringstream input;
		input << "{      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    }";

		auto jmap = Json::Load(input).GetRoot().AsMap();

		auto result = BuildStop(jmap);

		ASSERT(_stops.empty());
		ASSERT_EQUAL(_stops_i.size(), 1);
		ASSERT_EQUAL(_stops_i.at(0), "Biryulyovo Zapadnoye");
		ASSERT_EQUAL(result.first, "Biryulyovo Zapadnoye");
		ASSERT_EQUAL(result.second->index, 0);
		ASSERT_EQUAL(result.second->name, "Biryulyovo Zapadnoye");
		ASSERT_EQUAL(result.second->rlat, 0.96995575364713038);
		ASSERT_EQUAL(result.second->rlon, 0.65714613395436638);
	}

	void TestProcessSettReq()
	{
		using namespace std;
		stringstream input;
		input << "{\"bus_wait_time\": 6,\"bus_velocity\": 40}";

		auto jnode = Json::Load(input).GetRoot();

		ProcessSettReq(jnode);

		AssertDouble(_bus_velo, 666.6666667, 5);
		ASSERT_EQUAL(_wait_time, 6);
	}
	void TestProcessSettReq1()
	{
		using namespace std;
		stringstream input;
		input << "{\"bus_wait_time\": 2,\"bus_velocity\": 30}";

		auto jnode = Json::Load(input).GetRoot();

		ProcessSettReq(jnode);

		AssertDouble(_bus_velo, 500.0, 4);
		ASSERT_EQUAL(_wait_time, 2);
	}
	void TestProcessBaseReq()
	{
		using namespace std;
		stringstream input;
		input << "[    {      \"type\": \"Bus\",      \"name\": \"297\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryulyovo Zapadnoye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"stops\": [        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Prazhskaya\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Prazhskaya\": 4650,        \"Biryulyovo Tovarnaya\": 1380,        \"Biryulyovo Zapadnoye\": 2500      },      \"longitude\": 37.645687,      \"name\": \"Universam\",      \"latitude\": 55.587655    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Universam\": 890      },      \"longitude\": 37.653656,      \"name\": \"Biryulyovo Tovarnaya\",      \"latitude\": 55.592028    },    {      \"type\": \"Stop\",      \"road_distances\": {},      \"longitude\": 37.603938,      \"name\": \"Prazhskaya\",      \"latitude\": 55.611717    }  ]";

		auto jnode = Json::Load(input).GetRoot();

		_bus_velo = 60;
		_wait_time = 6;

		ProcessBaseReq(jnode);

		ASSERT_EQUAL(_bus_velo, 60);
		ASSERT_EQUAL(_wait_time, 6);

		ASSERT_EQUAL(_stops.size(), 4);
		ASSERT(_stops.count("Biryulyovo Zapadnoye"));
		ASSERT(_stops.count("Universam"));
		ASSERT(_stops.count("Biryulyovo Tovarnaya"));
		ASSERT(_stops.count("Prazhskaya"));

		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->index, 0);
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->name, "Biryulyovo Zapadnoye");
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->CountBuses(), 1);
		ASSERT(_stops.at("Biryulyovo Zapadnoye")->GetBuses().count("297"));
		ASSERT(!_stops.at("Biryulyovo Zapadnoye")->GetBuses().count("635"));
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->DistanceTo("Biryulyovo Tovarnaya").value(), 2600);
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->DistanceTo("Universam").value(), 2500);
		ASSERT(!_stops.at("Biryulyovo Zapadnoye")->DistanceTo("Prazhskaya").has_value());
		ASSERT(!_stops.at("Biryulyovo Zapadnoye")->DistanceTo("Biryulyovo Zapadnoye").has_value());

		ASSERT_EQUAL(_stops.at("Universam")->index, 1);
		ASSERT_EQUAL(_stops.at("Universam")->name, "Universam");
		ASSERT_EQUAL(_stops.at("Universam")->CountBuses(), 2);
		ASSERT(_stops.at("Universam")->GetBuses().count("297"));
		ASSERT(_stops.at("Universam")->GetBuses().count("635"));
		ASSERT_EQUAL(_stops.at("Universam")->DistanceTo("Biryulyovo Tovarnaya").value(), 1380);
		ASSERT_EQUAL(_stops.at("Universam")->DistanceTo("Biryulyovo Zapadnoye").value(), 2500);
		ASSERT_EQUAL(_stops.at("Universam")->DistanceTo("Prazhskaya").value(), 4650);
		ASSERT(!_stops.at("Universam")->DistanceTo("Universam").has_value());

		ASSERT_EQUAL(_stops.at("Biryulyovo Tovarnaya")->index, 2);
		ASSERT_EQUAL(_stops.at("Biryulyovo Tovarnaya")->name, "Biryulyovo Tovarnaya");
		ASSERT_EQUAL(_stops.at("Biryulyovo Tovarnaya")->CountBuses(), 2);
		ASSERT(_stops.at("Biryulyovo Tovarnaya")->GetBuses().count("297"));
		ASSERT(_stops.at("Biryulyovo Tovarnaya")->GetBuses().count("635"));
		ASSERT_EQUAL(_stops.at("Biryulyovo Tovarnaya")->DistanceTo("Universam").value(), 890);
		ASSERT_EQUAL(_stops.at("Biryulyovo Tovarnaya")->DistanceTo("Biryulyovo Zapadnoye").value(), 2600);
		ASSERT(!_stops.at("Biryulyovo Tovarnaya")->DistanceTo("Biryulyovo Tovarnaya").has_value());
		ASSERT(!_stops.at("Biryulyovo Tovarnaya")->DistanceTo("Prazhskaya").has_value());

		ASSERT_EQUAL(_stops.at("Prazhskaya")->index, 3);
		ASSERT_EQUAL(_stops.at("Prazhskaya")->name, "Prazhskaya");
		ASSERT_EQUAL(_stops.at("Prazhskaya")->CountBuses(), 1);
		ASSERT(!_stops.at("Prazhskaya")->GetBuses().count("297"));
		ASSERT(_stops.at("Prazhskaya")->GetBuses().count("635"));
		ASSERT_EQUAL(_stops.at("Prazhskaya")->DistanceTo("Universam").value(), 4650);
		ASSERT(!_stops.at("Prazhskaya")->DistanceTo("Prazhskaya").has_value());
		ASSERT(!_stops.at("Prazhskaya")->DistanceTo("Biryulyovo Tovarnaya").has_value());
		ASSERT(!_stops.at("Prazhskaya")->DistanceTo("Biryulyovo Zapadnoye").has_value());

		ASSERT_EQUAL(_stops_i.size(), 4);
		ASSERT_EQUAL(_stops_i.at(0), "Biryulyovo Zapadnoye");
		ASSERT_EQUAL(_stops_i.at(1), "Universam");
		ASSERT_EQUAL(_stops_i.at(2), "Biryulyovo Tovarnaya");
		ASSERT_EQUAL(_stops_i.at(3), "Prazhskaya");

		ASSERT_EQUAL(_buses.size(), 2);
		ASSERT(_buses.count("297"));
		ASSERT(_buses.count("635"));

		ASSERT_EQUAL(_buses.at("297").CountStops(), 4);
		ASSERT_EQUAL(_buses.at("297").CountUniqueStops(), 3);
		AssertDouble(_buses.at("297").GetCurvature(), 1.42963, 6);
		ASSERT_EQUAL(_buses.at("297").GetRoute(), 5990);

		ASSERT_EQUAL(_buses.at("635").CountStops(), 5);
		ASSERT_EQUAL(_buses.at("635").CountUniqueStops(), 3);
		AssertDouble(_buses.at("635").GetCurvature(), 1.30156, 6);
		ASSERT_EQUAL(_buses.at("635").GetRoute(), 11570);

		ASSERT(nullptr != _pGraph);
		ASSERT_EQUAL(_pGraph->GetVertexCount(), 4);
		ASSERT_EQUAL(_pGraph->GetEdgeCount(), 16);

		ASSERT_EQUAL(_pGraph->GetEdge(0).edge_name, "297");
		ASSERT_EQUAL(_pGraph->GetEdge(0).from, 0);
		ASSERT_EQUAL(_pGraph->GetEdge(0).to, 2);
		ASSERT_EQUAL(_pGraph->GetEdge(0).vertex_count, 1);
		AssertDouble(_pGraph->GetEdge(0).weight, 49.3333336, 5);

		ASSERT_EQUAL(_pGraph->GetEdge(1).edge_name, "297");
		ASSERT_EQUAL(_pGraph->GetEdge(1).from, 0);
		ASSERT_EQUAL(_pGraph->GetEdge(1).to, 1);
		ASSERT_EQUAL(_pGraph->GetEdge(1).vertex_count, 2);
		AssertDouble(_pGraph->GetEdge(1).weight, 64.16666667, 5);

		ASSERT_EQUAL(_pGraph->GetEdge(2).edge_name, "297");
		ASSERT_EQUAL(_pGraph->GetEdge(2).from, 2);
		ASSERT_EQUAL(_pGraph->GetEdge(2).to, 1);
		ASSERT_EQUAL(_pGraph->GetEdge(2).vertex_count, 1);
		AssertDouble(_pGraph->GetEdge(2).weight, 20.8333333, 5);

		ASSERT_EQUAL(_pGraph->GetEdge(3).edge_name, "297");
		ASSERT_EQUAL(_pGraph->GetEdge(3).from, 0);
		ASSERT_EQUAL(_pGraph->GetEdge(3).to, 0);
		ASSERT_EQUAL(_pGraph->GetEdge(3).vertex_count, 3);
		AssertDouble(_pGraph->GetEdge(3).weight, 105.83333333, 5);

		ASSERT_EQUAL(_pGraph->GetEdge(4).edge_name, "297");
		ASSERT_EQUAL(_pGraph->GetEdge(4).from, 2);
		ASSERT_EQUAL(_pGraph->GetEdge(4).to, 0);
		ASSERT_EQUAL(_pGraph->GetEdge(4).vertex_count, 2);
		AssertDouble(_pGraph->GetEdge(4).weight, 62.5, 3);

		ASSERT_EQUAL(_pGraph->GetEdge(5).edge_name, "297");
		ASSERT_EQUAL(_pGraph->GetEdge(5).from, 1);
		ASSERT_EQUAL(_pGraph->GetEdge(5).to, 0);
		ASSERT_EQUAL(_pGraph->GetEdge(5).vertex_count, 1);
		AssertDouble(_pGraph->GetEdge(5).weight, 47.666666, 5);

	}

	void TestProcessStatReq()
	{
		using namespace std;
		stringstream input;
		input << "[    {      \"type\": \"Bus\",      \"name\": \"297\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryulyovo Zapadnoye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"stops\": [        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Prazhskaya\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Prazhskaya\": 4650,        \"Biryulyovo Tovarnaya\": 1380,        \"Biryulyovo Zapadnoye\": 2500      },      \"longitude\": 37.645687,      \"name\": \"Universam\",      \"latitude\": 55.587655    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Universam\": 890      },      \"longitude\": 37.653656,      \"name\": \"Biryulyovo Tovarnaya\",      \"latitude\": 55.592028    },    {      \"type\": \"Stop\",      \"road_distances\": {},      \"longitude\": 37.603938,      \"name\": \"Prazhskaya\",      \"latitude\": 55.611717    }  ]";

		auto jbase = Json::Load(input).GetRoot();

		_bus_velo = 666.6666666667;
		_wait_time = 6;

		ProcessBaseReq(jbase);

		stringstream request;
		request << "[    {      \"type\": \"Bus\",      \"name\": \"297\",      \"id\": 1    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"id\": 2    },    {      \"type\": \"Stop\",      \"name\": \"Universam\",      \"id\": 3    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Universam\",      \"id\": 4    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Prazhskaya\",      \"id\": 5    }  ]";

		auto jnode = Json::Load(request).GetRoot();

		auto jstats = ProcessStatReq(jnode).GetRoot().AsArray();

		ASSERT_EQUAL(jstats.size(), 5);

		ASSERT_EQUAL(jstats[0].AsMap().size(), 5);
		AssertDouble(jstats[0].AsMap().at("curvature").AsDouble(), 1.42963, 6);
		ASSERT_EQUAL(jstats[0].AsMap().at("unique_stop_count").AsInt(), 3);
		ASSERT_EQUAL(jstats[0].AsMap().at("stop_count").AsInt(), 4);
		ASSERT_EQUAL(jstats[0].AsMap().at("request_id").AsInt(), 1);
		ASSERT_EQUAL(jstats[0].AsMap().at("route_length").AsInt(), 5990);

		ASSERT_EQUAL(jstats[1].AsMap().size(), 5);
		AssertDouble(jstats[1].AsMap().at("curvature").AsDouble(), 1.30156, 6);
		ASSERT_EQUAL(jstats[1].AsMap().at("unique_stop_count").AsInt(), 3);
		ASSERT_EQUAL(jstats[1].AsMap().at("stop_count").AsInt(), 5);
		ASSERT_EQUAL(jstats[1].AsMap().at("request_id").AsInt(), 2);
		ASSERT_EQUAL(jstats[1].AsMap().at("route_length").AsInt(), 11570);

		ASSERT_EQUAL(jstats[2].AsMap().size(), 2);
		ASSERT_EQUAL(jstats[2].AsMap().at("request_id").AsInt(), 3);
		ASSERT_EQUAL(jstats[2].AsMap().at("buses").AsArray().size(), 2);
		ASSERT_EQUAL(jstats[2].AsMap().at("buses").AsArray()[0].AsString(), "297");
		ASSERT_EQUAL(jstats[2].AsMap().at("buses").AsArray()[1].AsString(), "635");

		ASSERT_EQUAL(jstats[3].AsMap().size(), 3);
		ASSERT_EQUAL(jstats[3].AsMap().at("request_id").AsInt(), 4);
		AssertDouble(jstats[3].AsMap().at("total_time").AsDouble(), 11.235, 5);

		ASSERT_EQUAL(jstats[3].AsMap().at("items").AsArray().size(), 2);

		ASSERT_EQUAL(jstats[3].AsMap().at("items").AsArray()[0].AsMap().at("time").AsInt(), 6);
		ASSERT_EQUAL(jstats[3].AsMap().at("items").AsArray()[0].AsMap().at("type").AsString(), "Wait");
		ASSERT_EQUAL(jstats[3].AsMap().at("items").AsArray()[0].AsMap().at("stop_name").AsString(), "Biryulyovo Zapadnoye");

		ASSERT_EQUAL(jstats[3].AsMap().at("items").AsArray()[1].AsMap().at("span_count").AsInt(), 2);
		ASSERT_EQUAL(jstats[3].AsMap().at("items").AsArray()[1].AsMap().at("bus").AsString(), "297");
		ASSERT_EQUAL(jstats[3].AsMap().at("items").AsArray()[1].AsMap().at("type").AsString(), "Bus");
		AssertDouble(jstats[3].AsMap().at("items").AsArray()[1].AsMap().at("time").AsDouble(), 5.235, 4);

		ASSERT_EQUAL(jstats[4].AsMap().size(), 3);
		ASSERT_EQUAL(jstats[4].AsMap().at("request_id").AsInt(), 5);
		AssertDouble(jstats[4].AsMap().at("total_time").AsDouble(), 24.21, 4);

		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray().size(), 4);

		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[0].AsMap().at("time").AsInt(), 6);
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[0].AsMap().at("type").AsString(), "Wait");
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[0].AsMap().at("stop_name").AsString(), "Biryulyovo Zapadnoye");

		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[1].AsMap().at("span_count").AsInt(), 2);
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[1].AsMap().at("bus").AsString(), "297");
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[1].AsMap().at("type").AsString(), "Bus");
		AssertDouble(jstats[4].AsMap().at("items").AsArray()[1].AsMap().at("time").AsDouble(), 5.235, 4);

		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[2].AsMap().at("time").AsInt(), 6);
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[2].AsMap().at("type").AsString(), "Wait");
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[2].AsMap().at("stop_name").AsString(), "Universam");

		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[3].AsMap().at("span_count").AsInt(), 1);
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[3].AsMap().at("bus").AsString(), "635");
		ASSERT_EQUAL(jstats[4].AsMap().at("items").AsArray()[3].AsMap().at("type").AsString(), "Bus");
		AssertDouble(jstats[4].AsMap().at("items").AsArray()[3].AsMap().at("time").AsDouble(), 6.975, 4);
	}

	void AssertDouble(double result, double expected, size_t round)
	{
		using namespace std;
		stringstream results;
		stringstream expecteds;

		results << setprecision(round) << result;
		expecteds << setprecision(round) << expected;

		ASSERT_EQUAL(results.str(), expecteds.str());
	}
};
#endif

#ifdef TEST
#define MAKE_TEST_FUNCTION(n, c) void n() { c m; m.n(); }

MAKE_TEST_FUNCTION(TestBuildStop, TestManager)
MAKE_TEST_FUNCTION(TestProcessSettReq, TestManager)
MAKE_TEST_FUNCTION(TestProcessBaseReq, TestManager)
MAKE_TEST_FUNCTION(TestProcessStatReq, TestManager)
void TestCase0()
{
	std::stringstream input;
	input << "{\"base_requests\": [{\"type\": \"Stop\", \"name\": \"Tolstopaltsevo\", \"latitude\": 55.611087, \"longitude\": 37.20829, \"road_distances\": {\"Marushkino\": 3900}}, {\"type\": \"Stop\", \"name\": \"Marushkino\", \"latitude\": 55.595884, \"longitude\": 37.209755, \"road_distances\": {\"Rasskazovka\": 9900}}, {\"type\": \"Bus\", \"name\": \"256\", \"stops\": [\"Biryulyovo Zapadnoye\", \"Biryusinka\", \"Universam\", \"Biryulyovo Tovarnaya\", \"Biryulyovo Passazhirskaya\", \"Biryulyovo Zapadnoye\"], \"is_roundtrip\": true}, {\"type\": \"Bus\", \"name\": \"750\", \"stops\": [\"Tolstopaltsevo\", \"Marushkino\", \"Rasskazovka\"], \"is_roundtrip\": false}, {\"type\": \"Stop\", \"name\": \"Rasskazovka\", \"latitude\": 55.632761, \"longitude\": 37.333324, \"road_distances\": {}}, {\"type\": \"Stop\", \"name\": \"Biryulyovo Zapadnoye\", \"latitude\": 55.574371, \"longitude\": 37.6517, \"road_distances\": {\"Biryusinka\": 1800, \"Universam\": 2400, \"Rossoshanskaya ulitsa\": 7500}}, {\"type\": \"Stop\", \"name\": \"Biryusinka\", \"latitude\": 55.581065, \"longitude\": 37.64839, \"road_distances\": {\"Universam\": 750}}, {\"type\": \"Stop\", \"name\": \"Universam\", \"latitude\": 55.587655, \"longitude\": 37.645687, \"road_distances\": {\"Biryulyovo Tovarnaya\": 900, \"Rossoshanskaya ulitsa\": 5600}}, {\"type\": \"Stop\", \"name\": \"Biryulyovo Tovarnaya\", \"latitude\": 55.592028, \"longitude\": 37.653656, \"road_distances\": {\"Biryulyovo Passazhirskaya\": 1300}}, {\"type\": \"Stop\", \"name\": \"Biryulyovo Passazhirskaya\", \"latitude\": 55.580999, \"longitude\": 37.659164, \"road_distances\": {\"Biryulyovo Zapadnoye\": 1200}}, {\"type\": \"Bus\", \"name\": \"828\", \"stops\": [\"Biryulyovo Zapadnoye\", \"Universam\", \"Rossoshanskaya ulitsa\", \"Biryulyovo Zapadnoye\"], \"is_roundtrip\": true}, {\"type\": \"Stop\", \"name\": \"Rossoshanskaya ulitsa\", \"latitude\": 55.595579, \"longitude\": 37.605757, \"road_distances\": {}}, {\"type\": \"Stop\", \"name\": \"Prazhskaya\", \"latitude\": 55.611678, \"longitude\": 37.603831, \"road_distances\": {}}], \"stat_requests\": [{\"id\": 597682239, \"type\": \"Bus\", \"name\": \"256\"}, {\"id\": 269021385, \"type\": \"Bus\", \"name\": \"750\"}, {\"id\": 1781040762, \"type\": \"Bus\", \"name\": \"751\"}, {\"id\": 435292125, \"type\": \"Stop\", \"name\": \"Samara\"}, {\"id\": 921390765, \"type\": \"Stop\", \"name\": \"Prazhskaya\"}, {\"id\": 2042538691, \"type\": \"Stop\", \"name\": \"Biryulyovo Zapadnoye\"}]}";
	std::stringstream ideal;
	ideal << "[{\"request_id\": 597682239, \"stop_count\": 6, \"unique_stop_count\": 5, \"route_length\": 5950, \"curvature\": 1.3612391947315796}, {\"request_id\": 269021385, \"stop_count\": 5, \"unique_stop_count\": 3, \"route_length\": 27600, \"curvature\": 1.3180841159439354}, {\"request_id\": 1781040762, \"error_message\": \"not found\"}, {\"request_id\": 435292125, \"error_message\": \"not found\"}, {\"request_id\": 921390765, \"buses\": []}, {\"request_id\": 2042538691, \"buses\": [\"256\", \"828\"]}]";
	Json::Document jexpected = Json::Load(ideal);
	std::stringstream expected;
	expected << jexpected;

	std::stringstream output;

	Manager m;
	output << m.Process(Json::Load(input));

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase1()
{
	std::stringstream input;
	input << "{\"routing_settings\": {\"bus_wait_time\": 6,\"bus_velocity\": 40},\"base_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryulyovo Zapadnoye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"stops\": [        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Prazhskaya\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Prazhskaya\": 4650,        \"Biryulyovo Tovarnaya\": 1380,        \"Biryulyovo Zapadnoye\": 2500      },      \"longitude\": 37.645687,      \"name\": \"Universam\",      \"latitude\": 55.587655    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Universam\": 890      },      \"longitude\": 37.653656,      \"name\": \"Biryulyovo Tovarnaya\",      \"latitude\": 55.592028    },    {      \"type\": \"Stop\",      \"road_distances\": {},      \"longitude\": 37.603938,      \"name\": \"Prazhskaya\",      \"latitude\": 55.611717    }  ],  \"stat_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"id\": 1    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"id\": 2    },    {      \"type\": \"Stop\",      \"name\": \"Universam\",      \"id\": 3    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Universam\",      \"id\": 4    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Prazhskaya\",      \"id\": 5    }  ]}";
	std::stringstream ideal;
	ideal << "[    {        \"curvature\": 1.42963,        \"unique_stop_count\": 3,        \"stop_count\": 4,        \"request_id\": 1,        \"route_length\": 5990    },    {        \"curvature\": 1.30156,        \"unique_stop_count\": 3,        \"stop_count\": 5,        \"request_id\": 2,        \"route_length\": 11570    },    {        \"request_id\": 3,        \"buses\": [            \"297\",            \"635\"        ]    },    {        \"total_time\": 11.235,        \"items\": [            {                \"time\": 6,                \"type\": \"Wait\",                \"stop_name\": \"Biryulyovo Zapadnoye\"            },            {                \"span_count\": 2,                \"bus\": \"297\",                \"type\": \"Bus\",                \"time\": 5.235            }        ],        \"request_id\": 4    },    {        \"total_time\": 24.21,        \"items\": [            {                \"time\": 6,                \"type\": \"Wait\",                \"stop_name\": \"Biryulyovo Zapadnoye\"            },            {                \"span_count\": 2,                \"bus\": \"297\",                \"type\": \"Bus\",                \"time\": 5.235            },            {                \"time\": 6,                \"type\": \"Wait\",                \"stop_name\": \"Universam\"            },            {                \"span_count\": 1,                \"bus\": \"635\",                \"type\": \"Bus\",                \"time\": 6.975            }        ],        \"request_id\": 5    }]";
	Json::Document jexpected = Json::Load(ideal);
	std::stringstream expected;
	expected << jexpected;

	std::stringstream output;

	Manager m;
	output << m.Process(Json::Load(input));

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase2()
{
	std::stringstream input;
	input << "{  \"routing_settings\": {    \"bus_wait_time\": 2,    \"bus_velocity\": 30  },  \"base_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryusinka\",        \"Apteka\",        \"Biryulyovo Zapadnoye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"stops\": [        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryusinka\",        \"TETs 26\",        \"Pokrovskaya\",        \"Prazhskaya\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Bus\",      \"name\": \"828\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"TETs 26\",        \"Biryusinka\",        \"Universam\",        \"Pokrovskaya\",        \"Rossoshanskaya ulitsa\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600,        \"TETs 26\": 1100      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryusinka\": 760,        \"Biryulyovo Tovarnaya\": 1380,        \"Pokrovskaya\": 2460      },      \"longitude\": 37.645687,      \"name\": \"Universam\",      \"latitude\": 55.587655    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Universam\": 890      },      \"longitude\": 37.653656,      \"name\": \"Biryulyovo Tovarnaya\",      \"latitude\": 55.592028    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Apteka\": 210,        \"TETs 26\": 400      },      \"longitude\": 37.64839,      \"name\": \"Biryusinka\",      \"latitude\": 55.581065    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Zapadnoye\": 1420      },      \"longitude\": 37.652296,      \"name\": \"Apteka\",      \"latitude\": 55.580023    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 2850      },      \"longitude\": 37.642258,      \"name\": \"TETs 26\",      \"latitude\": 55.580685    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Rossoshanskaya ulitsa\": 3140      },      \"longitude\": 37.635517,      \"name\": \"Pokrovskaya\",      \"latitude\": 55.603601    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 3210      },      \"longitude\": 37.605757,      \"name\": \"Rossoshanskaya ulitsa\",      \"latitude\": 55.595579    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 2260      },      \"longitude\": 37.603938,      \"name\": \"Prazhskaya\",      \"latitude\": 55.611717    },    {      \"type\": \"Bus\",      \"name\": \"750\",      \"stops\": [        \"Tolstopaltsevo\",        \"Rasskazovka\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Rasskazovka\": 13800      },      \"longitude\": 37.20829,      \"name\": \"Tolstopaltsevo\",      \"latitude\": 55.611087    },    {      \"type\": \"Stop\",      \"road_distances\": {},      \"longitude\": 37.333324,      \"name\": \"Rasskazovka\",      \"latitude\": 55.632761    }  ],  \"stat_requests\": [    {      \"type\": \"Bus\",      \"name\": \"297\",      \"id\": 1    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"id\": 2    },    {      \"type\": \"Bus\",      \"name\": \"828\",      \"id\": 3    },    {      \"type\": \"Stop\",      \"name\": \"Universam\",      \"id\": 4    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Apteka\",      \"id\": 5    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Pokrovskaya\",      \"id\": 6    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Tovarnaya\",      \"to\": \"Pokrovskaya\",      \"id\": 7    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Tovarnaya\",      \"to\": \"Biryulyovo Zapadnoye\",      \"id\": 8    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Tovarnaya\",      \"to\": \"Prazhskaya\",      \"id\": 9    },    {      \"type\": \"Route\",      \"from\": \"Apteka\",      \"to\": \"Biryulyovo Tovarnaya\",      \"id\": 10    },    {      \"type\": \"Route\",      \"from\": \"Biryulyovo Zapadnoye\",      \"to\": \"Tolstopaltsevo\",      \"id\": 11    }  ]}";
	std::stringstream ideal;
	ideal << "[    {        \"stop_count\": 6,        \"route_length\": 5880,        \"unique_stop_count\": 5,        \"curvature\": 1.36159,        \"request_id\": 1    },    {        \"stop_count\": 11,        \"route_length\": 14810,        \"unique_stop_count\": 6,        \"curvature\": 1.12195,        \"request_id\": 2    },    {        \"stop_count\": 11,        \"route_length\": 15790,        \"unique_stop_count\": 6,        \"curvature\": 1.31245,        \"request_id\": 3    },    {        \"buses\": [            \"297\",            \"635\",            \"828\"        ],        \"request_id\": 4    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Zapadnoye\",                \"type\": \"Wait\"            },            {                \"time\": 3,                \"bus\": \"828\",                \"span_count\": 2,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Biryusinka\",                \"type\": \"Wait\"            },            {                \"time\": 0.42,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 5,        \"total_time\": 7.42    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Zapadnoye\",                \"type\": \"Wait\"            },            {                \"time\": 9.44,                \"bus\": \"828\",                \"span_count\": 4,                \"type\": \"Bus\"            }        ],        \"request_id\": 6,        \"total_time\": 11.44    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Tovarnaya\",                \"type\": \"Wait\"            },            {                \"time\": 1.78,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Universam\",                \"type\": \"Wait\"            },            {                \"time\": 4.92,                \"bus\": \"828\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 7,        \"total_time\": 10.7    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Tovarnaya\",                \"type\": \"Wait\"            },            {                \"time\": 6.56,                \"bus\": \"297\",                \"span_count\": 4,                \"type\": \"Bus\"            }        ],        \"request_id\": 8,        \"total_time\": 8.56    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Tovarnaya\",                \"type\": \"Wait\"            },            {                \"time\": 14.32,                \"bus\": \"635\",                \"span_count\": 5,                \"type\": \"Bus\"            }        ],        \"request_id\": 9,        \"total_time\": 16.32    },    {        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Apteka\",                \"type\": \"Wait\"            },            {                \"time\": 2.84,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Biryulyovo Zapadnoye\",                \"type\": \"Wait\"            },            {                \"time\": 5.2,                \"bus\": \"297\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 10,        \"total_time\": 12.04    },    {        \"error_message\": \"not found\",        \"request_id\": 11    }]";
	Json::Document jexpected = Json::Load(ideal);
	std::stringstream expected;
	expected << jexpected;

	std::stringstream output;

	Manager m;
	output << m.Process(Json::Load(input));

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase3()
{
	std::stringstream input;
	input << "{  \"routing_settings\": {    \"bus_wait_time\": 2,    \"bus_velocity\": 30  },  \"base_requests\": [    {      \"type\": \"Bus\",      \"name\": \"289\",      \"stops\": [        \"Zagorye\",        \"Lipetskaya ulitsa 46\",        \"Lipetskaya ulitsa 40\",        \"Lipetskaya ulitsa 40\",        \"Lipetskaya ulitsa 46\",	        \"Moskvorechye\",        \"Zagorye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Lipetskaya ulitsa 46\": 230      },      \"longitude\": 37.68372,      \"name\": \"Zagorye\",      \"latitude\": 55.579909    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Lipetskaya ulitsa 40\": 390,        \"Moskvorechye\": 12400      },      \"longitude\": 37.682205,      \"name\": \"Lipetskaya ulitsa 46\",      \"latitude\": 55.581441    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Lipetskaya ulitsa 40\": 1090,        \"Lipetskaya ulitsa 46\": 380      },      \"longitude\": 37.679133,      \"name\": \"Lipetskaya ulitsa 40\",      \"latitude\": 55.584496    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Zagorye\": 10000      },      \"longitude\": 37.638433,      \"name\": \"Moskvorechye\",      \"latitude\": 55.638433    }  ],  \"stat_requests\": [    {      \"type\": \"Bus\",      \"name\": \"289\",      \"id\": 1    },    {      \"type\": \"Route\",      \"from\": \"Zagorye\",      \"to\": \"Moskvorechye\",      \"id\": 2    },    {      \"type\": \"Route\",      \"from\": \"Moskvorechye\",      \"to\": \"Zagorye\",      \"id\": 3    },    {      \"type\": \"Route\",      \"from\": \"Lipetskaya ulitsa 40\",      \"to\": \"Lipetskaya ulitsa 40\",      \"id\": 4    }  ]}";

	std::stringstream ideal;
	ideal << "[    {        \"unique_stop_count\": 4,        \"stop_count\": 7,        \"request_id\": 1,        \"curvature\": 1.63414,        \"route_length\": 24490    },    {        \"total_time\": 29.26,        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Zagorye\",                \"type\": \"Wait\"            },            {                \"time\": 0.46,                \"bus\": \"289\",                \"span_count\": 1,                \"type\": \"Bus\"            },            {                \"time\": 2,                \"stop_name\": \"Lipetskaya ulitsa 46\",                \"type\": \"Wait\"            },            {                \"time\": 24.8,                \"bus\": \"289\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 2    },    {        \"total_time\": 22,        \"items\": [            {                \"time\": 2,                \"stop_name\": \"Moskvorechye\",                \"type\": \"Wait\"            },            {                \"time\": 20,                \"bus\": \"289\",                \"span_count\": 1,                \"type\": \"Bus\"            }        ],        \"request_id\": 3    },    {        \"total_time\": 0,        \"items\": [],        \"request_id\": 4    }]";

	Json::Document jexpected = Json::Load(ideal);
	std::stringstream expected;
	expected << jexpected;

	std::stringstream output;

	Manager m;
	output << m.Process(Json::Load(input));

	ASSERT_EQUAL(output.str(), expected.str());
}
void TestCase4()
{
	std::ifstream input("task/transport-input4.json");

	std::ifstream ideal("task/transport-output4.json");

	Json::Document jexpected = Json::Load(ideal);
	std::stringstream expected;
	expected << jexpected;

	std::stringstream output;

	
	auto start = std::chrono::steady_clock::now();
	Manager m;
	output << m.Process(Json::Load(input));
	auto finish = std::chrono::steady_clock::now();
	auto dur = finish - start;

#ifdef NDEBUG
	ASSERT(std::chrono::duration_cast<std::chrono::milliseconds>(dur).count() <= 3000);
#endif
	//ASSERT_EQUAL(output.str(), expected.str());
}
#endif