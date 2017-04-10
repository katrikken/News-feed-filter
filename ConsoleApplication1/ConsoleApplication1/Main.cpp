#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "Header.h"
#include <thread>
#include <functional>

using namespace std;

void split(const std::string &s, char delim, vector<string> & result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		result.push_back(item);
	}
}

long get_seconds(string time) {
	long seconds = 0;
	if (time[time.size() - 1] == 'm') {
		seconds = stoi(time) * 60;
	}
	else if (time[time.size() - 1] == 'h') {
		seconds = stoi(time) * 60 * 60;
	}
	return seconds;
}

int main(int argc, char *argv[])
{
	try {
		if (argc == 6) {
			Parser parser;

			string line;
			ifstream rss_file(argv[1]);
			if (rss_file.is_open())
			{
				while (getline(rss_file, line))
				{
					parser.rss_links.push_back(line);
				}
				rss_file.close();
			}
			else {
				cout << "Could not open file " << argv[1] << endl;
			}

			Filter filter;

			ifstream key_file(argv[2]);
			if (key_file.is_open())
			{
				bool yes_words = false;
				string yes = "Yes: ";
				string no = "No: ";
				while (getline(key_file, line, '\n'))
				{
					if (strncmp(line.c_str(), yes.c_str(), yes.size()) == 0) {
						split(line.substr(yes.size()), ' ', filter.positive_keys);
					}
					else if (strncmp(line.c_str(), no.c_str(), no.size()) == 0) {
						split(line.substr(no.size()), ' ', filter.negative_keys);
					}
					else {
						cout << "Could not identify keywords." << endl;
					}
				}
				key_file.close();
			}
			else {
				cout << "Could not open file " << argv[2] << endl;
			}

			filter.seconds = get_seconds(argv[5]);

			thread tr1(&Service::start, Service(parser, filter, argv[3], get_seconds(argv[4])));
			tr1.join();
		}
		else {
			cout << "Wrong number of arguments!" << endl;
		}
	}
	catch (int e) {
		cout << "Something went wrong: " << e << endl;
	}
	
	return 0;
}