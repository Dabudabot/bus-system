
#ifdef TEST
#include "profile.h"
#endif
#include "manager.h"

#include "router.h"
#include <optional>

using namespace std;
using namespace Json;

Json::Document Manager::Process(Json::Document input)
{
#ifdef TEST_PROFILE
	LOG_DURATION("Process");
#endif
	auto rootm = input.GetRoot().AsMap();

	if (rootm.count("routing_settings"))
	{
		ProcessSettReq(rootm.at("routing_settings"));
	}
	
	ProcessBaseReq(rootm.at("base_requests"));
	return ProcessStatReq(rootm.at("stat_requests"));
}

void Manager::ProcessSettReq(const Json::Node& input)
{
#ifdef TEST_PROFILE
	LOG_DURATION("ProcessSettReq");
#endif
	auto m = input.AsMap();
	_wait_time = m.at("bus_wait_time").AsInt();
	_bus_velo = m.at("bus_velocity").AsInt() * 1000.0 / 60;
}

void Manager::ProcessBaseReq(const Json::Node& input)
{
#ifdef TEST_PROFILE
	LOG_DURATION("ProcessBaseReq");
#endif
	auto arr = input.AsArray();

	{
#ifdef TEST_PROFILE
		LOG_DURATION("ProcessBaseReq: stops");
#endif
		// processing all stops
		for (const auto& elem : arr)
		{
			auto m = elem.AsMap();

			if (m.at("type").AsString() != "Stop")
			{
				continue;
			}

			_stops.insert(BuildStop(m));
		}
	}

	_pGraph = make_unique<GraphT>(_stops.size());

	{
#ifdef TEST_PROFILE
		LOG_DURATION("ProcessBaseReq: buses");
#endif
		// processing all buses
		for (const auto& elem : arr)
		{
			auto m = elem.AsMap();

			if (m.at("type").AsString() != "Bus")
			{
				continue;
			}

			_buses.insert(BuildBus(m));
		}
	}

	_pRouter = make_unique<RouterT>(*_pGraph);
}

pair<string, unique_ptr<Stop>> Manager::BuildStop(const NodesMap& nmap)
{
	auto name = nmap.at("name").AsString();

	auto stop = make_unique<Stop>(
		name, 
		nmap.at("latitude").AsDouble(), 
		nmap.at("longitude").AsDouble(),
		_stops.size());
	_stops_i.insert({ _stops.size(), name });

	// check if oldies already have distance to us
	for (const auto& [another_stop, pstop] : _stops)
	{
		auto dist = pstop->DistanceTo(name);
		if (dist != nullopt)
		{
			stop->AddDistance(another_stop, dist.value());
		}
	}

	for (const auto& [target_name, ndist] : nmap.at("road_distances").AsMap())
	{
		stop->AddDistance(target_name, ndist.AsInt());
	}

	// check if we got distance to some oldie that hasn`t
	for (const auto& [another_stop, pstop] : _stops)
	{
		auto dist = stop->DistanceTo(another_stop);
		if (dist != nullopt && pstop->DistanceTo(name) == nullopt)
		{
			pstop->AddDistance(name, dist.value());
		}
	}

	return { move(name), move(stop) };
}

pair<string, Bus> Manager::BuildBus(const NodesMap& nmap)
{
	auto name = nmap.at("name").AsString();

	Bus bus(name, _bus_velo, _wait_time);

	for (const auto& nstop : nmap.at("stops").AsArray())
	{
		auto stop = nstop.AsString();
		auto edges = bus.AddStop(_stops.at(stop).get());
		_stops.at(stop)->AddBus(name);
		_pGraph->AddEdges(edges);
	}

	if (!nmap.at("is_roundtrip").AsBoolean())
	{
		_pGraph->AddEdges(bus.RepeatStops());
	}

	return { move(name), move(bus) };
}

Document Manager::ProcessStatReq(const Node& input)
{
#ifdef TEST_PROFILE
	LOG_DURATION("ProcessStatReq");
#endif
	auto arr = input.AsArray();

	vector<Node> root(arr.size());

	for (size_t i = 0; i < arr.size(); ++i)
	{
		auto m = arr[i].AsMap();
		auto type = m.at("type").AsString();

		if (type == "Stop")
		{
			root[i] = ProcessStop(m);
		}
		else if (type == "Bus")
		{
			root[i] = ProcessBus(m);
		}
		else if (type == "Route")
		{
			root[i] = ProcessRoute(m);
		}
	}

	return Document(move(root));
}

Node Manager::ProcessStop(const NodesMap& nmap)
{
#ifdef TEST_PROFILE
	LOG_DURATION("ProcessStop");
#endif
	NodesMap answer;
	answer.insert({ "request_id", nmap.at("id") });

	auto name = nmap.at("name").AsString();

	if (!_stops.count(name))
	{
		answer.insert({ "error_message", string("not found")});
		return answer;
	}

	auto count_buses = _stops.at(name)->CountBuses();
	
	vector<Node> nbuses(count_buses);
	size_t i = 0;
	for (const auto& bus_name : _stops.at(name)->GetBuses())
	{
		nbuses[i++] = Node(bus_name);
	}
	answer.insert({"buses", move(nbuses)});

	return answer;
}

Node Manager::ProcessBus(const NodesMap& nmap)
{
#ifdef TEST_PROFILE
	LOG_DURATION("ProcessBus");
#endif
	NodesMap answer;
	answer.insert({ "request_id", nmap.at("id") });

	auto name = nmap.at("name").AsString();

	if (!_buses.count(name))
	{
		answer.insert({ "error_message", string("not found") });
		return answer;
	}

	answer.insert({"route_length", static_cast<double>(_buses.at(name).GetRoute())});
	answer.insert({"curvature", _buses.at(name).GetCurvature() });
	answer.insert({"stop_count", static_cast<double>(_buses.at(name).CountStops()) });
	answer.insert({"unique_stop_count", static_cast<double>(_buses.at(name).CountUniqueStops()) });

	return answer;
}

Node Manager::ProcessRoute(const NodesMap& nmap)
{
#ifdef TEST_PROFILE
	LOG_DURATION("ProcessRoute");
#endif
	NodesMap answer;
	answer.insert({ "request_id", nmap.at("id") });

	auto from_str = nmap.at("from").AsString();
	auto to_str = nmap.at("to").AsString();
	auto from_i = _stops.at(from_str)->index;
	auto to_i = _stops.at(to_str)->index;

	auto route_opt = _pRouter->BuildRoute(from_i, to_i);

	if (!route_opt)
	{
		answer.insert({ "error_message", "not found" });
		return answer;
	}

	answer.insert({"total_time", route_opt.value().weight });
	vector<Node> route_items_n(route_opt.value().edge_count * 2);

	for (size_t i = 0, j = 0; i < route_opt.value().edge_count; ++i)
	{
		auto edge = _pGraph->GetEdge(_pRouter->GetRouteEdge(route_opt.value().id, i));

		NodesMap route_item_wait
		{
			{"type", Node(string("Wait"))},
			{"stop_name", Node(_stops_i.at(edge.from))},
			{"time", Node(static_cast<double>(_wait_time))}
		};

		NodesMap route_item_bus
		{
			{"type", Node(string("Bus"))},
			{"bus", Node(edge.edge_name)},
			{"span_count", Node(static_cast<double>(edge.vertex_count))},
			{"time", Node(edge.weight - _wait_time)}
		};

		route_items_n[j++] = move(route_item_wait);
		route_items_n[j++] = move(route_item_bus);
	}

	answer.insert({ "items", Node(move(route_items_n)) });

	_pRouter->ReleaseRoute(route_opt.value().id);

	return answer;
}