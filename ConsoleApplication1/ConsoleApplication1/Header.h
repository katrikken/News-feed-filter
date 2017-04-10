#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include "tinyxml.h"

using namespace std;

class Article {
public:
	string title;
	string link;
	TiXmlElement* description;
	tm pubDate;
	string category;

	Article();
	string get_title();
	string get_link();
};

class Parser {
private:
	vector<Article> articles;

	bool fetch_web_page(const string& page);
	void resolve_DOM();
	
public:
	vector<string> rss_links;
	vector<Article> & parse_input();	
};

class Filter {
	
public:
	vector<string> positive_keys;
	vector<string> negative_keys;
	long seconds;

	Filter();

	bool meets_the_request(Article & article);
	bool contains_keys(stringstream & ss);
	bool is_fresh(tm date);
};

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