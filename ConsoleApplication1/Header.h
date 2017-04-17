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

	string& get_title();
	string& get_link();
};

/** This class is for downloading the web pages and parsing articles from them. */
class Parser {
private:
	bool fetch_web_page(const string& page);
	void resolve_DOM(const string& page);
	
public:
	vector<Article> articles;
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
	bool is_fresh(tm & date);
	void write_filter_info();
};

/** This class is a controller for the interaction between parsing, filtering and outputting part of the program. */
class Service {
private:
	void prepare_articles();
	void write_task_info();
	void write_output(vector<Article>& articles, ostream & os);
public:
	Parser & parser;
	Filter & filter;
	string output;
	long interval;

	void start();
	Service(Parser & parser_, Filter & filter_);
};