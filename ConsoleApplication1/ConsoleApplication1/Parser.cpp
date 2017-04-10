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

static const char *filename = "temp";

vector<Article> & Parser::parse_input() {
	for (auto it = rss_links.begin(); it != rss_links.end(); ++it) {
		bool status = fetch_web_page(*it);
		if (status) {
			resolve_DOM();
		}
	}
	remove(filename);
	return articles;
}

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	int written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

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
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
			status = false;
		}
		fclose(xml_file);
	}
	curl_easy_cleanup(curl);

	curl_global_cleanup();

	return status;
}

void Parser::resolve_DOM() {
	TiXmlDocument doc(filename);
	bool loadOkay = doc.LoadFile();
	if (loadOkay) {
		try {
			TiXmlElement* root = doc.FirstChildElement("rss")->FirstChildElement("channel");

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
			}
		}
		catch (int e) {
			cout << "Error while parsing XML " << e << endl;
		}
	}
}