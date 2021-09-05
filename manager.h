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
