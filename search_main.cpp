#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <sstream>
#include <map>
#include <time.h>
#include <iomanip>
#include <queue>
#include <stack>
using namespace std;

struct City {
	string cityName;
	double longitude;
	double latitude;
};


int getCityIndex(vector<City>& cityList, string& cityName) {
	for (int i = 0; i < cityList.size(); ++i) {
		if (cityList[i].cityName == cityName) {
			return i;
		}
	}
	return -1;
}

double CheckDistance(double city1long, double city1lat, double city2long, double city2lat) {
	double distance;
	distance = sqrt(pow((city2long - city1long), 2) + pow((city2lat - city1lat), 2));
	return distance;
}

void AdjacentList(vector<vector<string>>& adjTowns) {
	ifstream infile;
	string line;
	vector<string> tempList;


	infile.open("Adjacencies.txt");

	if (infile.is_open()) {
		while (getline(infile, line, '\n')) {
			stringstream ss(line);
			string word;
			while (getline(ss, word, ' ')) {
				tempList.push_back(word);
			}
			adjTowns.push_back(tempList);
			tempList.clear();

		}
		infile.close();
	}
}

map<string, vector<string>> AdjacencyMap(vector<vector<string>> adjTowns) {
	map<string, vector<string>> adjacencyMap;

	for (const vector<string>& towns : adjTowns) {
		if (towns.size() >= 2) {
			const string& town1 = towns[0];
			const string& town2 = towns[1];

			adjacencyMap[town1].push_back(town2);

			adjacencyMap[town2].push_back(town1);
		}
	}

	return adjacencyMap;
}

void CityList(vector<City>& kansasTowns) {
	ifstream infile;
	vector<string> temp;
	string line;
	City currentTown;

	infile.open("coordinates.csv");

	if (infile.is_open()) {
		while (getline(infile, line, '\n')) {
			stringstream ss(line);
			string word;
			while (getline(ss, word, ',')) {
				temp.push_back(word);
			}

			currentTown.cityName = temp.at(0);
			currentTown.longitude = stod(temp.at(1));
			currentTown.latitude = stod(temp.at(2));
			kansasTowns.push_back(currentTown);
			temp.clear();
		}
		infile.close();
	}
}

bool CheckWord(string searchWord, vector<City> kansasTowns) {
	for (int i = 0; i < kansasTowns.size(); ++i) {
		if (kansasTowns[i].cityName == searchWord) {
			return true;
		}
	}
	return false;
}

vector<string> aStarSearch(vector<City>& cityList, const map<string, vector<string>>& adjTowns, string start, string end) {
	priority_queue<pair<double, vector<string>>, vector<pair<double, vector<string>>>, greater<pair<double, vector<string>>>> pq;
	map<string, bool> visited;
	map<string, double> gScore;
	map<string, double> fScore;

	pq.push({ 0, {start} });
	gScore[start] = 0;
	fScore[start] = CheckDistance(cityList.at(getCityIndex(cityList, start)).longitude, cityList.at(getCityIndex(cityList, start)).latitude,
		cityList.at(getCityIndex(cityList, end)).longitude, cityList.at(getCityIndex(cityList, end)).latitude);

	while (!pq.empty()) {
		pair<double, vector<string>> node = pq.top();
		pq.pop();
		vector<string> path = node.second;
		double cost = node.first;
		string currentCity = path.back();

		if (currentCity == end) {
			return path;
		}

		for (string neighbor : adjTowns.at(currentCity)) {
			double tentativeGScore = gScore[currentCity] + CheckDistance(cityList.at(getCityIndex(cityList, currentCity)).longitude,
				cityList.at(getCityIndex(cityList, currentCity)).latitude,
				cityList.at(getCityIndex(cityList, neighbor)).longitude,
				cityList.at(getCityIndex(cityList, neighbor)).latitude);

			if (!gScore.count(neighbor) || tentativeGScore < gScore[neighbor]) {
				visited[neighbor] = true;
				gScore[neighbor] = tentativeGScore;
				fScore[neighbor] = gScore[neighbor] + CheckDistance(cityList.at(getCityIndex(cityList, neighbor)).longitude,
					cityList.at(getCityIndex(cityList, neighbor)).latitude,
					cityList.at(getCityIndex(cityList, end)).longitude,
					cityList.at(getCityIndex(cityList, end)).latitude);

				vector<string> newPath = path;
				newPath.push_back(neighbor);
				pq.push({ fScore[neighbor], newPath });
			}
		}
	}

	return {};
}

vector<string> breadthFirstSearch(vector<City> cityList, const map<string, vector<string>>& adjTowns, const string& start, const string& end) {
	queue<vector<string>> q;
	map<string, bool> visited;

	q.push({ start });
	visited[start] = true;

	while (!q.empty()) {
		vector<string> path = q.front();
		q.pop();
		string currentCity = path.back();

		if (currentCity == end) {
			return path;
		}

		for (const string& neighbor : adjTowns.at(currentCity)) {
			if (!visited[neighbor]) {
				visited[neighbor] = true;
				vector<string> newPath = path;
				newPath.push_back(neighbor);
				q.push(newPath);
			}
		}
	}

	return {};
}

void BruteForceSearch(vector<City>& kansasTowns, vector<vector<string>>& adjTowns, string& start, string& end) {

	string current;
	current = start;
	double x = 0.0;
	double y = 0.0;
	double total = 0.0;

	std::cout << "Going from " << start << " to " << end << ".\n";



	while (current != end) {
		string temp;
		double min = 300000.00;
		for (int i = 0; i < adjTowns.size(); i++) {
			if (adjTowns[i].front() == current) {
				for (int j = 0; j < kansasTowns.size(); j++) {
					if (kansasTowns[j].cityName == adjTowns[i].back()) {
						for (int k = 0; k < kansasTowns.size(); k++) {
							if (kansasTowns[k].cityName == current) {
								if (CheckDistance(kansasTowns[k].longitude, kansasTowns[k].latitude, kansasTowns[j].longitude, kansasTowns[j].latitude) < min) {
									min = CheckDistance(kansasTowns[k].longitude, kansasTowns[k].latitude, kansasTowns[j].longitude, kansasTowns[j].latitude);
									current = adjTowns[i].back();
									total += min;
									std::cout << "from " << kansasTowns[k].cityName << " to " << kansasTowns[j].cityName << " is a distance of " << min << " miles.\n";
								}
							}
						}
					}
				}
			}
		}

	}

	std::cout << "The fastest route is a total of " << total << " miles.\n";
}

vector<string> bestFirstSearch(vector<City> cityList, const map<string, vector<string>>& adjTowns, const string& start, const string& end) {
	priority_queue<pair<double, vector<string>>, vector<pair<double, vector<string>>>, greater<pair<double, vector<string>>>> pq;
	map<string, bool> visited;

	pq.push({ 0, {start} });
	visited[start] = true;

	while (!pq.empty()) {
		pair<double, vector<string>> node = pq.top();
		pq.pop();
		vector<string> path = node.second;
		double cost = node.first;
		string currentCity = path.back();

		if (currentCity == end) {
			return path;
		}

		for (string neighbor : adjTowns.at(currentCity)) {
			if (!visited[neighbor]) {
				visited[neighbor] = true;
				vector<string> newPath = path;
				newPath.push_back(neighbor);
				double newCost = cost + CheckDistance(cityList.at(getCityIndex(cityList, currentCity)).longitude, cityList.at(getCityIndex(cityList, currentCity)).latitude, cityList.at(getCityIndex(cityList, neighbor)).longitude, cityList.at(getCityIndex(cityList, neighbor)).latitude);
				pq.push({ newCost, newPath });
			}
		}
	}

	return {};
}

vector<string> depthFirstSearch(vector<City> cityList, const map<string, vector<string>>& adjTowns, const string& start, const string& end) {
	stack<vector<string>> s;
	map<string, bool> visited;

	s.push({ start });
	visited[start] = true;

	while (!s.empty()) {
		vector<string> path = s.top();
		s.pop();
		string currentCity = path.back();

		if (currentCity == end) {
			return path;
		}

		for (string neighbor : adjTowns.at(currentCity)) {
			if (!visited[neighbor]) {
				visited[neighbor] = true;
				vector<string> newPath = path;
				newPath.push_back(neighbor);
				s.push(newPath);
			}
		}
	}

	return {};
}

vector<string> depthLimitedDFS(vector<City> cityList, const map<string, vector<string>>& adjTowns, const string& start, const string& end, int depthLimit) {
	stack<vector<string>> s;
	map<string, bool> visited;

	s.push({ start });
	visited[start] = true;

	while (!s.empty()) {
		vector<string> path = s.top();
		s.pop();
		string currentCity = path.back();

		if (currentCity == end) {
			return path;
		}

		if (path.size() < depthLimit) {
			for (string neighbor : adjTowns.at(currentCity)) {
				if (!visited[neighbor]) {
					visited[neighbor] = true;
					vector<string> newPath = path;
					newPath.push_back(neighbor);
					s.push(newPath);
				}
			}
		}
	}

	return {};
}

vector<string> iterativeDeepeningDFS(vector<City> cityList, const map<string, vector<string>>& adjTowns, const string& start, const string& end, int maxDepth) {
	for (int depthLimit = 1; depthLimit <= maxDepth; ++depthLimit) {
		vector<string> path = depthLimitedDFS(cityList, adjTowns, start, end, depthLimit);
		if (!path.empty()) {
			return path;
		}
	}
	return {};
}

int main() {

	string startingTown;
	string endTown;
	int searchChoice;
	vector<City> townList;
	vector<vector<string>> adjTowns;
	map<string, vector<string>> adjMap;

	CityList(townList);
	AdjacentList(adjTowns);
	adjMap = AdjacencyMap(adjTowns);

	cout << "Hi, where are you starting? ";
	cin >> startingTown;
	while (CheckWord(startingTown, townList) != true) {
		cout << "That place is not in our database, please try again >> ";
		cin >> startingTown;

	}
	cout << "Where would you like to end up? ";
	cin >> endTown;
	while (CheckWord(endTown, townList) != true) {
		cout << "That place is not in our database, please try again >> ";
		cin >> endTown;

	}
	bool repeat = true;
	while (repeat) {
		cout << "\nPick how you would like to search >>\n1.undirected brute-force search\n2.breadth-first search\n3. depth-first search\n4. ID-DFS search\n5. best-first\n6. A* search\n";
		cout << "\nFrom the menu above, choose a number (1-6) or (7 to quit): ";
		cin >> searchChoice;

		if (searchChoice == 1) {
			time_t s, e;
			time(&s);
			BruteForceSearch(townList, adjTowns, startingTown, endTown);
			time(&e);
			double total_time = double(s - e);
			std::cout << "It took " << fixed << setprecision(5) << total_time;
			std::cout << " sec. to find your path.";
		}

		if (searchChoice == 2) {
			time_t s, e;
			time(&s);
			vector<string> path = breadthFirstSearch(townList, adjMap, startingTown, endTown);

			if (!path.empty()) {
				double total = 0;
				double x = 0;
				double y = 0;
				for (City town : townList) {
					if (town.cityName == startingTown) {
						x = town.longitude;
						y = town.latitude;
					}
				}
				cout << "Route found: ";
				for (const string& city : path) {
					cout << city << " -> ";
					for (City town : townList) {
						if (town.cityName == city) {
							total += CheckDistance(x, y, town.longitude, town.latitude);
							x = town.longitude;
							y = town.latitude;
						}
					}
				}
				cout << "Total Distance: " << total << " miles" << endl;
			}
			else {
				cout << "No route found." << endl;
			}
			time(&e);
			double total_time = double(s - e);
			std::cout << "It took " << fixed << setprecision(5) << total_time;
			std::cout << " sec. to find your path.";
		}

		if (searchChoice == 3) {
			time_t s, e;
			time(&s);
			vector<string> path = depthFirstSearch(townList, adjMap, startingTown, endTown);

			if (!path.empty()) {
				double total = 0;
				double x = 0;
				double y = 0;
				for (City town : townList) {
					if (town.cityName == startingTown) {
						x = town.longitude;
						y = town.latitude;
					}
				}
				cout << "Route found: ";
				for (const string& city : path) {
					cout << city << " -> ";
					for (City town : townList) {
						if (town.cityName == city) {
							total += CheckDistance(x, y, town.longitude, town.latitude);
							x = town.longitude;
							y = town.latitude;
						}
					}
				}
				cout << "Total Distance: " << total << " miles" << endl;
			}
			else {
				cout << "No route found." << endl;
			}
			time(&e);
			double total_time = double(s - e);
			std::cout << "It took " <<fixed << setprecision(5) << total_time;
			std::cout << " sec. to find your path.";
		}

		if (searchChoice == 4) {
			time_t s, e;
			time(&s);
			vector<string> path = iterativeDeepeningDFS(townList, adjMap, startingTown, endTown, int(adjMap.size()));

			if (!path.empty()) {
				double total = 0;
				double x = 0;
				double y = 0;
				for (City town: townList) {
					if (town.cityName == startingTown) {
						x = town.longitude;
						y = town.latitude;
					}
				}
				cout << "Route found: ";
				for (string city : path) {
					cout << city << " -> ";
					for (City town : townList) {
						if (town.cityName == city) {
							total += CheckDistance(x, y, town.longitude, town.latitude);
							x = town.longitude;
							y = town.latitude;
						}
					}
				}
				cout << "Total Distance: " << total << " miles" << endl;
			}
			else {
				cout << "No route found." << endl;
			}
			time(&e);
			double total_time = double(s - e);
			std::cout << "It took " << fixed << setprecision(5) << total_time;
			std::cout << " sec. to find your path.";

		}

		if (searchChoice == 5) {
			time_t s, e;
			time(&s);
			vector<string> path = bestFirstSearch(townList, adjMap, startingTown, endTown);

			if (!path.empty()) {
				double total = 0;
				double x = 0;
				double y = 0;
				for (City town : townList) {
					if (town.cityName == startingTown) {
						x = town.longitude;
						y = town.latitude;
					}
				}
				cout << "Route found: ";
				for (const string& city : path) {
					cout << city << " -> ";
					for (City town : townList) {
						if (town.cityName == city) {
							total += CheckDistance(x, y, town.longitude, town.latitude);
							x = town.longitude;
							y = town.latitude;
						}
					}
				}
				cout << "Total Distance: " << total << " miles" << endl;
			}
			else {
				cout << "No route found." << endl;
			}
			time(&e);
			double total_time = double(s - e);
			std::cout << "It took " << fixed << setprecision(5) << total_time;
			std::cout << " sec. to find your path.";
		}

		if (searchChoice == 6) {
			time_t s, e;
			time(&s);
			vector<string> path = aStarSearch(townList, adjMap, startingTown, endTown);


			if (!path.empty()) {
				double total = 0;
				double x = 0;
				double y = 0;
				for (City town : townList) {
					if (town.cityName == startingTown) {
						x = town.longitude;
						y = town.latitude;
					}
				}
				cout << "Route found: ";
				for (const string& city : path) {
					cout << city << " -> ";
					for (City town : townList) {
						if (town.cityName == city) {
							total += CheckDistance(x, y, town.longitude, town.latitude);
							x = town.longitude;
							y = town.latitude;
						}
					}
				}
				cout << "Total Distance: " << total << " miles" << endl;
			}
			else {
				cout << "No route found." << endl;
			}
			time(&e);
			double total_time = double(s - e);
			std::cout << "It took " << fixed << setprecision(5) << total_time;
			std::cout << " sec. to find your path.";

		}


		if (searchChoice == 7) {
			repeat = false;
		}
	}

}