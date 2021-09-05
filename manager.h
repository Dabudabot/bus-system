#pragma once

#include "stop.h"
#include "bus.h"
#include "json.h"
#include "graph.h"
#include "router.h"

#include <map>
#include <string>
#include <memory>
#include <sstream>

class Manager
{
public:

	Json::Document Process(Json::Document input);

protected:

	using NodesMap = std::map<std::string, Json::Node>;
	using GraphT = Graph::DirectedWeightedGraph<double>;
	using RouterT = Graph::Router<double>;

	struct RouteElement
	{
		std::string type;
		std::string name;
		double time;
		std::vector<std::string> stops;
		std::set<std::string> further_options;
	};

	void ProcessSettReq(const Json::Node& input);
	void ProcessBaseReq(const Json::Node& input);

	std::pair<std::string, std::unique_ptr<Stop>> BuildStop(const NodesMap& nmap);
	std::pair<std::string, Bus> BuildBus(const NodesMap& nmap);
	
	Json::Document ProcessStatReq(const Json::Node& input);
	Json::Node ProcessStop(const NodesMap& nmap);
	Json::Node ProcessBus(const NodesMap& nmap);
	Json::Node ProcessRoute(const NodesMap& nmap);

	size_t _wait_time = 1;
	double _bus_velo = 16.6666667;
	std::map<std::string, std::unique_ptr<Stop>> _stops;
	std::map<int, std::string> _stops_i;
	std::map<std::string, Bus> _buses;
	std::unique_ptr<GraphT> _pGraph = nullptr;
	std::unique_ptr<RouterT> _pRouter = nullptr;
};

#ifdef DAULET
#include "test_runner.h"
#endif
#ifdef DAULET
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
	/*
	void TestProcessBaseReq1()
	{
		using namespace std;
		stringstream input;
		input << "[    {      \"type\": \"Bus\",      \"name\": \"297\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryusinka\",        \"Apteka\",        \"Biryulyovo Zapadnoye\"      ],      \"is_roundtrip\": true    },    {      \"type\": \"Bus\",      \"name\": \"635\",      \"stops\": [        \"Biryulyovo Tovarnaya\",        \"Universam\",        \"Biryusinka\",        \"TETs 26\",        \"Pokrovskaya\",        \"Prazhskaya\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Bus\",      \"name\": \"828\",      \"stops\": [        \"Biryulyovo Zapadnoye\",        \"TETs 26\",        \"Biryusinka\",        \"Universam\",        \"Pokrovskaya\",        \"Rossoshanskaya ulitsa\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Tovarnaya\": 2600,        \"TETs 26\": 1100      },      \"longitude\": 37.6517,      \"name\": \"Biryulyovo Zapadnoye\",      \"latitude\": 55.574371    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryusinka\": 760,        \"Biryulyovo Tovarnaya\": 1380,        \"Pokrovskaya\": 2460      },      \"longitude\": 37.645687,      \"name\": \"Universam\",      \"latitude\": 55.587655    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Universam\": 890      },      \"longitude\": 37.653656,      \"name\": \"Biryulyovo Tovarnaya\",      \"latitude\": 55.592028    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Apteka\": 210,        \"TETs 26\": 400      },      \"longitude\": 37.64839,      \"name\": \"Biryusinka\",      \"latitude\": 55.581065    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Biryulyovo Zapadnoye\": 1420      },      \"longitude\": 37.652296,      \"name\": \"Apteka\",      \"latitude\": 55.580023    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 2850      },      \"longitude\": 37.642258,      \"name\": \"TETs 26\",      \"latitude\": 55.580685    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Rossoshanskaya ulitsa\": 3140      },      \"longitude\": 37.635517,      \"name\": \"Pokrovskaya\",      \"latitude\": 55.603601    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 3210      },      \"longitude\": 37.605757,      \"name\": \"Rossoshanskaya ulitsa\",      \"latitude\": 55.595579    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Pokrovskaya\": 2260      },      \"longitude\": 37.603938,      \"name\": \"Prazhskaya\",      \"latitude\": 55.611717    },    {      \"type\": \"Bus\",      \"name\": \"750\",      \"stops\": [        \"Tolstopaltsevo\",        \"Rasskazovka\"      ],      \"is_roundtrip\": false    },    {      \"type\": \"Stop\",      \"road_distances\": {        \"Rasskazovka\": 13800      },      \"longitude\": 37.20829,      \"name\": \"Tolstopaltsevo\",      \"latitude\": 55.611087    },    {      \"type\": \"Stop\",      \"road_distances\": {},      \"longitude\": 37.333324,      \"name\": \"Rasskazovka\",      \"latitude\": 55.632761    }  ]";

		auto jnode = Json::Load(input).GetRoot();

		ProcessBaseReq(jnode);

		ASSERT_EQUAL(_bus_velo, 1);
		ASSERT_EQUAL(_wait_time, 1);
		ASSERT_EQUAL(_stops.size(), 11);

		ASSERT(_stops.count("Biryulyovo Zapadnoye"));
		ASSERT(_stops.count("Universam"));
		ASSERT(_stops.count("Biryulyovo Tovarnaya"));
		ASSERT(_stops.count("Prazhskaya"));
		ASSERT(_stops.count("Biryusinka"));
		ASSERT(_stops.count("Apteka"));
		ASSERT(_stops.count("TETs 26"));
		ASSERT(_stops.count("Pokrovskaya"));
		ASSERT(_stops.count("Rossoshanskaya ulitsa"));
		ASSERT(_stops.count("Tolstopaltsevo"));
		ASSERT(_stops.count("Rasskazovka"));

		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->index, 0);
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->name, "Biryulyovo Zapadnoye");
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->CountBuses(), 2);
		ASSERT(_stops.at("Biryulyovo Zapadnoye")->GetBuses().count("297"));
		ASSERT(_stops.at("Biryulyovo Zapadnoye")->GetBuses().count("828"));
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->DistanceTo("Biryulyovo Tovarnaya").value(), 2600);
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->DistanceTo("Universam").value(), 2500);
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->DistanceTo("TETs 26").value(), 1100);
		ASSERT_EQUAL(_stops.at("Biryulyovo Zapadnoye")->DistanceTo("Apteka").value(), 1420);

		ASSERT_EQUAL(_stops.at("Universam")->index, 1);
		ASSERT_EQUAL(_stops.at("Universam")->name, "Universam");
		ASSERT_EQUAL(_stops.at("Universam")->CountBuses(), 3);
		ASSERT(_stops.at("Universam")->GetBuses().count("297"));
		ASSERT(_stops.at("Universam")->GetBuses().count("635"));
		ASSERT(_stops.at("Universam")->GetBuses().count("828"));
		ASSERT_EQUAL(_stops.at("Universam")->DistanceTo("Biryulyovo Tovarnaya").value(), 1380);
		ASSERT_EQUAL(_stops.at("Universam")->DistanceTo("Biryusinka").value(), 760);
		ASSERT_EQUAL(_stops.at("Universam")->DistanceTo("Pokrovskaya").value(), 2460);

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
		ASSERT_EQUAL(_buses.at("297").GetCurvature(), 1.42963);
		ASSERT_EQUAL(_buses.at("297").GetRoute(), 5990);

		ASSERT_EQUAL(_buses.at("635").CountStops(), 5);
		ASSERT_EQUAL(_buses.at("635").CountUniqueStops(), 3);
		ASSERT_EQUAL(_buses.at("635").GetCurvature(), 1.30156);
		ASSERT_EQUAL(_buses.at("635").GetRoute(), 11570);

		ASSERT(nullptr != _pGraph);
		ASSERT_EQUAL(_pGraph->GetVertexCount(), 4);
		ASSERT_EQUAL(_pGraph->GetEdgeCount(), 16);

		AssertEdges(0, 6, "297");
		AssertEdges(6, 16, "635");
	}*/
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