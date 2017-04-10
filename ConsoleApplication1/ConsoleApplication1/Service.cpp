#include "Header.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

Service::Service(Parser& parser_, Filter& filter_, string output_file, long interval_) {
	parser = parser_;
	filter = filter_;
	output = output_file;
	interval = interval_ * 1000;
}

void Service::prepare_articles() {
	vector<Article> articles = parser.parse_input();
	ofstream output_stream(output);
	if (output_stream.is_open())
	{
		for (auto it = articles.begin(); it != articles.end(); ++it) {
			if (filter.meets_the_request(*it)) {
				output_stream << it->get_title() << it->get_link() << endl; 
			}
		}

		output_stream.close();
	}
	else {
		cout << "Could not open file " << output << endl;
	}
}

void Service::start() {
	while (true) {
		prepare_articles();
		this_thread::sleep_for(chrono::milliseconds(interval));
	}
}
