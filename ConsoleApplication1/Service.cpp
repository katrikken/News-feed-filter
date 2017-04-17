#include "Header.h"
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

/** Constructor.*/
Service::Service(Parser & parser_, Filter & filter_) : parser(parser_), filter(filter_) {}

/** This method writes articles to the output stream.*/
void Service::write_output(vector<Article>& articles, ostream & os) {
	bool empty_output = true;

	for (auto && article : articles) {
		if (filter.meets_the_request(article)) {
			empty_output = false;
			os << article.get_title() << endl << article.get_link() << endl << endl;
		}
	}
	
	if (empty_output) {
		os << "No article matches request parameters. Check if RSS link and key words are correct." << endl;
	}
}

/** This method calls for the Parser and Filter methods and performs output of the articles. */
void Service::prepare_articles() {
	vector<Article> articles = parser.parse_input();
	if (output.size() > 0) {
		ofstream output_stream(output);
		if (output_stream.is_open())
		{
			write_output(articles, output_stream);

			output_stream.close();
		}
		else {
			cout << "Could not open file " << output << endl;
			throw(1111);
		}
	}
	else {
		write_output(articles, cout);
	}
}

/** This method writes information about recieved task to the standard output.*/
void Service::write_task_info() {
	cout << "Downloading articles from:" << endl;
	for (auto && link : parser.rss_links) {
		cout << link << endl;
	}

	filter.write_filter_info();

	if (output.size() != 0) {
		cout << "To the output file " << output << endl;
	}

	cout << endl;
}

/** This method runs algorithm in a loop with a given interval. */
void Service::start() {
	write_task_info();

	try {
		if (interval > 0) {
			while (true) {
				prepare_articles();

				errno_t err;
				time_t rawtime;
				time(&rawtime);
				time_t new_date = rawtime + interval / 1000;
				
				tm new_time;
				if (err = localtime_s(&new_time, &new_date)) {
					cout << "Could not count time interval " << endl;
				}
				else {
					char buff[20];
					strftime(buff, 20, "%Y-%m-%d %H:%M:%S", &new_time);
					cout << "Program is waiting until " << buff << " to update news feed." << endl << endl;
					parser.articles.clear();
				}		
				
				this_thread::sleep_for(chrono::milliseconds(interval));
			}
		}
		else {
			prepare_articles();
			cout << "Program has successfully ended." << endl;
		}
	}
	catch (int e) {
		if (e != 1111) {
			cout << "Unknown exception occured. " << e << endl;
		}
		cout << "Program stopped." << endl;
	}
}
