#include "Header.h"
#include <vector>
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "tinyxml.h"
#include <locale>
#include <iomanip>
#include <time.h> 

using namespace std;

/** This is a name of file for temporary storage of a downloaded XML.*/
static const char *filename = "temp";

/** This method calls private Parser methods to fill articles attribute with instances of Article parsed from the XML.*/
vector<Article> & Parser::parse_input() {
	for (auto && link : rss_links) {
		bool status = fetch_web_page(link);
		if (status) {
			resolve_DOM(link);
		}
	}
	remove(filename);
	return articles;
}

/** This method serves the curlib purposes to write fetched data to the file. */
static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

/** This method downloads web page and stores it into a temprorary file. */
bool Parser::fetch_web_page(const string& page) {
	bool status = true;
	CURL *curl;
	CURLcode res;
	FILE *xml_file;
	
	errno_t err;

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, page.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

	if (err = fopen_s(&xml_file, filename, "w")) //"w" mode rewrites the file
	{
		curl_easy_cleanup(curl);
		cout << "Could not save web page " << page << endl;
		status = false;
	}
	else 
	{
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, xml_file);
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			cout << "Failed to download web page: " << page << endl;
			throw(1111);
		}
		fclose(xml_file);
	}
	curl_easy_cleanup(curl);

	curl_global_cleanup();

	return status;
}

/** This method parses the XML from a temporary file, from which creates and stores instances of the Article class. */
void Parser::resolve_DOM(const string& page) {
	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();
	if (loadOkay) {
		try {
			TiXmlElement* root = doc.FirstChildElement("rss")->FirstChildElement("channel");
			bool found_article = false;
			for (TiXmlElement* e = root->FirstChildElement("item"); e != NULL; e = e->NextSiblingElement("item"))
			{		
					Article article;
					//These three elements must exist in RSS feed. Otherwise it is not valid.
					article.title = e->FirstChildElement("title")->GetText();
					article.link = e->FirstChildElement("link")->GetText();
					article.description = e->FirstChildElement("description");

					TiXmlElement* category = e->FirstChildElement("category");
					if (category) { article.category = category->GetText(); }
					else { article.category = "No";  }

					TiXmlElement* pubDate = e->FirstChildElement("pubDate");
					if (pubDate) { 
						tm t = {};
						istringstream ts(pubDate->GetText());
						//Sun, 19 May 2002 15:21:36 GMT format
						ts >> get_time(&t, "%a, %d %b %Y %H:%M:%S %Z");
						article.pubDate = t;
					}
					else { 
						errno_t err;
						time_t rawtime;
						tm timeinfo;
						time(&rawtime);
						if (err = gmtime_s(&timeinfo, &rawtime)) {
							cout << "Could not get local time " << e << endl;
						}
						else {
							article.pubDate = timeinfo;
						}					
					}

					articles.push_back(article);		
					found_article = true;
			}
			if (!found_article) {
				cout << "Didn't find any article on the provided web address: " << page << endl;
				throw(1111);
			}
		}
		catch (int e) {
			cout << "Error while parsing XML " << e << endl;
			throw(1111);
		}
	}
}