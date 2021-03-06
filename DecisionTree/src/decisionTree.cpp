#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include "../lib/json/json.h"
#include "decisionTree.h"

using std::cout;
using std::endl;
using std::list;
using std::map;
using std::log;
using std::set;
using std::string;
using std::stack;

double entropy(list<Data> dataSet)
{
	// 1. Calculate probability
	double total = dataSet.size();
	//cout << total << endl;
	map<string, unsigned int > stat;
	for (auto iter : dataSet)
	{
		stat[iter.label]++;
	}
	// 2. Calculate entropy
	double entro = 0.0;
	for (auto iter : stat)
	{
		double prob = iter.second / total;
		entro -= prob * log2(prob);
	}
	return entro;
}

double log2(double n)
{
	return log(n)/log(2);
}

list<Data> split(list<Data> dataSet, string axis, string value)
{
	list<Data> retDataSet;
	for (auto iter : dataSet)
	{
		if (iter.features[axis] == value)
		{
			retDataSet.push_back(iter);
		}
	}
	return retDataSet;
}

string bestAxis(list<Data> dataSet)
{
	int numFeatures = dataSet.front().features.size();
	double originalEntropy = entropy(dataSet);
	if (numFeatures == 1 && originalEntropy != 0)
		return dataSet.front().features.begin()->first;
	double bestIncrease = 0.0;
	string bestAxis;

	set<string> features;
	for (auto iter : dataSet.front().features)
	{
		features.insert(iter.first);
	}

	map< string, set< string > > possibleValues;

	for (auto data : dataSet)
	{
		for (auto feature : features)
		{
			possibleValues[feature].insert(data.features[feature]);
		}
	}

	for (auto feature : features)
	{
		//cout << feature << endl;
		double newEntropy = 0.0;
		for (auto value : possibleValues[feature])
		{
			newEntropy += entropy(split(dataSet, feature, value));
		}
		if (newEntropy - originalEntropy > bestIncrease)
		{
			bestIncrease = originalEntropy - newEntropy;
			bestAxis = feature;
		}
	}
	return bestAxis;
}

TreeNode createTree(list<Data> dataSet)
{
	string axis = bestAxis(dataSet);
	//cout << "bestAxis : " << axis << endl;
	TreeNode node;
	if (axis.empty())
	{
		node.axis = "";
		node.label = dataSet.front().label;
	}
	else
	{
		node.axis = axis;
		map< string, set< string > > possibleValues;
		for (auto data : dataSet)
		{
			possibleValues[axis].insert(data.features[axis]);
		}

		for (auto value : possibleValues[axis])
		{
			list<Data> temp = split(dataSet, axis, value);

			for (auto& iter : temp)
			{
				iter.features.erase(axis);
			}

			//cout << "Value : " << value << endl;
			node.children[value] = createTree(temp);
		}
		node.label = "";
	}
	return node;
}

void printTree(TreeNode root, int deepth, string value)
{
	cout << deepth <<  " |" << value << "|";
	for (int i = 0; i < deepth; i++)
	{
		cout << "------";
	}
	cout << "> ";
	if (root.axis.empty())
	{
		cout << "Label : " << root.label;
	}
	else
	{
		cout << "Axis : " << root.axis << endl;
		for (auto iter : root.children)
		{
			//cout << "Value : " << iter.first;
			printTree(iter.second, deepth + 1, iter.first);
		}
	}
	cout << endl;
}

void serialize(TreeNode root, int deepth, std::ofstream& fout)
{

	fout << "{";

	fout << "\"axis\" : \"" << root.axis << "\",";

	fout << "\"label\" : \"" << root.label << "\",";

	fout << "\"children\" : [";

	if (root.children.empty() == false)
	{
		int count = 1;

		for (auto iter : root.children)
		{
			if (count-- != 1)
				fout << ",";

			fout << "{";

			fout << "\"" << iter.first << "\" : ";
			serialize(iter.second , deepth + 3, fout);
			fout << "}";
		}
	}
	fout << "]";
	fout << "}";
}

TreeNode unserialize(char* filename)
{
	Json::Reader reader;
	Json::Value root;

	string JS;

	FILE* file = fopen(filename, "r");
	if (!file)
	{
		std::cerr << "Cannot open file" << filename << endl;
		return TreeNode();
	}
	char* buffer = NULL;
	std::size_t length;


	while(getline(&buffer, &length, file) != -1)
	{
		JS = buffer;
	}

	fclose(file);

	//free(buffer);
	if (reader.parse(JS, root))
	{	
		return unserializeHelper(root);
	}


	return TreeNode();
}

TreeNode unserializeHelper(Json::Value value)
{
	TreeNode ret;
	ret.axis = value["axis"].asString();
	ret.label = value["label"].asString();
	const Json::Value arrayObj = value["children"];

	for (unsigned int i = 0; i < arrayObj.size(); i++)
	{
		Json::Value::Members members = arrayObj[i].getMemberNames();
		ret.children[members[0]] = unserializeHelper(arrayObj[i][members[0]]);
	}
	return ret;
}






