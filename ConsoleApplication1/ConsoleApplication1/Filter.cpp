#include "Header.h"
#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>

Filter::Filter() {

}

bool Filter::meets_the_request(Article & article) {
	stringstream ss;
	ss.str(article.title);
	stringstream ss2;
	ss2 << article.description;
	stringstream ss3;
	ss3.str(article.category);
	if ((contains_keys(ss) || contains_keys(ss2) || contains_keys(ss3)) && is_fresh(article.pubDate)){
		return true;
	}
	return false;
}

bool is_spase_or_punct(char c) {
	static const char characters[] = "\r\n\t !?.,/-=:;'\\&*%@#()[]{}\"";
	for (size_t i = 0; i < sizeof(characters); ++i) {
		if (c == characters[i]) {
			return true;
		}
	}
	return false;
}

string & trim(string &s) {
	size_t pos1 = 0;
	size_t pos2 = s.size() - 1;
	for (size_t i = 0; i < s.size(); ++i) {
		if (!is_spase_or_punct(s[i])) {
			pos1 = i;
			break;
		}
	}
	for (int i = s.size() - 1; i >= 0; --i) {
		if (!is_spase_or_punct(s[i])) {
			pos2 = i;
			break;
		}
	}
	s.erase(0, pos1 - 0);
	s.erase(pos2, s.size() - 1 - pos2);
	return s;
}

bool Filter::is_fresh(tm date) {
	errno_t err;
	time_t rawtime;
	tm current_time;
	time(&rawtime);
	if (err = gmtime_s(&current_time, &rawtime)) {
		cout << "Could not get local time " << endl;
	}
	else {
		time_t new_date = mktime(&date) + seconds;
		time_t t1 = mktime(&current_time);

		if (difftime(t1, new_date) <= 0) {
			return true;
		}
	}
	return false;
}

bool Filter::contains_keys(stringstream & ss) {
	string item;
	while (getline(ss, item, ' ')) {
		trim(item);
		for (auto it = negative_keys.begin(); it != negative_keys.end(); ++it) {
			if (item == *it) {
				return false;
			}
		}

		for (auto it = positive_keys.begin(); it != positive_keys.end(); ++it) {
			if (item == *it) {
				return true;
			}
		}

		if (positive_keys.size() == 0) {
			return true;
		}
	}

	return false;
}

