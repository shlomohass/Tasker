//
//  Tasker.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#include "TaskerMain.hpp"
#include "SETTASKER.hpp"
#include "TaskerAdd.hpp"

#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>
#include <map>
#include <iterator>

#ifdef PLATLINUX
	#include <string>
	#include <limits.h>
	#include <unistd.h>
    #include <time.h>
	#ifndef MAX_PATH
		#define MAX_PATH PATH_MAX
	#endif	
#else
	#include <string>
	#include <io.h>
	#include <windows.h>
	#define RW_OK   6       /* Test for read & write permission.  */
	#define R_OK    4       /* Test for read permission.  */
	#define W_OK    2       /* Test for write permission.  */
	#define X_OK    1       /* execute permission - unsupported in windows*/
	#define F_OK    0
#endif

namespace tasker {

using json = nlohmann::json;


// trim from start (in place)
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
		return !std::isspace(ch);
	}));
}
// trim from end (in place)
static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
		return !std::isspace(ch);
	}).base(), s.end());
}
// trim from both ends (in place)
static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}
// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}
// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}
// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}


TaskerMain::TaskerMain(bool _color)
{
	this->color = _color;
	this->basepath = "notset";
	this->fullpath = "notset";
}
void TaskerMain::setPath()
{

	#ifdef PLATLINUX
		char buffer[MAX_PATH];
		if (getcwd(buffer, sizeof(buffer))) {
            this->basepath = std::string(buffer);
			this->fullpath = std::string(buffer) + TASKER_FOLDER_SEP + TASKER_OBJNAME;
		}
	#else
		char buffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buffer);
		this->basepath = std::string(buffer);
		this->fullpath = std::string(buffer) + TASKER_FOLDER_SEP + TASKER_OBJNAME;
	#endif
}
void TaskerMain::_basepath(std::string path)
{
	this->basepath = path;
}
void TaskerMain::_fullpath(std::string path)
{
	this->fullpath = path;
}
std::string TaskerMain::_basepath()
{
	return this->basepath;
}
std::string TaskerMain::_fullpath()
{
	return this->fullpath;
}
bool TaskerMain::loadBase()
{
	#ifdef PLATLINUX
		if (access(this->fullpath.c_str(), F_OK) == 0) {
			return true;
		}
	#else
		if (_access(this->fullpath.c_str(), F_OK) == 0) {
			return true;
		}
	#endif
		return false;
}
bool TaskerMain::closeBase()
{
	return true;
}
void TaskerMain::parseOptions(bool colors_override)
{
	//Set color mode:
	if (!colors_override) {
		this->color = this->thestruct["tasker"].at("usecolors");
	}
	//Set load modes:
	this->load = this->thestruct["tasker"].at("enableloads");

	//Set delete flag:
	this->delitems = this->thestruct["tasker"].at("allowdelete");
}
bool TaskerMain::setOption(const std::string& which, const std::string& state)
{
	//Validate state:
	if (state != "1" && state != "0" && state != "true" && state != "false") {
		return false;
	}
	//Parse state:
	bool boolstate = (state == "1" || state== "true") ? true : false;

	//Save to object
	if (which == "optcolor") {
		this->thestruct["tasker"].at("usecolors") = boolstate;

	} else if (which == "optdelete") {
		this->thestruct["tasker"].at("allowdelete") = boolstate;

	} else if (which == "optloads") {
		this->thestruct["tasker"].at("enableloads") = boolstate;

	} else {
		return false;
	}
	//Reparse options:
	this->parseOptions(false);

	//Notify:
	this->printTaskerNotify("Option saved!");
	std::cout << std::endl;

	return true;
}
void TaskerMain::createEmpty()
{	
	//Sets :
	this->thestruct["tasker"] = {
		{ "version",			TASKER_VERSION	},
		{ "usecolors",			true			},
		{ "enableloads",		true			},
		{ "allowdelete",		true			}
	};
	this->thestruct["users"] = json::array();
	this->thestruct["types"] = {{"task",{"desc","simple task"}}};
	this->thestruct["tasks"] = json::array();
	this->thestruct["tags"]	 = json::array();
	this->thestruct["note"]	 = json::object();
	std::string projName;
	std::string projDesc;
	std::string projVersion;
	std::string userName;
	std::string userDesc;
	std::string userEmail;

	//Interactively get all needed:
	std::cout << std::endl << "> Quick Tasker setup: ";
	std::cout << std::endl << "  1. Set the project name: ";
	std::getline(std::cin, projName);
	std::cout << "  2. Set the project description: ";
	std::getline(std::cin, projDesc);
	std::cout << "  3. Set the project version: ";
	std::getline(std::cin, projVersion);
	std::cout << "  4. Set the default user name: ";
	std::getline(std::cin, userName);
	std::cout << "  5. Set the default user description: ";
	std::getline(std::cin, userDesc);
	std::cout << "  6. Set the default user email address: ";
	std::getline(std::cin, userEmail);

	//Finish:
	this->thestruct["name"]		= projName;
	this->thestruct["desc"]		= projDesc;
	this->thestruct["version"]	= projVersion;
	this->thestruct["users"].push_back({ { userName, {{"desc",userDesc },{"mail",userEmail }}} });
}
void TaskerMain::createEmpty(json structure) 
{
	this->thestruct = structure;
}
std::string TaskerMain::getSerialized()
{
	return this->thestruct.dump();
}
std::string TaskerMain::getSerialized(int pretty_spaces)
{
	return this->thestruct.dump(pretty_spaces);
}
bool TaskerMain::checkWriteObj(bool full)
{	
	if (full)
		return this->checkWriteObj(this->fullpath);
	else
		return this->checkWriteObj(this->basepath);
}
bool TaskerMain::checkWriteObj(std::string& path) 
{
	#ifdef PLATLINUX
		if (access(path.c_str(), W_OK) == 0 || access(path.c_str(), W_OK) == 0)
			return true; 
	#else
		if (_access(path.c_str(), W_OK) == 0 || _access(path.c_str(), RW_OK) == 0)
			return true; 
	#endif
	return false;
}
bool TaskerMain::writeObj(bool newobj)
{
	if (this->checkWriteObj(newobj)) {
		std::ofstream ofs;
		ofs.open(this->fullpath, std::ofstream::out | std::ofstream::trunc);
		ofs << this->getSerialized(4);
		ofs.close();
		return true;
	} 
	return false;
}
bool TaskerMain::loadObj()
{
	if (this->checkWriteObj(true)) {
		std::ifstream ifs(this->fullpath);
		std::string content((std::istreambuf_iterator<char>(ifs)),
							(std::istreambuf_iterator<char>()));
		ifs.close();
		this->thestruct = json::parse(content.c_str());
		return true;
	}
	return false;
}

//Prints:
void TaskerMain::printTaskerNotify(const std::string& mes)
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
void TaskerMain::printTaskerInfo(const std::string& type, const std::string& mes) // Types Error, Advice, Note
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

std::string TaskerMain::getcurdatetime() {
	return this->getcurdatetime("%d-%m-%Y %H:%M:%S");
}
std::string TaskerMain::getcurdatetime(const std::string& format) {
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
int TaskerMain::parseDateParts(
	const std::string& datestr,
	int& day, int& month, int& year, int& hours, int& minutes, int& seconds
) {
	return this->parseDateParts(datestr, "%2d-%2d-%4d %2d:%2d:%2d", day, month, year, hours, minutes, seconds);
}
int TaskerMain::parseDateParts(
	const std::string& datestr, 
	const std::string& format, int& day, int& month, int& year, int& hours, int& minutes, int& seconds
) {
	int  _month, _day, _year, _hours, _minutes, _seconds;
	int found = 0;
	found = sscanf(datestr.c_str(), format.c_str(), &_day, &_month, &_year, &_hours, &_minutes, &_seconds);
	day			= _day;
	month		= _month;
	year		= _year;
	hours		= _hours;
	minutes		= _minutes;
	seconds		= _seconds;
	return found;
}
std::string TaskerMain::createDateFromInts(int day, int mon, int year, int hour, int min, int sec)
{
	std::string s_day	= day < 10 ? "0" : "";
	std::string s_mon	= mon < 10 ? "0" : "";
	std::string s_hour	= hour < 10 ? "0" : "";
	std::string s_min	= min < 10 ? "0" : "";
	std::string s_sec	= sec < 10 ? "0" : "";
	std::stringstream ss;
	//put arbitrary formatted data into the stream
	ss 
		<< s_day  << day << "-"
		<< s_mon  << mon << "-"
		<< year   << " "
		<< s_hour << hour << ":"
		<< s_min  << min  << ":"
		<< s_sec  << sec
		;
	//convert the stream buffer into a string
	return ss.str();
}
bool TaskerMain::isFloat(std::string str) {
	std::istringstream iss(str);
	float f;
	iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
							   // Check the entire string was consumed and if either failbit or badbit is set
	return iss.eof() && !iss.fail();
}
float TaskerMain::getFloat(std::string str) 
{
	return std::stof(str);
}
std::vector<int> TaskerMain::parseTaskListStr(std::string str) {
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

//Get Console color Values:
std::string TaskerMain::getcolor(const std::string& which)
{
	return this->getcolor(which, 0, "");
}
std::string TaskerMain::getcolor(const std::string& which, const std::string& svalue)
{
	return this->getcolor(which, 0, svalue);
}
std::string TaskerMain::getcolor(const std::string& which, float fvalue)
{
	return this->getcolor(which, fvalue, "");
}
std::string TaskerMain::getcolor(const std::string& which, float value, const std::string& svalue)
{
	if (!this->color) return "";
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
		} else {
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
char TaskerMain::usecolor()
{
	if (!this->color) return '\0';
	return '\033';
}

float TaskerMain::normalizeStatus(std::string str) {
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
std::time_t TaskerMain::getEpochTime(const std::wstring& dateTime)
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
int TaskerMain::findDefinedUser(const std::string& user) {
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
int TaskerMain::findDefinedTag(const std::string& tag) {
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
std::string TaskerMain::getDefindUserName(int index) {
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
std::string TaskerMain::getReservedUserNames(const std::string& deli)
{
	std::string reserved_names_str;
	for (auto value : tasker::reserve_user_names)
		reserved_names_str += value + deli;
	return reserved_names_str.substr(0, reserved_names_str.size() - deli.size());
}
std::string TaskerMain::getReservedTagNames(const std::string& deli)
{
	std::string reserved_names_str;
	for (auto value : tasker::reserve_tag_names)
		reserved_names_str += value + deli;
	return reserved_names_str.substr(0, reserved_names_str.size() - deli.size());
}
std::string TaskerMain::trim_gen(const std::string& str, const char rem)
{
	size_t first = str.find_first_not_of(rem);
	if (std::string::npos == first) return str;
	size_t last = str.find_last_not_of(rem);
	return str.substr(first, (last - first + 1));
}
std::vector<std::string> TaskerMain::splitString(const std::string &text, char sep) {
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
bool TaskerMain::onlyDigits(const std::string str) {
	if (str == "") return false;
	return str.find_first_not_of("0123456789") == std::string::npos;
}
exists TaskerMain::findRow(const std::string& strId) {
	exists ret;
	ret.type = 0;
	std::vector<std::string> parts = this->splitString(strId, '.');
	if (parts.size() == 1 && this->onlyDigits(parts.at(0))) {
		//Target a task row:
		ret.type	= 1;
		ret.taskId	= stoi(parts.at(0)) - 1;
		ret.id		= stof(parts.at(0));
		//Validate Id
		if (ret.taskId < 0 
			|| (int)this->thestruct["tasks"].size() <= ret.taskId
			|| this->thestruct["tasks"].at(ret.taskId).at("cancel") == true
		) {
			ret.type = 0;
			return ret;
		};
	}
	else if (parts.size() == 2 && this->onlyDigits(parts.at(0)) && this->onlyDigits(parts.at(1))) {
		//Target a report row:
		ret.type		= 2;
		ret.taskId		= stoi(parts.at(0)) - 1;
		ret.reportId	= stoi(parts.at(1)) - 1;
		ret.id = stof(parts.at(0) + "." + parts.at(1));
		//Validate Id
		if (ret.taskId < 0 
			|| (int)this->thestruct["tasks"].size() <= ret.taskId
			|| this->thestruct["tasks"].at(ret.taskId).at("cancel") == true
			|| (int)this->thestruct["tasks"].at(ret.taskId).at("report").size() <= ret.reportId
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

std::string TaskerMain::getUserName(bool& push_plan, bool allowskip, int taskIdForSkip, const std::string& userFixStr) {
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
			break;
		}
		else if (this->findDefinedUser(userStr) == -1) {
			this->printTaskerInfo("Error", "The user name you typed can't be found.");
			if (showAdvice) {
				this->printTaskerInfo("Advice", "Leave empty and press ENTER for not assigned.");
				this->printTaskerInfo("Advice", "Type `default` and press ENTER for auto assign default user.");
				if (allowskip) this->printTaskerInfo("Advice", "Type `skip` and press ENTER to not change the user.");
				this->printTaskerInfo("Advice", "Run `--users` to see all users defined.");
				showAdvice = false;
			}
			std::cout << "\tType: ";
		}
		else {
			reloop = false;
		}
	}
	return userStr;
}
std::string TaskerMain::getStrMessage(const std::string& err) {
	std::string mes;
	bool reloop = true;
	while (reloop) {
		std::getline(std::cin, mes);
		mes = trim_copy(mes);
		if (mes == "") {
			this->printTaskerInfo("Error", err);
			std::cout << "\tType: ";
		} else {
			reloop = false;
		}
	}
	return mes;
}
std::string TaskerMain::getStrDate(const std::string& err, bool allowSkip) {
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
int TaskerMain::getLoad(const std::string& err) {
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
		} else {
			// starts with integer
			reloop = false;
		}
	}
	return loadint;
}
std::string TaskerMain::getStrTag(const std::string& err) {
	bool reloop		 = true;
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
		else if (this->findDefinedTag(tagStr) == -1) {
			this->printTaskerInfo("Error", err);
			if (showAdvice) {
				this->printTaskerInfo("Advice", "Leave empty and press ENTER for not tagged.");
				this->printTaskerInfo("Advice", "Run `--tags` to see all tags defined.");
				showAdvice = false;
			}
			std::cout << "\tType: ";
		}
		else {
			reloop = false;
		}
	}
	return tagStr;
}
std::string TaskerMain::getStrVersion(bool& push_plan, bool allowskip, const std::string& versionForSkip) {
	std::string version;
	std::string currentversion = this->thestruct["version"];
	std::getline(std::cin, version);
	if (version == "skip" && allowskip) {
		version = versionForSkip;
	} else {
		push_plan = true;
		version = this->trim_gen((version != "" ? version : currentversion), '"');
	}
	return version;
}

bool TaskerMain::setNewTask(const std::string& strTask)
{	
	std::string plan_user			= "";
	std::string tagged_as			= "";
	std::string plan_currentversion = this->thestruct["version"];
	std::string plan_version		= "";
	std::string plan_duedate		= "";
	std::string task_created		= this->getcurdatetime();
	std::string task_status			= "";
	int			loadint				= 1;
	bool        push_plan			= false;
	float		task_status_num;

	//Interactively get all needed:
	std::cout << std::endl << " > New task: ";

	//Assign a user name:
	std::cout << std::endl << "  1. Assign to user (empty for none): ";
	plan_user = this->getUserName(push_plan, false, 0, "");

	//Tag the task:
	std::cout << "  2. Tag the task (empty for none): ";
	tagged_as = this->getStrTag("The Tag you typed can't be found.");

	//Set the planned version:
	std::cout << "  3. Planned for version (empty for current): ";
	plan_version = this->getStrVersion(push_plan, false, "");

	//Set due date:
	std::cout << "  4. Due date `d-m-Y H:M:S` (empty for none or `today`): ";
	plan_duedate = this->getStrDate("Please retry. Use the correct format: `d-m-Y H:M:S` or `d-m-Y`.", false);
	
	//Set status:
	std::cout << "  5. Set current status (1|0, true|false): ";
	std::getline(std::cin, task_status);
	task_status_num = this->normalizeStatus(task_status);

	//Set load of task if enabled:
	if (this->load) {
		std::cout << "  6. Set load units of this task (use an integer): ";
		loadint = this->getLoad("Bad input - Please enter a positive integer only.");
	}

	//Create finall Object:
	json taskObj = {
		{ "plan",		""					},
		{ "created",	task_created		},
		{ "updated",	task_created		},
		{ "task",		this->trim_gen(trim_copy(strTask), '"')	},
		{ "status",		task_status_num		},
		{ "cancel",		false				},
		{ "load",		loadint				},
		{ "tagged",		""					},
		{ "report",		""					} 
	};
	taskObj["tagged"] = json::array();
	if (tagged_as != "") taskObj["tagged"].push_back(tagged_as);
	taskObj["plan"] = json::array(); 
	taskObj["plan"].push_back({ 
		{ "v" ,			trim_copy(plan_version) },
		{ "user" ,		plan_user				},
		{ "date" ,		plan_duedate			}
	});
	taskObj["report"] = json::array();

	//Save
	this->thestruct["tasks"].push_back(taskObj);

	//Notify:
	this->printTaskerNotify("New task Created!");
	this->printTaskerInfo("Info","Task ID is: " + std::to_string(this->thestruct["tasks"].size()));
	std::cout << std::endl;
	return true;
}
bool TaskerMain::reportToTask(const std::string& strTask) {

	int theTask = -1;
	//Validate the index: 
	try { theTask = stoi(strTask) - 1; }
	catch (...) { return false; }
	if (theTask < 0) return false;

	//Does the task exists:
	if ((int)this->thestruct["tasks"].size() <= theTask) { return false; }

	//Does the task enabled:
	if (this->thestruct["tasks"].at(theTask).at("cancel") == true) { return false; }

	std::string rep_date = this->getcurdatetime();
	std::string rep_user = "";
	std::string rep_note = "";
	std::string rep_status = "";
	std::string owner = this->thestruct["tasks"].at(theTask).at("plan").back().at("user");
	float		rep_status_num;
	bool		push_plan = true;
	
	//Set percision:
	std::cout << std::setprecision(2) << std::fixed;
	//Interactively get all needed:
	std::cout << std::endl << " > Report to task: " << strTask;

	//Set an updated progress:
	std::cout << std::endl << "  1. Set new progress status (0.0 - 1.0, current "; 
	std::cout << usecolor() << getcolor("workbar");
	std::cout << (float)this->thestruct["tasks"].at(theTask).at("status");
	std::cout << usecolor() << getcolor("reset");
	std::cout << "): ";
	std::getline(std::cin, rep_status);

	//Normalize progress status:
	rep_status_num = this->normalizeStatus(rep_status);

	//Get the user who made the report:
	std::cout << "  2. Progress of user (assigned to "; 
	std::cout << usecolor() << getcolor("user");
	std::cout << ((owner == "") ? "not assigned" : TASKER_USER_PREFIX + owner);
	std::cout << usecolor() << getcolor("reset");
	std::cout << " - Empty for unknown" << "): ";
	rep_user = this->getUserName(push_plan, false, 0, "");
	
	//Get the report note:
	std::cout << "  3. Task progress report note: ";
	rep_note = this->getStrMessage("Please retry. You must add a report note.");

	//Push to plan notes:
	this->thestruct["tasks"].at(theTask).at("report").push_back({
		{"date",		this->trim_gen(rep_date, '"')			 },
		{"status",		rep_status_num							 },
		{"note",		this->trim_gen(trim_copy(rep_note), '"') },
		{"by",			this->trim_gen(trim_copy(rep_user), '"') }
	});

	//Update parent status:
	this->thestruct["tasks"].at(theTask).at("status") = rep_status_num;

	this->printTaskerNotify("Reported and saved successfully! ");
	this->printTaskerInfo("Info", "Reported to Task Id: " + strTask);

	return true;
}
bool TaskerMain::refactorTask(const std::string& strTask) {

	exists theRow = this->findRow(strTask);

	if (theRow.type == 0) {
		return false;
	}
	else if (theRow.type == 1) {

		//Refactor Task:
		std::string new_task_title = "";
		std::string plan_user = "";
		std::string plan_version = "";
		std::string plan_duedate = "";
		std::string task_updated = this->getcurdatetime();
		std::string task_load = "1";
		int			loadint		= 1;
		bool        push_plan	= false;

		//Interactively update all needed:
		std::cout << std::endl << " > Refactor Task: ";
		std::cout << usecolor() << getcolor("faded") 
				  << this->thestruct["tasks"].at(theRow.taskId).at("task")
				  << usecolor() << getcolor("reset");

		//Set new title:
		std::cout << std::endl << "  1. Set New task title (empty for skip): ";
		std::getline(std::cin, new_task_title);
		new_task_title = (new_task_title == "") ? 
			this->thestruct["tasks"].at(theRow.taskId).at("task").get<std::string>() : 
			this->trim_gen(trim_copy(new_task_title), '"');

		//Change assigned user:
		std::cout << "  2. Change user (empty = none | 'skip' = skip): ";
		plan_user = this->getUserName(push_plan, true, theRow.taskId, "");

		//Update the planned version:
		std::cout << "  3. Update planned for version (empty = current | 'skip' = skip): ";\
		plan_version = this->getStrVersion(
			push_plan, true,
			this->thestruct["tasks"].at(theRow.taskId).at("plan").back().at("v").get<std::string>()
		);

		//Update Due date:
		std::string prevDateSet = this->thestruct["tasks"].at(theRow.taskId).at("plan").back().at("date").get<std::string>();
		std::cout << "  4. Update Due date (" 
				  << this->usecolor() << this->getcolor("hour")
				  << (prevDateSet == "" ? "not set" : prevDateSet)
				  << this->usecolor() << this->getcolor("reset")
				  << ") - Expects `d-m-Y H:M:S` | empty for none | `today` | `skip`: ";
		plan_duedate = this->getStrDate("Please retry. Use the correct format: `d-m-Y H:M:S` or `d-m-Y`.", true);
		if (plan_duedate == "skip") {
			plan_duedate = prevDateSet;
			push_plan = true;
		}

		//Update Load if enabled:
		if (this->load) {
			int prevLoadSet = this->thestruct["tasks"].at(theRow.taskId).at("load").get<int>();
			std::cout << "  5. Update load units of this task (use an integer) - Currently `"
				<< this->usecolor() << this->getcolor("notify")
				<< prevLoadSet
				<< this->usecolor() << this->getcolor("reset")
				<< "`: ";
			loadint = this->getLoad("Bad input - Please enter a positive integer only.");
		}

		//Create finall Object:
		json taskObj = {
			{ "plan",		this->thestruct["tasks"].at(theRow.taskId).at("plan")	},
			{ "created",	this->thestruct["tasks"].at(theRow.taskId).at("created")},
			{ "updated",	task_updated											},
			{ "task",		new_task_title											},
			{ "status",		this->thestruct["tasks"].at(theRow.taskId).at("status") },
			{ "cancel",		false													},
			{ "load",		loadint													},
			{ "tagged",		this->thestruct["tasks"].at(theRow.taskId).at("tagged") },
			{ "report",		this->thestruct["tasks"].at(theRow.taskId).at("report") }
		};
		//Add plan if needed:
		if (push_plan) {
			taskObj["plan"].push_back({
				{ "v" ,			trim_copy(plan_version)			  },
				{ "user" ,		plan_user						  },
				{ "date" ,		plan_duedate					  }
			});
		}

		//Save
		this->thestruct["tasks"].at(theRow.taskId) = taskObj;

		//Notify:
		this->printTaskerNotify("Task Updated successfully!");
		this->printTaskerInfo("Info", "Task ID is: " + std::to_string(theRow.taskId + 1));
		std::cout << std::endl;
	}
	else {

		//Refactor Report:
		json theObjOld = this->thestruct["tasks"].at(theRow.taskId).at("report").at(theRow.reportId);
		std::string rep_note = "";
		std::string rep_status = "";
		bool		push_plan = true;

		//Set percision:
		std::cout << std::setprecision(2) << std::fixed;

		//Interactively update all needed:
		std::cout << std::endl << " > Refactor Task report: ";
		std::cout << usecolor() << getcolor("faded")
				  << theObjOld["note"]
				  << usecolor() << getcolor("reset");

		//Update report progress:
		std::cout << std::endl << "  1. Update progress status (0.0 - 1.0, current ";
		std::cout << usecolor() << getcolor("workbar");
		std::cout << (float)theObjOld.at("status");
		std::cout << usecolor() << getcolor("reset");
		std::cout << ") OR type `skip`: ";
		std::getline(std::cin, rep_status);
		rep_status  = this->trim_gen(trim_copy(rep_status), '"');

		//Normalize progress status:
		if (rep_status != "skip") {
			theObjOld["status"] = this->normalizeStatus(rep_status);
		}

		//Get the user who made the report:
		std::cout << "  2. Updtae user that issued the report (currently ";
		std::cout << usecolor() << getcolor("user");
		std::cout << ((theObjOld["by"].get<std::string>() == "") ? "not assigned" : TASKER_USER_PREFIX + theObjOld["by"].get<std::string>());
		std::cout << usecolor() << getcolor("reset");
		std::cout << " - Expects empty for unknown | `skip` | `default`" << "): ";
		theObjOld["by"] = this->trim_gen(trim_copy(this->getUserName(push_plan, true, -1, theObjOld["by"].get<std::string>())), '"');

		//Get the report note:
		std::cout << "  3. Update task report note (Type `skip` to skip): ";
		rep_note = this->trim_gen(trim_copy(this->getStrMessage("Please retry. You must add a report note.")),'"');
		if (rep_note != "skip") {
			theObjOld["note"] = rep_note;
		}

		//Push to plan notes:
		this->thestruct["tasks"].at(theRow.taskId).at("report").at(theRow.reportId) = theObjOld;

		//Update parent status:
		if (theRow.reportId == (int)this->thestruct["tasks"].at(theRow.taskId).at("report").size() - 1) {
			this->thestruct["tasks"].at(theRow.taskId).at("status") = theObjOld["status"];
		}

		this->printTaskerNotify("Refactored task report and saved successfully! ");
		this->printTaskerInfo("Info", "Report Id: " + std::to_string(theRow.taskId + 1) + "." + std::to_string(theRow.reportId + 1));
	}
	return true;
}
bool TaskerMain::cancelTask(const std::string& strTask, bool state)
{
	int theTask = -1;

	//Validate the index: 
	try { theTask = stoi(strTask) - 1; }
	catch (...) { return false; }
	if (theTask < 0) return false;

	//Does the task exists:
	if ((int)this->thestruct["tasks"].size() <= theTask) { return false; }

	//Set cancel
	if (state) {
		std::cout << std::endl << " > Canceling task: " << theTask << std::endl;
		this->thestruct["tasks"].at(theTask).at("cancel") = true;
		this->printTaskerNotify("Task marked as canceled!");
		this->printTaskerInfo("Info", "You can run `--listcancel` to see all canceled tasks");
		this->printTaskerInfo("Info", "You can enable a task again by running `--enabletask {id}`");
	} else {
		std::cout << std::endl << " > Enabling task: " << theTask << std::endl;
		this->thestruct["tasks"].at(theTask).at("cancel") = false;
		this->printTaskerNotify("Task was enabled!");
		this->printTaskerInfo("Info", "You can run `--listcancel` to see all canceled tasks");
	}
	std::cout << std::endl;
	return true;
}
bool TaskerMain::deleteTask(const std::string& strTask)
{
	int theTask = -1;

	//Validate the index: 
	try { theTask = stoi(strTask) - 1; }
	catch (...) { return false; }
	if (theTask < 0) return false;

	//Does the task exists:
	if ((int)this->thestruct["tasks"].size() <= theTask) { return false; }
	
	//Validate allowed delete:
	if (this->delitems == false) {
		this->printTaskerNotify("Task delete Prevented!");
		this->printTaskerInfo("Info", "You can't delete tasks because a global option was set tp prevent that - you may want to `--cancel {id}` insted.");
	} else {
		//Delete task
		std::cout << std::endl << " > Deleting task: " << theTask << std::endl;
		this->thestruct["tasks"].erase(theTask);
		this->printTaskerNotify("Task deleted successfully!");
	}
	std::cout << std::endl;
	return true;
}

void TaskerMain::showtags()
{
	//Print main
	std::cout << std::endl << " > Defined tags: " << std::endl << std::endl;

	int counter = 0;
	//Iterate:
	for (json::iterator it = this->thestruct["tags"].begin(); it != this->thestruct["tags"].end(); ++it) {
		for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
			counter++;
			std::string desc = ite.value().at("desc");
			std::cout
				<< "   (" << counter << ") "
				<< this->usecolor() << this->getcolor("tag")
				<< TASKER_TAG_PREFIX << ite.key()
				<< " -> "
				<< this->usecolor() << this->getcolor("hour")
				<< (desc == "" ? "Description not set" : desc)
				<< this->usecolor() << this->getcolor("reset")
				<< std::endl;
		}
	}
	if (counter > 0) {
		std::cout << std::endl;
		this->printTaskerInfo("Info", "Total tags defined: " + std::to_string(counter));
		this->printTaskerInfo("Info", "Reserved tag names: " + this->getReservedUserNames(", ") + ".");
		std::cout << std::endl;
	}
	else {
		this->printTaskerNotify("No tags were defined");
		this->printTaskerInfo("Advice", "You can use `--addtag {tagname}` to define a tag.");
	}
}
bool TaskerMain::addtag(const std::string& _tag, const std::string& strTask) {

	std::string tag = this->trim_gen(trim_copy(_tag), '"');
	std::string taskListStr;
	std::vector<int> taskList;

	std::cout << std::endl
		<< " > Tagging with: "
		<< this->usecolor() << this->getcolor("tag")
		<< TASKER_TAG_PREFIX << tag
		<< this->usecolor() << this->getcolor("reset");

	if (strTask == "") {
		std::cout << std::endl << std::endl
			<< "  1. Which task to tag (list of integers): ";
		std::getline(std::cin, taskListStr);
		taskListStr = this->trim_gen(trim_copy(taskListStr), '"');
	} else {
		taskListStr = this->trim_gen(trim_copy(strTask), '"');
	}

	//Remove spaces:
	std::string::iterator end_pos = std::remove(taskListStr.begin(), taskListStr.end(), ' ');
	taskListStr.erase(end_pos, taskListStr.end());

	//Parse the task list + validate Ids:
	taskList = this->parseTaskListStr(taskListStr);
	if (taskList.empty()) {
		this->printTaskerNotify("Can't tag! The task/s you types are not set or canceled.");
		this->printTaskerInfo("Advice", "You can use `--listall 2` to list all active tasks.");
	}
	//Validate the tag:
	if (this->thestruct["tags"].size() < 1) return false;
	int tagindex = this->findDefinedTag(tag);

	int  counter_tags = 0;
	if (tagindex != -1) {
		for (const auto &taskid : taskList)
		{
			bool addFlag = true;
			json deftags = this->thestruct["tasks"].at(taskid).at("tagged");
			if (!deftags.empty()) {
				for (int i = 0; i < deftags.size(); i++) {
					if (deftags.at(i) == tag) 
						addFlag = false;
				}
			}
			if (addFlag) {
				this->thestruct["tasks"].at(taskid).at("tagged").push_back(tag);
				counter_tags++;
			}
		}
		//print results:
		this->printTaskerNotify("Tagged successfully!");
		this->printTaskerInfo("Info", "Affected: " + std::to_string(counter_tags) + " Tasks.");

	} else {
		this->printTaskerNotify("Tag `" + (TASKER_TAG_PREFIX + tag) + "` is not defined.");
		this->printTaskerInfo("Advice", "You can use `--tags` to list all tags defined.");
	}
	std::cout << std::endl;
	return true;
}
bool TaskerMain::remtag(const std::string& _tag, const std::string& strTask) {
	return false;
}
bool TaskerMain::newtag(const std::string& _tag)
{
	std::string tag = this->trim_gen(trim_copy(_tag), '"');
	std::string desc;

	//Remove spaces:
	std::string::iterator end_pos = std::remove(tag.begin(), tag.end(), ' ');
	tag.erase(end_pos, tag.end());

	//Early validate:
	if (tag.length() < 2) { return false; } /* tag name must be atleast 2 chars long */
	if (std::find(reserve_tag_names.begin(), reserve_tag_names.end(), tag) != reserve_tag_names.end()) {
		return false; /* reserved tag name used */
	}

	//Print main:
	std::cout << std::endl
		<< " > Define new tag: "
		<< this->usecolor() << this->getcolor("tag")
		<< TASKER_TAG_PREFIX << tag
		<< this->usecolor() << this->getcolor("reset")
		<< std::endl;

	//Check not allready set:
	int check = this->findDefinedTag(tag);
	if (check == -1) {

		//Interactively get all needed:
		std::cout << std::endl << "  1. Set tag description (enter for none): ";
		std::getline(std::cin, desc);

		//Save to Object:
		this->thestruct["tags"].push_back({
			{ tag, {
				{ "desc", desc }
			} }
		});

		//print results:
		this->printTaskerNotify("New tag defined!");
		this->printTaskerInfo("Info", "Tag ID is: " + std::to_string(this->thestruct["tags"].size()));

	} else {
		this->printTaskerNotify("Tag `" + (TASKER_TAG_PREFIX + tag) + "` is allready defined!");
		this->printTaskerInfo("Advice", "You can use `--tags` to list all tags defined.");
		this->printTaskerInfo("Advice", "You can use `--updatetag {tagname}` to change tag credentials.");
	}
	return true;
}
bool TaskerMain::deltag(const std::string& _tag)
{
	std::string tag = this->trim_gen(trim_copy(_tag), '"');

	//Print main:
	std::cout << std::endl
		<< " > Deleting a defined tag: "
		<< this->usecolor() << this->getcolor("tag")
		<< TASKER_TAG_PREFIX << tag
		<< this->usecolor() << this->getcolor("reset")
		<< std::endl;

	//validate first:
	if (this->thestruct["tags"].size() < 1) {
		return false;
	}

	//Check if set & delete:
	int  index = this->findDefinedTag(tag);
	int  counter_tags = 0;
	if (index != -1) {

		//Remove tag:
		this->thestruct["tags"].erase(index);
		//Remove from assignments:
		for (unsigned i = 0; i < this->thestruct["tasks"].size(); i++) {
			//Remove from main
			for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("tagged").size(); j++) {
				if (this->thestruct["tasks"].at(i).at("tagged").at(j) == tag) {
					this->thestruct["tasks"].at(i).at("tagged").erase(j);
					counter_tags++;
				}
			}
			
		}
		//print results:
		this->printTaskerNotify("Tag deleted!");
		this->printTaskerInfo("Info", "Affected: " + std::to_string(counter_tags) + " Tasks.");

	}
	else {
		this->printTaskerNotify("Tag `" + (TASKER_TAG_PREFIX + tag) + "` is not defined.");
		this->printTaskerInfo("Advice", "You can use `--tags` to list all tags defined.");
	}
	std::cout << std::endl;
	return true;
}
bool TaskerMain::updatetag(const std::string& _tag) {
	std::string tag = this->trim_gen(trim_copy(_tag), '"');
	std::string desc;
	std::string desc_old;

	//Remove spaces:
	std::string::iterator end_pos = std::remove(tag.begin(), tag.end(), ' ');
	tag.erase(end_pos, tag.end());

	//Early validate:
	if (tag.length() < 2) { return false; } /* tag name must be atleast 2 chars long */
	if (std::find(reserve_tag_names.begin(), reserve_tag_names.end(), tag) != reserve_tag_names.end()) {
		return false; /* reserved tag name used */
	}

	//Print main:
	std::cout << std::endl
		<< " > Update a defined tag: "
		<< this->usecolor() << this->getcolor("tag")
		<< TASKER_TAG_PREFIX << tag
		<< this->usecolor() << this->getcolor("reset")
		<< std::endl;

	//Check not allready set:
	int check = this->findDefinedTag(tag);
	if (check != -1) {

		desc_old = this->trim_gen(this->thestruct["tags"].at(check).at(tag).at("desc"), '"');
		desc_old = this->trim_gen(desc_old, ' ');

		//Interactively get all needed:
		std::cout << std::endl << "  > Tag description: " 
							   << this->usecolor() << this->getcolor("faded")
							   << (desc_old != "" ? desc_old : "Not set") 
							   << this->usecolor() << this->getcolor("reset");

		std::cout << std::endl << "  1. Update tag description (enter for none): ";
		std::getline(std::cin, desc);

		//clean:
		desc = this->trim_gen(trim_copy(desc), '"');

		//Save to Object:
		this->thestruct["tags"].at(check).at(tag).at("desc") = desc;

		//print results:
		this->printTaskerNotify("Tag Updated!");
		this->printTaskerInfo("Info", "Tag ID is: " + std::to_string(check));

	} else {
		this->printTaskerNotify("Tag `" + (TASKER_TAG_PREFIX + tag) + "` is not defined!");
		this->printTaskerInfo("Advice", "You can use `--tags` to list all tags defined.");
		this->printTaskerInfo("Advice", "You can use `--newtag {tagname}` to define a new tag.");
	}
	return true;
}

void TaskerMain::showusers()
{
	//Print main
	std::cout << std::endl << " > Defined users: " << std::endl << std::endl;

	int counter = 0;
	//Iterate:
	for (json::iterator it = this->thestruct["users"].begin(); it != this->thestruct["users"].end(); ++it) {
		for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
			counter++;
			std::string desc = ite.value().at("desc");
			std::string mail = ite.value().at("mail");
			std::cout
				<< "   (" << counter << ") "
				<< this->usecolor() << this->getcolor("user")
				<< TASKER_USER_PREFIX << ite.key()
				<< " -> "
				<< this->usecolor() << this->getcolor("hour")
				<< (desc == "" ? "Description not set" : desc)
				<< " - "
				<< (mail == "" ? "E-mail not set" : mail)
				<< this->usecolor() << this->getcolor("reset")
				<< std::endl;
		}
	}
	if (counter > 0) {
		std::cout << std::endl;
		this->printTaskerInfo("Info", "Total users defined: " + std::to_string(counter));
		this->printTaskerInfo("Info", "Reserved user names: " + this->getReservedUserNames(", ") + ".");
		std::cout << std::endl;
	} else {
		this->printTaskerNotify("No users were defined");
		this->printTaskerInfo("Advice", "You can use `adduser {username}` to define a user.");
	}
}
bool TaskerMain::showstats(const std::string& type)
{
	//Validate type:
	if (type != "tags" && type != "users") {
		return false;
	}
	//Define
	struct statobj {
		float loadunits;
		float loadunitsleft;
		float workunits;
		float workunitsleft;
		statobj() {
			loadunits		= float(0.0);
			loadunitsleft	= float(0.0);
			workunits		= float(0.0);
			workunitsleft	= float(0.0);
		}
	} total;
	std::map<std::string, statobj> container;
	if (type == "users") {
		//Build users:
		for (json::iterator it = this->thestruct["users"].begin(); it != this->thestruct["users"].end(); ++it) {
			for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
				container.insert(std::pair<std::string, statobj>(ite.key(), statobj()));
			}
		}
		//Add not assigned
		container.insert(std::pair<std::string, statobj>("not assigned", statobj()));
	} else {
		//Build tags:
		for (json::iterator it = this->thestruct["tags"].begin(); it != this->thestruct["tags"].end(); ++it) {
			for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
				container.insert(std::pair<std::string, statobj>(ite.key(), statobj()));
			}
		}
		//Add not tagged
		container.insert(std::pair<std::string, statobj>("not tagged", statobj()));
	}

	//Calculate and collect:
	for (unsigned i = 0; i < this->thestruct["tasks"].size(); i++) {
		//Skip cancel:
		if (this->thestruct["tasks"].at(i).at("cancel")) continue;
		//Calculate this task:
		float _workunitsleft = (float(1.0) - (float)this->thestruct["tasks"].at(i).at("status")) * float(100.0);
		float _loadbase		 = (float)this->thestruct["tasks"].at(i).at("load");
		float _loadunitsleft = _loadbase * (float(1.0) - (float)this->thestruct["tasks"].at(i).at("status"));
		//Set total:
		total.workunits += 100;
		total.workunitsleft += _workunitsleft;
		total.loadunits		+= _loadbase;
		total.loadunitsleft += _loadunitsleft;

		//Set object:
		if (type == "users") {
			//Get assigned:
			std::string user = this->thestruct["tasks"].at(i).at("plan").back().at("user");
			user = (user == "") ? "not assigned" : user;
			container[user].loadunits += _loadbase;
			container[user].loadunitsleft += _loadunitsleft;
			container[user].workunitsleft += _workunitsleft;
			container[user].workunits += 100;
		} else {
			//Get tagged:
			unsigned int tagged_size = (unsigned int)this->thestruct["tasks"].at(i).at("tagged").size();
			if (tagged_size > 0) {
				for (unsigned int j = 0; j < tagged_size; j++) {
					std::string _tag = this->thestruct["tasks"].at(i).at("tagged").at(j);
					if (this->findDefinedTag(_tag) != -1) {
						container[_tag].loadunits += _loadbase;
						container[_tag].loadunitsleft += _loadunitsleft;
						container[_tag].workunitsleft += _workunitsleft;
						container[_tag].workunits += 100;
					}
				}
			} else {
				container["not tagged"].loadunits += _loadbase;
				container["not tagged"].loadunitsleft += _loadunitsleft;
				container["not tagged"].workunitsleft += _workunitsleft;
				container["not tagged"].workunits += 100;
			}
		}
	}

	//Print main
	std::cout << std::endl << " > Tasker stats: " << std::endl << std::endl;

	//Print out the stats:
	if (total.workunits > 0) {
		
		//Set percision:
		std::cout << std::setprecision(2) << std::fixed;
		
		//Set the prefix:
		std::string prefix_name = (type == "tags" ? TASKER_TAG_PREFIX : TASKER_USER_PREFIX);
		std::string color_name = (type == "tags" ? "tag" : "user");
		//Print by users or tags:
		for (auto const& x : container)
		{
			float percwork   = (x.second.workunitsleft > 0) ? (float(1.0) - (x.second.workunitsleft / x.second.workunits)) : float(-1.0);
			float loadcalc = (x.second.loadunitsleft > 0) ? (x.second.loadunitsleft / total.loadunitsleft) : float(0.0);
			std::string workbar = "";
			std::string loadbar = "";

			float barprogress = (percwork > float(-1.0)) ? percwork : float(0.0);

			//Work Progress bar:
			int barWidth = TASKER_BAR_LENGTH;
			int workpos = int(barWidth * percwork);
			for (int i = 0; i < barWidth; ++i) {
				if (i < workpos) workbar += TASKER_BAR_FULL;
				else if (i == workpos) workbar += TASKER_BAR_ARROW;
				else workbar += TASKER_BAR_EMPTY;
			}

			//Work Progress bar:
			int loadpos = int(barWidth * loadcalc);
			for (int i = 0; i < barWidth; ++i) {
				if (i < loadpos) loadbar += TASKER_BAR_FULL;
				else if (i == loadpos) {
					loadbar += TASKER_BAR_CURSSOR;
				}
				else loadbar += TASKER_BAR_EMPTY;
			}

			//Load bar:
			std::cout
				<< "   - "
				<< this->usecolor() << this->getcolor(color_name)
				<< prefix_name << x.first
				<< ":" << std::endl
				<< this->usecolor() << this->getcolor("hour")
				<< "    \t -> Progress     : "
				<< TASKER_BAR_OPEN;
			std::cout
				<< this->usecolor() << this->getcolor("workbar", percwork)
				<< workbar;
			std::cout
				<< this->usecolor() << this->getcolor("hour")
				<< TASKER_BAR_CLOSE
				<< " " << ((percwork > -1.0) ? std::to_string(int(percwork * 100.0)) + "%" : "Empty") << std::endl
				<< "    \t -> Work Load    : "
				<< TASKER_BAR_OPEN;
			std::cout
				<< this->usecolor() << this->getcolor("loadbar", loadcalc)
				<< loadbar;
			std::cout
				<< this->usecolor() << this->getcolor("hour")
				<< TASKER_BAR_CLOSE
				<< " " << ((loadcalc > 0) ? std::to_string(int(loadcalc * 100.0)) + "%" : "Empty")
				<< ", " << x.second.loadunitsleft << std::endl
				<< this->usecolor() << this->getcolor("reset") << std::endl;
		}
		
	} else {

		//Print no stats:
		this->printTaskerNotify("Tasker is empty. Add some tasks.");
		std::cout << std::endl;
		return true;
	}

	//Notify:
	this->printTaskerNotify("Stats calculated successfully!");
	std::cout << std::endl;

	return true;
}
bool TaskerMain::adduser(const std::string& _user)
{
	std::string user = trim_gen(trim_copy(_user), '"');
	std::string desc;
	std::string mail;

	//Remove spaces:
	std::string::iterator end_pos = std::remove(user.begin(), user.end(), ' ');
	user.erase(end_pos, user.end());

	//Early validate:
	if (user.length() < 2) { return false; } /* name must be atleast 2 chars long */
	if (std::find(reserve_user_names.begin(), reserve_user_names.end(), user) != reserve_user_names.end()) {
		return false; /* reserved name used */
	}

	//Print main:
	std::cout << std::endl 
			  << " > Define user: " 
			  << this->usecolor() << this->getcolor("user")
			  << user 
			  << this->usecolor() << this->getcolor("reset")
			  << std::endl;

	//Check not allready set:
	int check = this->findDefinedUser(user); 
	if (check == -1) {

		//Interactively get all needed:
		std::cout << std::endl << "  1. Set user description (enter for none): ";
		std::getline(std::cin, desc);
		std::cout << "  2. Set user e-mail address (enter for none): ";
		std::getline(std::cin, mail);

		//Save to Object:
		this->thestruct["users"].push_back({
			{ user, {
				{"desc", desc },
				{"mail", mail }
			}}
		});

		//print results:
		this->printTaskerNotify("New user defined!");
		this->printTaskerInfo("Info", "User ID is: " + std::to_string(this->thestruct["users"].size()));

	} else {
		this->printTaskerNotify("User `" + user + "` is allready defined!");
		this->printTaskerInfo("Advice", "You can use `users` to list all users defined.");
		this->printTaskerInfo("Advice", "You can use `updateuser {username}` to change user credentials.");
	}
	return true;
}
bool TaskerMain::deluser(const std::string& _user)
{

	std::string user = trim_gen(trim_copy(_user), '"');

	//Print main:
	std::cout << std::endl
		<< " > Deleting user: "
		<< this->usecolor() << this->getcolor("user")
		<< user
		<< this->usecolor() << this->getcolor("reset")
		<< std::endl;

	//validate first:
	if (this->thestruct["users"].size() < 2) {
		return false;
	}

	//Check if set & delete:
	int  index = this->findDefinedUser(user);
	int  counter_tasks = 0;
	int  counter_reports = 0;
	if (index != -1) {

		//Remove user:
		this->thestruct["users"].erase(index);
		//Remove from assignments:
		for (unsigned i = 0; i < this->thestruct["tasks"].size(); i++) {
			//Remove from main
			for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("plan").size(); j++) {
				if (this->thestruct["tasks"].at(i).at("plan").at(j).at("user") == user) {
					this->thestruct["tasks"].at(i).at("plan").at(j).at("user") = "";
					counter_tasks++;
				}
			}
			//Remove from reports:
			for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("report").size(); j++) {
				if (this->thestruct["tasks"].at(i).at("report").at(j).at("by") == user) {
					this->thestruct["tasks"].at(i).at("report").at(j).at("by") = "";
					counter_reports++;
				}
			}
		}
		//print results:
		this->printTaskerNotify("User deleted!");
		this->printTaskerInfo("Info", "Affected: " + std::to_string(counter_tasks) + " Tasks, " + std::to_string(counter_reports) + " reports.");

	} else {
		this->printTaskerNotify("User `" + user + "` is not defined.");
		this->printTaskerInfo("Advice", "You can use `users` to list all users defined.");
	}
	std::cout << std::endl;
	return true;
}
bool TaskerMain::updateuser(const std::string& _user)
{
	std::string user = trim_gen(trim_copy(_user), '"');
	std::string desc;
	std::string mail;
	//Print main:
	std::cout << std::endl
		<< " > Updating user: "
		<< this->usecolor() << this->getcolor("user")
		<< user
		<< this->usecolor() << this->getcolor("reset")
		<< std::endl
		<< "   - "
		<< this->usecolor() << this->getcolor("hour")
		<<"Press ENTER for skip, type `empty` to truncate."
		<< this->usecolor() << this->getcolor("reset")
		<< std::endl;

	//Check if set & update:
	int index = this->findDefinedUser(user);
	if (index != -1) {
		//Update:
		std::cout << std::endl << "  1. Set user description: ";
		std::getline(std::cin, desc);
		std::cout << "  2. Set user e-mail address: ";
		std::getline(std::cin, mail);

		//Trim
		desc = trim_gen(trim_copy(desc), '"');
		mail = trim_gen(trim_copy(mail), '"');

		//Save to Object:
		if (desc == "empty") {
			this->thestruct["users"].at(index).at(user).at("desc") = "";
		} else if (desc == "" || desc == "\"\"") {
			//Do nothing
		} else {
			this->thestruct["users"].at(index).at(user).at("desc") = desc;
		}
		if (mail == "empty") {
			this->thestruct["users"].at(index).at(user).at("mail") = "";
		} else if (mail == "" || mail == "\"\"") {
			//Do nothing
		} else {
			this->thestruct["users"].at(index).at(user).at("mail") = mail;
		}

		//print results:
		this->printTaskerNotify("User updated successfully!");
		std::cout << std::endl;

	} else {
		return false;
	}
	return true;
}

bool TaskerMain::list(const std::string& level, const std::string& which) {
	return this->list(level, which, "");
}
bool TaskerMain::list(const std::string& level, const std::string& which, const std::string& _filter) {

	int theLevel = -1;
	std::vector<std::string> users;
	std::vector<std::string> tags;
	std::string filter = trim_copy(_filter);
	int counter_found = 0;

	//Validate the index: 
	try { theLevel = stoi(level); }
	catch (...) { return false; }
	if (theLevel < 1) return false;

	//Print main
	std::cout << std::endl << " > Listing matched tasks: " << std::endl;

	//Before start check if we want to parse a filter first:
	if (which == "user") {
		std::string theusersstr = filter;
		std::string deli = TASKER_SPLIT_DELI;
		size_t pos = 0;
		std::string token;
		while ((pos = theusersstr.find(deli)) != std::string::npos) {
			token = theusersstr.substr(0, pos);
			theusersstr.erase(0, pos + deli.length());
			users.push_back(token);
		}
		if (users.size() == 0 || theusersstr.length() > 0)
			users.push_back(theusersstr);
	} else if (which == "tag") {
		std::string thetagsstr = filter;
		std::string deli = TASKER_SPLIT_DELI;
		size_t pos = 0;
		std::string token;
		while ((pos = thetagsstr.find(deli)) != std::string::npos) {
			token = thetagsstr.substr(0, pos);
			thetagsstr.erase(0, pos + deli.length());
			tags.push_back(token);
		}
		if (tags.size() == 0 || thetagsstr.length() > 0)
			tags.push_back(thetagsstr);
	}
	//Print tasks
	for (unsigned i = 0; i < this->thestruct["tasks"].size(); i++) {

		//Selective print:
		if (which != "cancel" && this->thestruct["tasks"].at(i).at("cancel") == true) {
			continue;
		}
		if (which == "cancel" && this->thestruct["tasks"].at(i).at("cancel") != true) {
			continue;
		}
		if (which == "done" && (float)this->thestruct["tasks"].at(i).at("status") < 1.00) {
			continue;
		}
		if (which == "open" && (float)this->thestruct["tasks"].at(i).at("status") > 0.99) {
			continue;
		}
		if (which == "today") {
			std::string duedate = this->thestruct["tasks"].at(i).at("plan").back().at("date");
			if (duedate == "" || duedate == "\"\"") {
				continue;
			} else {
				std::string todayDate = this->getcurdatetime("%d-%m-%Y");
				duedate = duedate.substr(0, 10);
				if (duedate != todayDate) {
					continue;
				}
			}
		}
		if (which == "user") {
			std::string theuser = this->thestruct["tasks"].at(i).at("plan").back().at("user");
			if (std::find(users.begin(), users.end(), theuser) != users.end()) {

			} else {
				continue;
			}
		}
		if (which == "tag") {
			bool tagtest = false;
			for (json::iterator it = this->thestruct["tasks"].at(i).at("tagged").begin(); it != this->thestruct["tasks"].at(i).at("tagged").end(); ++it) {
				std::stringstream t;
				std::string ts;
				t << it.value();
				ts = t.str();
				ts.erase(std::remove(ts.begin(), ts.end(), '"'), ts.end());
				if (std::find(tags.begin(), tags.end(), ts) != tags.end())
					tagtest = true;
			}
			if (!tagtest) {
				continue;
			}
		}
		//Print main row:
		counter_found++;
		std::string user = this->thestruct["tasks"].at(i).at("plan").back().at("user");
		std::string target = this->thestruct["tasks"].at(i).at("plan").back().at("date");
		std::string created = this->thestruct["tasks"].at(i).at("created");
		std::stringstream tagged;
		std::string tagged_str;
		for (json::iterator it = this->thestruct["tasks"].at(i).at("tagged").begin(); it != this->thestruct["tasks"].at(i).at("tagged").end(); ++it) {
			tagged << TASKER_TAG_PREFIX << it.value();
			if (std::next(it) != this->thestruct["tasks"].at(i).at("tagged").end()) // last element
				tagged << ", ";
		}
		tagged_str = tagged.str();
		tagged_str.erase(remove(tagged_str.begin(), tagged_str.end(), '\"'), tagged_str.end());

		if (user == "" || user == "\"\"") {
			user = "not assigned";
		}
		else {
			user = TASKER_USER_PREFIX + user;
		}
		if (target == "" || target == "\"\"") {
			target = "not set";
		}
		user.erase(std::remove(user.begin(), user.end(), '"'), user.end());
		target.erase(std::remove(target.begin(), target.end(), '"'), target.end());
		created.erase(std::remove(created.begin(), created.end(), '"'), created.end());

		//Group tags:
		std::cout
			<< " ("
			<< (i + 1)
			<< ") : ["
			<< this->usecolor() << this->getcolor("status", (float)this->thestruct["tasks"].at(i).at("status"))
			<< std::to_string(
					(int)((float)this->thestruct["tasks"].at(i).at("status") * 100)
				).substr(0, 3)
			<< "%"
			<< this->usecolor() << this->getcolor("reset")
			<< "] -> "
			<< this->usecolor() << ((which == "cancel") ? this->getcolor("error") : this->getcolor("reset"))
			<< ((which == "cancel") ? "CANCELED / " : "")
			<< this->usecolor() << this->getcolor("reset")
			<< this->thestruct["tasks"].at(i).at("task") 
			<< std::endl
			<< this->usecolor() << this->getcolor("faded")
			<< "\t* Due Date: "
			<< this->usecolor() << this->getcolor("target")
			<< target
			<< this->usecolor() << this->getcolor("faded")
			<< " , Created: "
			<< this->usecolor() << this->getcolor("hour")
			<< created
			<< this->usecolor() << this->getcolor("faded")
			<< " , Assigned To: "
			<< this->usecolor() << this->getcolor("user")
			<< user
			<< this->usecolor() << this->getcolor("reset")
			<< std::endl
			<< this->usecolor() << this->getcolor("faded")
			<< "\t* Tags: "
			<< this->usecolor() << this->getcolor("tag")
			<< (tagged_str.size() > 1 ? tagged_str : "not tagged")
			<< this->usecolor() << this->getcolor("reset")
			<< std::endl;

		if (theLevel == 2) {
			//Will print task notes
			for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("report").size(); j++) {

				//Prepare for print:
				std::string byuser = this->thestruct["tasks"].at(i).at("report").at(j).at("by");
				byuser.erase(std::remove(byuser.begin(), byuser.end(), '"'), byuser.end());

				std::string dateout = this->thestruct["tasks"].at(i).at("report").at(j).at("date");
				dateout.erase(std::remove(dateout.begin(), dateout.end(), '"'), dateout.end());
				
				std::string onlydate = dateout.substr(0, 10);
				std::string onlyhour = dateout.substr(11, 8);
				//Print report:
				std::cout
					<< "\t- "
					<< "("
					<< (i+1) << "." << (j+1)
					<< ")"
					<< " : ["
					<< this->usecolor() << this->getcolor("status", (float)this->thestruct["tasks"].at(i).at("report").at(j).at("status"))
					<< std::to_string(
							(int)((float)this->thestruct["tasks"].at(i).at("report").at(j).at("status") * 100)
						).substr(0, 3)
					<< "%"
					<< this->usecolor() << this->getcolor("reset")
					<< "] : "
					<< this->usecolor() << this->getcolor("hour")
					<< onlydate << " " << onlyhour
					<< this->usecolor() << this->getcolor("reset")
					<< " -> "
					<< this->thestruct["tasks"].at(i).at("report").at(j).at("note")
					<< ", by "
					<< this->usecolor() << this->getcolor("user")
					<< (byuser == "" ? "unknown" : (TASKER_USER_PREFIX + byuser) )
					<< this->usecolor() << this->getcolor("reset")
					<< std::endl;
			}
			std::cout << std::endl;
		}
	}
	//In case nothing was found:
	if (counter_found == 0) {
		this->printTaskerNotify("No matching tasks found!");
		if (which == "user") {
			this->printTaskerInfo("Advice", "Make sure the user name is correct. run `users` to see all defined users.");
		}
		std::cout << std::endl;
	}
	return true;
}

TaskerMain::~TaskerMain()
{

}

}