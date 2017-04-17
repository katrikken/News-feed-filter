#include "Header.h"
#include <string>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <iostream>

/** This method passes data about articles to other filter methods. 
  * Returns true if the article meets the request. */
bool Filter::meets_the_request(Article & article) {
	stringstream ss;
	ss << article.title << article.description << article.category;

	if (contains_keys(ss) && is_fresh(article.pubDate)){
		return true;
	}
	return false;
}

/** This method checks if a character is a white space or punctuation. */
bool is_spase_or_punct(char &c) {
	static const char characters[] = "\r\n\t !?.,/-=:;'\\&*%@#()[]{}\"";
	for (size_t i = 0; i < sizeof(characters); ++i) {
		if (c == characters[i]) {
			return true;
		}
	}
	return false;
}

/** This method trims the string from white spaces and punctuation. */
string & trim(string &s) {
	size_t pos1 = 0;
	size_t pos2 = 0;
	for (size_t i = 0; i < s.size(); ++i) {
		if (!is_spase_or_punct(s[i])) {
			pos1 = i;
			break;
		}
	}
	for (int i = s.size() - 1; i >= 0; --i) {
		if (!is_spase_or_punct(s[i])) {
			pos2 = i+1;
			break;
		}
	}
	s.erase(pos2, s.size() - pos2);
	if (s.size() >= pos1) {
		s.erase(0, pos1);
	}

	return s;
}

/** This method checks if a given article date is within time interval between now and user defined time ago. */
bool Filter::is_fresh(tm &date) {
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

/** This method checks whether article title, category or description contain desired or undesired keywords. 
  * Returns true if article doesn't contain any negative keyword AND does contain positive keyword if defined. */
bool Filter::contains_keys(stringstream & ss) {
	string item;
	while (getline(ss, item, ' ')) {
		trim(item);
		for (auto && key : negative_keys) {
			if (item == key) {
				return false;
			}
		}

		for (auto && key : positive_keys) {
			if (item == key) {
				return true;
			}
		}
	}

	if (positive_keys.size() == 0) {
		return true;
	}

	return false;
}

/** This method writes information about filter settings to the standard output.*/
void Filter::write_filter_info() {
	int time = seconds / 60;
	if (time > 59) {
		time = time / 60;
		cout << "Looking for articles added within last " << time << " hours" << endl;
	}
	else {
		cout << "Looking for articles added within last " << time << " minutes";
	}
	
	if (positive_keys.size() == 0 && negative_keys.size() == 0) {
		cout << "No filter will be applied." << endl;
	}
	else {
		if (negative_keys.size() != 0) {
			cout << "Articles will not contain:" << endl;
			for (auto && key : negative_keys) {
				cout << key << " ";
			}
			cout << endl;
		}
		if (positive_keys.size() != 0) {
			cout << "Articles will contain:" << endl;
			for (auto && key : positive_keys) {
				cout << key << " ";
			}
			cout << endl;
		}
	}
}

