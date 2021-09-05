
#include "stop.h"

using namespace std;

const optional<size_t> Stop::DistanceTo(const string& to) const
{
	if (!_dists.count(to)) return nullopt;
	return _dists.at(to);
}