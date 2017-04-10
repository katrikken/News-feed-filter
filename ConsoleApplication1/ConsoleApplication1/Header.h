#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include "tinyxml.h"

using namespace std;

/** This class represents articles. */
class Article {
public:
	string title;
	string link;
	TiXmlElement* description;
	tm pubDate;
	string category;

	string get_title();
	string get_link();
};

/** This class is for downloading the web pages and parsing articles from them. */
class Parser {
private:
	vector<Article> articles;

	bool fetch_web_page(const string& page);
	void resolve_DOM();
	
public:
	vector<string> rss_links;
	vector<Article> & parse_input();	
};

/** This class contains information which articles are filtred by and methods to check if articles pass the filter. */
class Filter {
	
public:
	vector<string> positive_keys;
	vector<string> negative_keys;
	long seconds;

	bool meets_the_request(Article & article);
	bool contains_keys(stringstream & ss);
	bool is_fresh(tm date);
};

/** This class is a controller for the interaction between parsing, filtering and outputting part of the program. */
class Service {
private:
	void prepare_articles();
public:
	Parser parser;
	Filter filter;
	string output;
	long interval;

	Service(Parser& parser_, Filter& filter_, string output_, long interval_);
	void start();
};