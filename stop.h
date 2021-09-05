#pragma once

#include <string>
#include <set>
#include <map>
#include <optional>

class Stop
{
public:

	Stop(const std::string& n, double lat, double lon, size_t idx) : name(n), index(idx)
	{
		rlat = (lat * 3.1415926535) / 180;
		rlon = (lon * 3.1415926535) / 180;
	}

	const std::optional<size_t> DistanceTo(const std::string& to) const;

	void AddDistance(const std::string& to, size_t dist) { _dists[to] = dist; }
	size_t CountBuses() const { return _bus_names.size(); }
	const std::set<std::string>& GetBuses() const { return _bus_names; }
	void AddBus(const std::string& bus_name) { _bus_names.insert(bus_name); }


	std::string name;
	double rlat = 0.0;
	double rlon = 0.0;
	size_t index = 0;

protected:

	std::set<std::string> _bus_names;
	std::map<std::string, size_t> _dists;
};