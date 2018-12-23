//
//  TaskerUpgrade.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#include "TaskerBase.hpp"

namespace tasker {

	bool TaskerBase::checkValidUserName(const std::string& name) {
		if (name.length() < TASKER_NAME_MIN_LEN) { return false; } /* name must be atleast * chars long */
		if (std::find(RESERVE_USER_NAMES.begin(), RESERVE_USER_NAMES.end(), name) != RESERVE_USER_NAMES.end()) {
			return false; /* reserved name used */
		}
		return true;
	}
	bool TaskerBase::checkValidTagName(const std::string& name) {
		//Early validate:
		if (name.length() < TASKER_NAME_MIN_LEN) { return false; } /* tag name must be atleast * chars long */
		if (std::find(RESERVE_TAG_NAMES.begin(), RESERVE_TAG_NAMES.end(), name) != RESERVE_TAG_NAMES.end()) {
			return false; /* reserved tag name used */
		}
		return true;
	}
	bool TaskerBase::onlyDigits(const std::string str) {
		if (str == "") return false;
		return str.find_first_not_of("0123456789") == std::string::npos;
	}

	//Time related:
	std::string TaskerBase::getcurdatetime() {
		return this->getcurdatetime("%d-%m-%Y %H:%M:%S");
	}
	std::string TaskerBase::getcurdatetime(const std::string& format) {
		std::array<char, 64> buffer;
		buffer.fill(0);
		std::time_t rawtime;
		struct tm timeinfo;
		time(&rawtime);

#ifdef PLATLINUX
		localtime_r(&rawtime, &timeinfo);
#else
		localtime_s(&timeinfo, &rawtime);
#endif

		std::strftime(buffer.data(), sizeof(buffer), format.c_str(), &timeinfo);
		return std::string(buffer.data());
	}
	int TaskerBase::parseDateParts(
		const std::string& datestr,
		int& day, int& month, int& year, int& hours, int& minutes, int& seconds
	) {
		return this->parseDateParts(datestr, "%2d-%2d-%4d %2d:%2d:%2d", day, month, year, hours, minutes, seconds);
	}
	int TaskerBase::parseDateParts(
		const std::string& datestr,
		const std::string& format, int& day, int& month, int& year, int& hours, int& minutes, int& seconds
	) {
		int  _month, _day, _year, _hours, _minutes, _seconds;
		int found = 0;
		found = sscanf(datestr.c_str(), format.c_str(), &_day, &_month, &_year, &_hours, &_minutes, &_seconds);
		day = _day;
		month = _month;
		year = _year;
		hours = _hours;
		minutes = _minutes;
		seconds = _seconds;
		return found;
	}
	std::string TaskerBase::createDateFromInts(int day, int mon, int year, int hour, int min, int sec)
	{
		std::string s_day = day < 10 ? "0" : "";
		std::string s_mon = mon < 10 ? "0" : "";
		std::string s_hour = hour < 10 ? "0" : "";
		std::string s_min = min < 10 ? "0" : "";
		std::string s_sec = sec < 10 ? "0" : "";
		std::stringstream ss;
		//put arbitrary formatted data into the stream
		ss
			<< s_day << day << "-"
			<< s_mon << mon << "-"
			<< year << " "
			<< s_hour << hour << ":"
			<< s_min << min << ":"
			<< s_sec << sec
			;
		//convert the stream buffer into a string
		return ss.str();
	}
	std::time_t TaskerBase::getEpochTime(const std::wstring& dateTime)
	{
		// Let's consider we are getting all the input in
		// this format: '2014-07-25T20:17:22Z' (T denotes
		// start of Time part, Z denotes UTC zone).
		// A better approach would be to pass in the format as well.
		static const std::wstring dateTimeFormat{ L"%d-%m-%Y %H:%M:%S" };

		// Create a stream which we will use to parse the string,
		// which we provide to constructor of stream to fill the buffer.
		std::wistringstream ss{ dateTime };

		// Create a tm object to store the parsed date and time.
		std::tm dt;

		// Now we read from buffer using get_time manipulator
		// and formatting the input appropriately.
		ss >> std::get_time(&dt, dateTimeFormat.c_str());

		// Convert the tm structure to time_t value and return.
		return std::mktime(&dt);
	}

	//Get Console color Values:
	std::string TaskerBase::getcolor(const std::string& which)
	{
		return this->getcolor(which, 0, "");
	}
	std::string TaskerBase::getcolor(const std::string& which, const std::string& svalue)
	{
		return this->getcolor(which, 0, svalue);
	}
	std::string TaskerBase::getcolor(const std::string& which, float fvalue)
	{
		return this->getcolor(which, fvalue, "");
	}
	std::string TaskerBase::getcolor(const std::string& which, float value, const std::string& svalue)
	{
		if (!this->opt.use_colors) return "";
		if (which == "reset") {
			return TASKER_COLOR_RESET;
		}
		if (which == "status") {
			return (value > 0.99 ? TASKER_COLOR_GREEN : TASKER_COLOR_YELLOW);
		}
		if (which == "workbar") {
			if (value > 0.8) {
				return TASKER_COLOR_GREEN;
			}
			else if (value > 0.4) {
				return TASKER_COLOR_YELLOW;
			}
			else {
				return TASKER_COLOR_RED;
			}
		}
		if (which == "loadbar") {
			if (value > 0.8) {
				return TASKER_COLOR_RED;
			}
			else if (value > 0.4) {
				return TASKER_COLOR_YELLOW;
			}
			else {
				return TASKER_COLOR_GREEN;
			}
		}
		if (which == "notify") {
			return TASKER_COLOR_YELLOW;
		}
		if (which == "hour") {
			return TASKER_COLOR_GREY;
		}
		if (which == "faded") {
			return TASKER_COLOR_GREY;
		}
		if (which == "user") {
			return TASKER_COLOR_MAGENTA;
		}
		if (which == "tag") {
			return TASKER_COLOR_MAGENTA;
		}
		if (which == "target") {
			return TASKER_COLOR_BLUE;
		}
		if (which == "advice") {
			return TASKER_COLOR_GREEN;
		}
		if (which == "error") {
			return TASKER_COLOR_RED;
		}
		if (which == "note") {
			return TASKER_COLOR_CYAN;
		}
		return "";
	}
	char TaskerBase::usecolor()
	{
		if (!this->opt.use_colors) return '\0';
		return '\033';
	}

	//Console get:
	std::vector<std::string> TaskerBase::getUserName(bool& push_plan, bool allowskip, int taskIdForSkip, const std::string& userFixStr) {
		bool showAdvice = true;
		bool reloop = true;
		std::string userStr = "";
		while (reloop) {
			std::getline(std::cin, userStr);
			userStr = this->trim_gen(trim_copy(userStr), '"');
			std::string::iterator end_pos = std::remove(userStr.begin(), userStr.end(), ' ');
			userStr.erase(end_pos, userStr.end());
			if (userStr == "default") {
				userStr = this->getDefindUserName(0);
				reloop = false;
				push_plan = true;
				break;
			}
			else if (userStr == "") {
				reloop = false;
				push_plan = true;
				break;
			}
			else if (userStr == "skip" && allowskip) {
				if (taskIdForSkip == -1) {
					userStr = userFixStr;
				}
				else {
					userStr = this->thestruct["tasks"].at(taskIdForSkip).at("plan").back().at("user").get<std::string>();
				}
				reloop = false;

				//Clean name:
				this->cleanString(userStr, { ' ','"','\'','@','#' });
				break;
			}
			else if (userStr == "?") {
				int i = 0;
				int tot = (int)this->thestruct["users"].size();
				this->printTaskerInfo("Help", " Type one of those or several of them seperated by a single space.");
				std::cout << "             " << this->usecolor() << this->getcolor("faded");
				if (tot < 1) {
					std::cout << "No users created!";
				}
				else {
					for (json::iterator it = this->thestruct["users"].begin(); it != this->thestruct["users"].end(); ++it) {
						for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
							i++;
							std::cout << ite.key();
							if (i < tot) {
								std::cout << ", ";
							}
						}
					}
				}
				std::cout << this->usecolor() << this->getcolor("reset") << std::endl;
				std::cout << "\tType: ";
			}
			else if (userStr.length() > 1) {

				//Clean name:
				this->cleanString(userStr, { ' ','"','\'','@','#' });
				//check defined:
				if (!this->findDefinedUser(userStr, true)) {
					this->printTaskerInfo("Error", "The user name you typed can't be found.");
					if (showAdvice) {
						this->printTaskerInfo("Advice", "Leave empty and press ENTER for not assigned.");
						this->printTaskerInfo("Advice", "Type `default` and press ENTER for auto assign default user.");
						if (allowskip) this->printTaskerInfo("Advice", "Type `skip` and press ENTER to not change the user.");
						this->printTaskerInfo("Advice", "Type `?` to see all users defined or `tasker --users`.");
						showAdvice = false;
					}
					std::cout << "\tType: ";
				}
				else {
					reloop = false;
				}
			}
			else {
				reloop = false;
			}
		}
		//Create vector of user names:
		return userStr.length() > 0 ? this->splitString(userStr, TASKER_SPLIT_DELI_CHAR) : std::vector<std::string>();
	}
	std::string TaskerBase::getStrMessage(const std::string& err) {
		std::string mes;
		bool reloop = true;
		while (reloop) {
			std::getline(std::cin, mes);
			mes = trim_copy(mes);
			if (mes == "") {
				this->printTaskerInfo("Error", err);
				std::cout << "\tType: ";
			}
			else {
				reloop = false;
			}
		}
		return mes;
	}
	std::string TaskerBase::getStrDate(const std::string& err, bool allowSkip) {
		bool reloop = true;
		std::string datestr;
		while (reloop) {
			std::getline(std::cin, datestr);
			datestr = this->trim_gen(trim_copy(datestr), '"');
			if (datestr == "") {
				break;
			}
			else if (datestr == "today") {
				datestr = this->getcurdatetime("%d-%m-%Y 23:59:00");
				break;
			}
			else if (datestr == "skip" && allowSkip) {
				break;
			}
			else {
				int day = -1, mon = -1, year = -1, hour = 0, min = 0, sec = 0;
				int found = this->parseDateParts(datestr, day, mon, year, hour, min, sec);
				if (found == 3) {
					if (year < 1000 || day > 31 || day < 1 || mon > 12 || mon < 1) {
						this->printTaskerInfo("Error", err);
						std::cout << "\tType: ";
					}
					else {
						datestr = this->createDateFromInts(day, mon, year, 0, 0, 0);
						reloop = false;
					}
				}
				else if (found == 6) {
					if (year < 1000 || day > 31 || day < 1 || mon > 12 || mon < 1
						|| hour > 24 || hour < 0 || min < 0 || min > 60 || sec < 0 || sec > 60
						) {
						this->printTaskerInfo("Error", err);
						std::cout << "\tType: ";
					}
					else {
						datestr = this->createDateFromInts(day, mon, year, hour, min, sec);
						reloop = false;
					}
				}
				else {
					this->printTaskerInfo("Error", err);
					std::cout << "\tType: ";
				}
			}
		}
		return datestr;
	}
	int TaskerBase::getLoad(const std::string& err) {
		std::string loadStr;
		int loadint = 1;
		bool reloop = true;
		while (reloop) {
			std::getline(std::cin, loadStr);
			int scan_value = std::sscanf(loadStr.c_str(), "%d", &loadint);
			if (scan_value == 0) {
				// does not start with integer
				this->printTaskerInfo("Error", err);
				std::cout << "\tType: ";
			}
			else {
				// starts with integer
				reloop = false;
			}
		}
		return loadint;
	}
	std::vector<std::string> TaskerBase::getTags(const std::string& err) {
		bool reloop = true;
		bool showAdvice = true;
		std::string tagStr;
		while (reloop) {
			std::getline(std::cin, tagStr);
			tagStr = this->trim_gen(trim_copy(tagStr), '"');
			std::string::iterator end_pos = std::remove(tagStr.begin(), tagStr.end(), ' ');
			tagStr.erase(end_pos, tagStr.end());
			if (tagStr == "") {
				reloop = false;
				break;
			}
			else if (tagStr == "?") {
				int i = 0;
				int tot = (int)this->thestruct["tags"].size();
				this->printTaskerInfo("Help", " Type one of those or several of them seperated by a single space.");
				std::cout << "             " << this->usecolor() << this->getcolor("faded");
				if (tot < 1) {
					std::cout << "No tags defined!";
				}
				else {
					for (json::iterator it = this->thestruct["tags"].begin(); it != this->thestruct["tags"].end(); ++it) {
						for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
							i++;
							std::cout << ite.key();
							if (i < tot) {
								std::cout << ", ";
							}
						}
					}
				}
				std::cout << this->usecolor() << this->getcolor("reset") << std::endl;
				std::cout << "\tType: ";
			}
			else if (this->findDefinedTag(tagStr) == -1) {

				//Clean name:
				this->cleanString(tagStr, { ' ','"','\'','@','#' });

				if (!this->findDefinedTag(tagStr, true)) {
					this->printTaskerInfo("Error", err);
					if (showAdvice) {
						this->printTaskerInfo("Advice", "Leave empty and press ENTER for not tagged.");
						this->printTaskerInfo("Advice", "Type `?` to see all tags defined.");
						this->printTaskerInfo("Advice", "Run `--tags` to see all tags defined.");
						showAdvice = false;
					}
					std::cout << "\tType: ";
				}
				else {
					reloop = false;
				}
			}
			else {
				reloop = false;
			}
		}
		//Create vector of tags:
		return tagStr.length() > 0 ? this->splitString(tagStr, TASKER_SPLIT_DELI_CHAR) : std::vector<std::string>();
	}
	std::string TaskerBase::getStrVersion(bool& push_plan, bool allowskip, const std::string& versionForSkip) {
		std::string version;
		std::string currentversion = this->thestruct["version"];
		std::getline(std::cin, version);
		if (version == "skip" && allowskip) {
			version = versionForSkip;
		}
		else {
			push_plan = true;
			version = this->trim_gen((version != "" ? version : currentversion), '"');
		}
		return version;
	}

	//Helpers:
	bool TaskerBase::isFloat(std::string str) {
		std::istringstream iss(str);
		float f;
		iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
								   // Check the entire string was consumed and if either failbit or badbit is set
		return iss.eof() && !iss.fail();
	}
	float TaskerBase::getFloat(std::string str)
	{
		return std::stof(str);
	}
	float TaskerBase::normalizeStatus(std::string str) {
		float task_status_num = (str == "1" ||
			str == "true" ||
			str == "done" ||
			str == "finish"
			) ? (float)1 : (float)0;
		if (task_status_num != 1 && this->isFloat(str)) {
			return this->getFloat(str);
		}
		return task_status_num;
	}
	std::string TaskerBase::getUserString(std::vector<std::string>& users, std::string prefix, bool addNotAssigned) {

		std::stringstream users_string;
		std::string return_string;
		for (auto it = users.begin(); it != users.end(); ++it)
		{
			users_string << prefix << *it;
			if (std::next(it) != users.end()) // last element
				users_string << ", ";
		}

		return_string = users_string.str();
		return_string.erase(remove(return_string.begin(), return_string.end(), '\"'), return_string.end());
		if (return_string == "" && addNotAssigned) {
			return TASKER_USER_NOT_ASSIGNED;
		}
		return return_string;
	}
	bool TaskerBase::findDefinedUser(const std::string& user, bool multi) {
		std::vector<std::string> users_candid = this->splitString(user, TASKER_SPLIT_DELI_CHAR);
		int check = 0;
		for (auto &i : users_candid) {
			if (this->findDefinedUser(i) == -1) {
				return false;
			}
		}
		return true;
	}
	int TaskerBase::findDefinedUser(const std::string& user) {
		bool check = 0;
		int  index = 0;
		for (json::iterator it = this->thestruct["users"].begin(); it != this->thestruct["users"].end(); ++it) {
			for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
				if (ite.key() == user)
					return index;
			}
			index++;
		}
		return -1;
	}
	bool TaskerBase::findDefinedTag(const std::string& tag, bool multi) {
		std::vector<std::string> tags_candid = this->splitString(tag, TASKER_SPLIT_DELI_CHAR);
		int check = 0;
		for (auto &i : tags_candid) {
			if (this->findDefinedTag(i) == -1) {
				return false;
			}
		}
		return true;
	}
	int TaskerBase::findDefinedTag(const std::string& tag) {
		bool check = 0;
		int  index = 0;
		for (json::iterator it = this->thestruct["tags"].begin(); it != this->thestruct["tags"].end(); ++it) {
			for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
				if (ite.key() == tag)
					return index;
			}
			index++;
		}
		return -1;
	}
	std::string TaskerBase::getDefindUserName(int index) {
		for (unsigned i = 0; i < this->thestruct["users"].size(); i++) {
			if (i == index) {
				for (json::iterator ite = this->thestruct["users"].at(i).begin();
					ite != this->thestruct["users"].at(i).end();
					++ite
					) {
					return ite.key();
				}
			}
		}
		return "";
	}
	std::string TaskerBase::getReservedUserNames(const std::string& deli)
	{
		std::string reserved_names_str;
		for (auto value : tasker::reserve_user_names)
			reserved_names_str += value + deli;
		return reserved_names_str.substr(0, reserved_names_str.size() - deli.size());
	}
	std::string TaskerBase::getReservedTagNames(const std::string& deli)
	{
		std::string reserved_names_str;
		for (auto value : tasker::reserve_tag_names)
			reserved_names_str += value + deli;
		return reserved_names_str.substr(0, reserved_names_str.size() - deli.size());
	}
	std::string TaskerBase::trim_gen(const std::string& str, const char rem)
	{
		size_t first = str.find_first_not_of(rem);
		if (std::string::npos == first) return str;
		size_t last = str.find_last_not_of(rem);
		return str.substr(first, (last - first + 1));
	}
	void TaskerBase::cleanString(std::string& str, const std::vector<char>& rem)
	{
		for (auto &i : rem) {
			str.erase(std::remove(str.begin(), str.end(), i), str.end());
		}
	}
	std::vector<std::string> TaskerBase::splitString(const std::string &text, char sep) {
		std::vector<std::string> tokens;
		std::size_t start = 0, end = 0;
		while ((end = text.find(sep, start)) != std::string::npos) {
			if (end != start) {
				tokens.push_back(text.substr(start, end - start));
			}
			start = end + 1;
		}
		if (end != start) {
			tokens.push_back(text.substr(start));
		}
		return tokens;
	}
	std::vector<int> TaskerBase::parseTaskListStr(std::string str) {
		std::string deli = TASKER_SPLIT_DELI;
		std::vector<int> ret;
		size_t pos = 0;
		std::string token;
		while ((pos = str.find(deli)) != std::string::npos) {
			token = str.substr(0, pos);
			str.erase(0, pos + deli.length());
			exists row = this->findRow(token);
			if (row.type == 1)
				ret.push_back(row.taskId);
		}
		if (str.length() > 0) {
			exists row = this->findRow(str);
			if (row.type == 1)
				ret.push_back(row.taskId);
		}
		return ret;
	}

	TaskerBase::~TaskerBase()
	{
	}




}

