
#include "bus.h"
#include <stdexcept>
#include <cmath>

using namespace std;


vector<Graph::Edge<double>> Bus::AddStop(const Stop* const stop)
{
	_ustops.insert(stop);
	_stops.push_back(stop);

	if (_stops.size() < 2) return {};

	return CalcDists(_stops.size() - 1);
}

vector<Graph::Edge<double>> Bus::RepeatStops()
{
	if (_stops.size() < 2) return {};
	auto old_size = _stops.size();
	_stops.resize(_stops.size() * 2 - 1);

	auto result_size = 
		(((_stops.size() - 1) * _stops.size()) / 2) - 
		(((old_size - 1) * old_size) / 2);
	vector<Graph::Edge<double>> result(result_size);
	size_t result_i = 0;

	for (size_t i = old_size, j = 2; i < _stops.size(); ++i, j += 2)
	{
		_stops[i] = _stops[i - j];

		for (auto& edge : CalcDists(i))
		{
			result[result_i++] = move(edge);
		}
	}

	return result;
}

vector<Graph::Edge<double>> Bus::CalcDists(size_t last_index)
{
	_direct_d += CalcDirectDist(*_stops[last_index - 1], *_stops[last_index]);
	_route_d += CalcRouteDist(*_stops[last_index - 1], *_stops[last_index]);
	
	_routes_d.push_back(_route_d);

	vector<Graph::Edge<double>> result(last_index);

	for (size_t i = 0; i < last_index; ++i)
	{
		result[i].from = _stops[i]->index;
		result[i].to = _stops[last_index]->index;
		size_t w = 0;
		if (i == 0)
		{
			w += _routes_d.back();
		}
		else
		{
			w += _routes_d.back() - _routes_d[i - 1];
		}
		result[i].weight = w * 1.0 / _bus_velo + _wait_time;
		result[i].vertex_count = _routes_d.size() - i;
		result[i].edge_name = _name;
	}

	return result;
}

double Bus::CalcDirectDist(const Stop& lhs, const Stop& rhs)
{
	return acos(sin(lhs.rlat) * sin(rhs.rlat) +
		cos(lhs.rlat) * cos(rhs.rlat) *
		cos(abs(lhs.rlon - rhs.rlon))
	) * 6371000;
}

size_t Bus::CalcRouteDist(const Stop& from, const Stop& to)
{
	auto d = from.DistanceTo(to.name);

	if (d == nullopt)
	{
		throw invalid_argument("unknown distance to " + to.name);
	}

	return d.value();
}