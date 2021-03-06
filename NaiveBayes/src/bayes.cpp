#include <iostream>
#include <list>
#include <map>
#include <set>
#include <cstring>

#include "bayes.h"

using namespace std;

map<Data, map<string, double> > bayes(string label, Data target, list<Data> dataSet)
{
	map<string, double> ret;
	set<string> kinds;
	for (auto data : dataSet)
	{
		kinds.insert(data.label);
	}
	
	for (auto data : dataSet)
	{
		map<string, double> temp;
		for (auto kind : kinds)
		{
			double count = 0.0;
			for (auto feature : data.features)
			{
				if (data.features.find(feature.first) != data.features.end())
				{
					temp[kind] += feature.second;
				}
				count += feature.second;
			}
			temp[kind] /= count;
		}
		ret[data] = temp;
	}
	return ret;
}











