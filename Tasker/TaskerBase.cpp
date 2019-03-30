//
//  TaskerUpgrade.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#include "TaskerBase.hpp"

namespace tasker {

	json TaskerBase::thestruct;

	//Validation stuff:
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
	bool TaskerBase::isInteger(const std::string str) {
		if (str == "") return false;
		return str.find_first_not_of("0123456789") == std::string::npos;
	}
	bool TaskerBase::isNumber(const std::string str) {
		if (str == "") return false;
		return str.find_first_not_of("0123456789.") == std::string::npos;
	}
	bool TaskerBase::isFloat(std::string str) {
		std::istringstream iss(str);
		float f;
		iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
								   // Check the entire string was consumed and if either failbit or badbit is set
		return iss.eof() && !iss.fail();
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
		if (which == "high") {
			return TASKER_COLOR_HIGHLIGHT;
		}
		return "";
	}
	char TaskerBase::usecolor()
	{
		if (!this->opt.use_colors) return '\0';
		return '\033';
	}

	//Printing:
	void TaskerBase::printTaskerNotify(const std::string& mes)
	{
		std::cout
			<< " * "
			<< this->usecolor() << this->getcolor("notify")
			<< "Tasker said"
			<< this->usecolor() << this->getcolor("reset")
			<< " : "
			<< trim_copy(mes)
			<< std::endl;
	}
	void TaskerBase::printTaskerInfo(const std::string& type, const std::string& mes) // Types Error, Advice, Note
	{
		std::string useColor = "note";
		if (type == "Error") {
			useColor = "error";
		}
		else if (type == "Advice") {
			useColor = "advice";
		}
		std::cout
			<< "     "
			<< this->usecolor() << this->getcolor(useColor)
			<< type
			<< " -> "
			<< this->usecolor() << this->getcolor("reset")
			<< trim_copy(mes) << std::endl;
	}
	//Printing:
	void TaskerBase::printTaskerHighlighted(const std::string& mes, const std::string& value, std::size_t startneedle, bool loopall)
	{
		std::string print = mes;
		while (startneedle != std::string::npos) {
			std::cout << print.substr(0, startneedle) << this->usecolor() << this->getcolor("high");
			std::cout << print.substr(startneedle, value.length()) << this->usecolor() << this->getcolor("reset");
			print = print.substr(startneedle + value.length(), print.length() - (startneedle + value.length()));
			startneedle = print.find(value);
			if (!loopall) break;
		}
		std::cout << print << this->usecolor() << this->getcolor("reset") << std::endl;
	}
	std::string TaskerBase::getTagsAsStr() {
		int i = 0;
		std::stringstream sstr;

		int tot = (int)TaskerBase::thestruct["tags"].size();
		if (tot < 1) {
			return "";
		}
		else {
			for (json::iterator it = TaskerBase::thestruct["tags"].begin(); it != TaskerBase::thestruct["tags"].end(); ++it) {
				for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
					i++;
					sstr << ite.key();
					if (i < tot) sstr << ", ";
				}
			}
		}
		return std::string(sstr.str());
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
					userStr = TaskerBase::thestruct["tasks"].at(taskIdForSkip).at("plan").back().at("user").get<std::string>();
				}
				reloop = false;

				//Clean name:
				this->cleanString(userStr, { ' ','"','\'','@','#' });
				break;
			}
			else if (userStr == "?") {
				int i = 0;
				int tot = (int)TaskerBase::thestruct["users"].size();
				this->printTaskerInfo("Help", " Type one of those or several of them seperated by a single space.");
				std::cout << "             " << this->usecolor() << this->getcolor("faded");
				if (tot < 1) {
					std::cout << "No users created!";
				}
				else {
					for (json::iterator it = TaskerBase::thestruct["users"].begin(); it != TaskerBase::thestruct["users"].end(); ++it) {
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
				std::string tagsString = this->getTagsAsStr();
				this->printTaskerInfo("Help", " Type one of those or several of them seperated by a single space.");
				std::cout << "             " << this->usecolor() << this->getcolor("faded");
				if (tagsString == "") {
					std::cout << "No tags defined!";
				} else {
					std::cout << tagsString;
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
	
	bool TaskerBase::promptUser(const std::string& mes) {
		std::string buffer = "";
		std::cout << mes;
		std::getline(std::cin, buffer);
		return buffer == "y" || buffer == "yes" || buffer == "ok" || buffer == "go";
	}

	//Base elements builds and get:
	std::string TaskerBase::getStrVersion(bool& push_plan, bool allowskip, const std::string& versionForSkip) {
		std::string version;
		std::string currentversion = TaskerBase::thestruct["version"].back();
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
	json::object_t TaskerBase::getBaseTaskPlan() {
		return this->getBaseTaskPlan(std::string(""), std::string(""));
	}
	json::object_t TaskerBase::getBaseTaskPlan(std::string& date) {
		return this->getBaseTaskPlan(date, std::string(""));
	}
	json::object_t TaskerBase::getBaseTaskPlan(std::string& date, std::string& version) {
		json::object_t basePlan = json::object();
		basePlan.emplace("date", date);
		basePlan.emplace("user", json::array());
		basePlan.emplace("v", version);
		return basePlan;
	}

	//Basic op
	/* Finds a row base on a string [23 | 23.3 ...]
	 * Returns an exists object that identify wich type it is and what is the correct id.
	 * note: will ignore cancels....
	*/
	exists TaskerBase::findRow(const std::string& strId) {
		exists ret;
		ret.type = 0;
		std::vector<std::string> parts = this->splitString(strId, '.');
		if (parts.size() == 1 && this->onlyDigits(parts.at(0))) {
			//Target a task row:
			ret.type = 1;
			ret.taskId = stoi(parts.at(0)) - 1;
			ret.id = stof(parts.at(0));
			//Validate Id
			if (ret.taskId < 0
				|| (int)TaskerBase::thestruct["tasks"].size() <= ret.taskId
				|| TaskerBase::thestruct["tasks"].at(ret.taskId).at("cancel") == true
				) {
				ret.type = 0;
				return ret;
			};
		}
		else if (parts.size() == 2 && this->onlyDigits(parts.at(0)) && this->onlyDigits(parts.at(1))) {
			//Target a report row:
			ret.type = 2;
			ret.taskId = stoi(parts.at(0)) - 1;
			ret.reportId = stoi(parts.at(1)) - 1;
			ret.id = stof(parts.at(0) + "." + parts.at(1));
			//Validate Id
			if (ret.taskId < 0
				|| (int)TaskerBase::thestruct["tasks"].size() <= ret.taskId
				|| TaskerBase::thestruct["tasks"].at(ret.taskId).at("cancel") == true
				|| (int)TaskerBase::thestruct["tasks"].at(ret.taskId).at("report").size() <= ret.reportId
				) {
				ret.type = 0;
				return ret;
			};
		}
		else {
			return ret;
		}
		return ret;
	}

	//Helpers:
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
		for (json::iterator it = TaskerBase::thestruct["users"].begin(); it != TaskerBase::thestruct["users"].end(); ++it) {
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
		for (json::iterator it = TaskerBase::thestruct["tags"].begin(); it != TaskerBase::thestruct["tags"].end(); ++it) {
			for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
				if (ite.key() == tag)
					return index;
			}
			index++;
		}
		return -1;
	}
	std::string TaskerBase::getDefindUserName(int index) {
		for (unsigned i = 0; i < TaskerBase::thestruct["users"].size(); i++) {
			if (i == index) {
				for (json::iterator ite = TaskerBase::thestruct["users"].at(i).begin();
					ite != TaskerBase::thestruct["users"].at(i).end();
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
		for (auto value : RESERVE_USER_NAMES)
			reserved_names_str += value + deli;
		return reserved_names_str.substr(0, reserved_names_str.size() - deli.size());
	}
	std::string TaskerBase::getReservedTagNames(const std::string& deli)
	{
		std::string reserved_names_str;
		for (auto value : RESERVE_TAG_NAMES)
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
	// trim from start (in place)
	void TaskerBase::ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
			return !std::isspace(ch);
		}));
	}
	// trim from end (in place)
	void TaskerBase::rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
			return !std::isspace(ch);
		}).base(), s.end());
	}
	// trim from both ends (in place)
	void TaskerBase::trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}
	// trim from start (copying)
	std::string TaskerBase::ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}
	// trim from end (copying)
	std::string TaskerBase::rtrim_copy(std::string s) {
		rtrim(s);
		return s;
	}
	// trim from both ends (copying)
	std::string TaskerBase::trim_copy(std::string s) {
		trim(s);
		return s;
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

