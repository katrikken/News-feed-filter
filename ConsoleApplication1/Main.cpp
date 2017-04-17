#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "Header.h"
#include <thread>
#include <functional>

using namespace std;

string link_p = "-l";
string link_file_p = "-L";
string keywords_p = "-k"; //for positive keys
string keywords_file_p = "-K";
string output_p = "-o";
string time_period_p = "-p";
string repeatition_p = "-r";

/**
 * This method splits string by a given delimeter and stores the result to a referenced vector.
 */
void split(const std::string &s, char delim, vector<string> & result) {
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim)) {
		result.push_back(item);
	}
	if (!ss.eof()) {
		getline(ss, item);
		result.push_back(item);
	}
}

/** This method parses time argument formatted as [0-9][0-9]*[hm] to the amount of seconds. */
long get_seconds(string & time) {
	long seconds = 0;
	if (time[time.size() - 1] == 'm') {
		seconds = stoi(time) * 60;
	}
	else if (time[time.size() - 1] == 'h') {
		seconds = stoi(time) * 60 * 60;
	}
	return seconds;
}

/** This method writes short user documentation to the standard output.*/
void write_usage_info() {
	cout << "Usage: NewsFeed.exe [-options]" << endl;
	cout << "where options are:" << endl;
	cout << link_p <<"<link>	set RSS link <link>" << endl;
	cout << link_file_p << "<file name>	pass name of the file containing RSS links separated by new lines" << endl;
	cout << keywords_p << "<keyword1(,keyword2,keyword3)>	set keywords for searching in the articles" << endl;
	cout << keywords_file_p << "<file name>	pass name of the file containing keywords separated by new line" << endl;
	cout << "	Keywords file format: " << endl;
	cout << "		No: keyword1 keyword2" << endl;
	cout << "		Yes: keyword1 keyword2" << endl;

	cout << output_p << "<file name>	pass name of the file to write output into. If missing, output will be written to the console." << endl;
	cout << time_period_p << "<time>	set interval of time where publishing date should fall into." << endl;
	cout << "	Time format is [number][h/m] where h stands for hours, m for minutes. Example: 12h" << endl;

	cout << repeatition_p << "<time>	set interval for updating the news feed. See time format above." << endl;
}

int main(int argc, char *argv[])
{
	try {
		Parser parser;
		Filter filter;
		Service service(parser, filter);

		if (argc == 1) {
			write_usage_info();
		}
		else {
			for (int i = 1; i < argc; i++) {
				string str(argv[i]);

				if (str.find(link_p) == 0) {
					parser.rss_links.push_back(str.substr(link_p.size()));
				}

				else if (str.find(link_file_p) == 0) {
					string line;
					ifstream rss_file(str.substr(link_file_p.size()));
					if (rss_file.is_open())
					{
						while (getline(rss_file, line))
						{
							parser.rss_links.push_back(line);
						}
						rss_file.close();
					}
					else {
						cout << "Could not open file " << str.substr(link_file_p.size()) << endl;
					}
				}

				else if (str.find(keywords_p) == 0) {
					split(str.substr(keywords_p.size()), ',', filter.positive_keys);
				}

				else if (str.find(keywords_file_p) == 0) {
					ifstream key_file(str.substr(keywords_file_p.size()));
					string line;
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
								cout << "Could not identify keywords on the line " << line << endl;
							}
						}
						key_file.close();
					}
					else {
						cout << "Could not open file " << str.substr(keywords_file_p.size()) << endl;
					}
				}

				else if (str.find(output_p) == 0) {
					service.output = str.substr(output_p.size());
				}

				else if (str.find(time_period_p) == 0) {
					filter.seconds = get_seconds(str.substr(time_period_p.size()));
				}

				else if (str.find(repeatition_p) == 0) {
					long seconds = get_seconds(str.substr(repeatition_p.size()));
					service.interval =  seconds * 1000;

					if (seconds != 0 && service.interval / seconds != 1000) {
						//overflow handling
						cout << "Time interval for repeated updating is too long." << endl;
						service.interval = 0;
					}		
				}

				else {
					cout << "Parameter is illegal: " << argv[i] << endl;
				}
			}
		
			if (parser.rss_links.size() > 0 && filter.seconds > 0) {
				service.filter = filter;
				service.parser = parser;

				service.start();
			}
			else {
				cout << "Missing parameters." << endl;
				if (parser.rss_links.size() == 0) {			
					cout << "Check correctness of RSS URLs in " << link_p << " or " << link_file_p << endl;
				}
				if (filter.seconds <= 0) {
					cout << "Check correctness of time parameter " << time_period_p << endl;
				}
				write_usage_info();
			}
		}
	}
	catch (int e) {
		cout << "Something went wrong: " << e << endl;
	}
	
	return 0;
}