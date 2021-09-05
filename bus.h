#pragma once

#include "stop.h"
#include "graph.h"

#include <vector>
#include <string>
#include <unordered_set>
#include <optional>

class Bus
{
public:

	Bus(const std::string& name, double bus_velo, size_t wait_time) : 
		_name(name), _bus_velo(bus_velo), _wait_time(wait_time) {}

	size_t GetRoute() const { return _route_d; }
	double GetCurvature() const { return _route_d * 1.0 / _direct_d; }
	size_t CountStops() const { return _stops.size(); }
	size_t CountUniqueStops() const { return _ustops.size(); }

	std::vector<Graph::Edge<double>> AddStop(const Stop* const stop);
	std::vector<Graph::Edge<double>> RepeatStops();

	

protected:

	size_t CalcRouteDist(const Stop& from, const Stop& to);
	double CalcDirectDist(const Stop& lhs, const Stop& rhs);
	std::vector<Graph::Edge<double>> CalcDists(size_t last_index);


	std::string _name;
	std::vector<const Stop*> _stops;
	std::unordered_set<const Stop*> _ustops;
	double _direct_d = 0;
	size_t _route_d = 0;
	double _bus_velo = 0.0;
	size_t _wait_time = 0;
	std::vector<size_t> _routes_d;
};