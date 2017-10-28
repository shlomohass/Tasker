//
//  Tasker.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#include "TaskerMain.hpp"
#include "SETTASKER.hpp"

#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <locale>

#ifdef LINUX
	#include <string>
	#include <limits.h>
	#include <unistd.h>
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
	#ifdef LINUX
		char buffer[PATH_MAX];
		if (getcwd(buffer, sizeof(buffer))) {
			this->fullpath = std::string(buffer) + "\\" + TASKER_OBJNAME;
		}
	#else
		char buffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buffer);
		this->basepath = std::string(buffer);
		this->fullpath = std::string(buffer) + "\\" + TASKER_OBJNAME;
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
	#ifdef LINUX
		if (_access(this->fullpath.c_str(), F_OK) == 0) {
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
void TaskerMain::createEmpty()
{	
	//Sets :
	this->thestruct["tasker"] = {{"version", TASKER_VERSION }};
	this->thestruct["users"] = json::array();
	this->thestruct["types"] = {{"task",{"desc","simple task"}}};
	this->thestruct["tasks"] = json::array();
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
	this->thestruct["name"] = projName;
	this->thestruct["desc"] = projDesc;
	this->thestruct["version"] = projVersion;
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
	#ifdef LINUX
		if (access(path.c_str(), W_OK) == 0 || access(path.c_str(), RW_OK) == 0)
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
	localtime_s(&timeinfo, &rawtime);
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

bool TaskerMain::setNewTask(const std::string& strTask)
{	
	std::string plan_user	 = "";
	std::string plan_currentversion = this->thestruct["version"];
	std::string plan_version = "";
	std::string plan_duedate = "";
	std::string task_created = this->getcurdatetime();
	std::string task_status  = "";
	float		task_status_num;
	bool        reloop_user	 = true;
	bool        reloop_date  = true;
	bool        show_advice_user = true;

	//Interactively get all needed:
	std::cout << std::endl << " > New task: ";
	std::cout << std::endl << "  1. Assign to user (empty for none): ";
	while (reloop_user) {
		std::getline(std::cin, plan_user);
		plan_user = trim_copy(plan_user);
		std::string::iterator end_pos = std::remove(plan_user.begin(), plan_user.end(), ' ');
		plan_user.erase(end_pos, plan_user.end());
		if (plan_user == "default") {
			plan_user = this->getDefindUserName(0);
			reloop_user = false;
			break;
		} else if (plan_user == "") {
			plan_user = "";
			reloop_user = false;
			break;
		} else if (this->findDefinedUser(plan_user) == -1) {
			this->printTaskerInfo("Error", "The user name you typed can't be found.");
			if (show_advice_user) {
				this->printTaskerInfo("Advice", "Leave empty and press ENTER for not assigned.");
				this->printTaskerInfo("Advice", "Type `default` and press ENTER for auto assign default user.");
				this->printTaskerInfo("Advice", "Run `--users` to see all users defined.");
				show_advice_user = false;
			}
			std::cout << "\tType: ";
		} else {
			reloop_user = false;
		}
	}
	std::cout << "  2. Planned for version (empty for current): ";
	std::getline(std::cin, plan_version);
	std::cout << "  3. Due date `d-m-Y H:M:S` (empty for none or `today`): ";
	while (reloop_date) {
		std::getline(std::cin, plan_duedate);
		if (plan_duedate == "") {
			break;
		} else if (plan_duedate == "today") {
			plan_duedate = this->getcurdatetime("%d-%m-%Y 23:59:00");
			break;
		} else {
			int day = -1, mon = -1, year = -1, hour = 0, min = 0, sec = 0;
			int found = this->parseDateParts(plan_duedate, day, mon, year, hour, min, sec);
			if (found == 3) {
				if (year < 1000 || day > 31 || day < 1 || mon > 12 || mon < 1) {
					this->printTaskerInfo("Error", "Please retry. Use the correct format: `d-m-Y H:M:S` or `d-m-Y`.");
					std::cout << "\tType: ";
				}
				else {
					plan_duedate = this->createDateFromInts(day, mon, year, 0, 0, 0);
					reloop_date = false;
				}
			}
			else if (found == 6) {
				if (year < 1000 || day > 31 || day < 1 || mon > 12 || mon < 1
					|| hour > 24 || hour < 0 || min < 0 || min > 60 || sec < 0 || sec > 60
					) {
					this->printTaskerInfo("Error", "Please retry. Use the correct format: `d-m-Y H:M:S` or `d-m-Y`.");
					std::cout << "\tType: ";
				}
				else {
					plan_duedate = this->createDateFromInts(day, mon, year, hour, min, sec);
					reloop_date = false;
					
				}
			} else {
				this->printTaskerInfo("Error", "Please retry. Use the correct format: `d-m-Y H:M:S` or `d-m-Y`.");
				std::cout << "\tType: ";
			}
		}
		
	}
	
	std::cout << "  4. Set current status (1|0, true|false): ";
	std::getline(std::cin, task_status);

	//Normalize:
	plan_version = plan_version != "" ? plan_version : plan_currentversion;
	task_status_num = this->normalizeStatus(task_status);

	//Create finall Object:
	json taskObj = {
		{ "plan",		""					},
		{ "created",	task_created		},
		{ "task",		trim_copy(strTask)	},
		{ "status",		task_status_num		},
		{ "report",		""					} 
	};
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

	std::string rep_date = this->getcurdatetime();
	std::string rep_user = "";
	std::string rep_note = "";
	std::string rep_status = "";
	float		rep_status_num;
	bool		reloop_user = true;
	bool		reloop_note = true;
	bool        show_advice_user = true;
	
	//Interactively get all needed:
	std::cout << std::endl << "> Report to task: " << strTask;
	std::cout << std::endl << "  1. Set new progress status (0.0 - 1.0): ";
	std::getline(std::cin, rep_status);
	//Get the user who made the report:
	std::cout << "  2. Progress of user: ";
	while (reloop_user) {
		std::getline(std::cin, rep_user);

		rep_user = trim_copy(rep_user);
		std::string::iterator end_pos = std::remove(rep_user.begin(), rep_user.end(), ' ');
		rep_user.erase(end_pos, rep_user.end());
		
		if (rep_user == "default") {
			rep_user = this->getDefindUserName(0);
			reloop_user = false;
			break;
		}
		else if (rep_user == "") {
			rep_user = "";
			reloop_user = false;
			break;
		}
		else if (this->findDefinedUser(rep_user) == -1) {
			this->printTaskerInfo("Error", "The user name you typed can't be found.");
			if (show_advice_user) {
				this->printTaskerInfo("Advice", "Leave empty and press ENTER for not assigned.");
				this->printTaskerInfo("Advice", "Type `default` and press ENTER for setting the default user.");
				this->printTaskerInfo("Advice", "Run `--users` to see all users defined.");
				show_advice_user = false;
			}
			std::cout << "\tType: ";
		} else {
			reloop_user = false;
		}
	}
	//Get the report note:
	std::cout << "  3. Type task report note: ";
	while (reloop_note) {
		std::getline(std::cin, rep_note);
		rep_note = trim_copy(rep_note);
		if (rep_note == "") {
			this->printTaskerInfo("Error", "Please retry. You must add a report note.");
			std::cout << "\tType: ";
		} else {
			reloop_note = false;
		}
	}

	//Normalize status:
	rep_status_num = this->normalizeStatus(rep_status);

	//Push to plan notes:
	this->thestruct["tasks"].at(theTask).at("report").push_back({
		{"date",		rep_date			},
		{"status",		rep_status_num		},
		{"note",		trim_copy(rep_note) }, 
		{"by",			trim_copy(rep_user) }
	});

	//Update parent status:
	this->thestruct["tasks"].at(theTask).at("status") = rep_status_num;

	this->printTaskerNotify("Reported and saved successfully! ");
	this->printTaskerInfo("Info", "Reported to Task Id: " + strTask);

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
				<< ite.key()
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
		this->printTaskerInfo("Info", "Total users defined : " + std::to_string(counter));
		std::cout << std::endl;
	} else {
		this->printTaskerNotify("No users were defined");
		this->printTaskerInfo("Advice", "You can use `adduser {username}` to define a user name.");
	}
}
bool TaskerMain::adduser(const std::string& _user)
{
	std::string user = trim_copy(_user);
	std::string desc;
	std::string mail;

	//Remove spaces:
	std::string::iterator end_pos = std::remove(user.begin(), user.end(), ' ');
	user.erase(end_pos, user.end());

	//Early validate:
	if (user.length() < 2) {
		return false;
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

	std::string user = trim_copy(_user);

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

	if (index != -1) {
		//Remove user:
		this->thestruct["users"].erase(index);
		//Remove from assignments:
		for (unsigned i = 0; i < this->thestruct["tasks"].size(); i++) {
			for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("plan").size(); j++) {
				if (this->thestruct["tasks"].at(i).at("plan").at(j).at("user") == user) {
					this->thestruct["tasks"].at(i).at("plan").at(j).at("user") = "";
					counter_tasks++;
				}
			}
		}
		//print results:
		this->printTaskerNotify("User deleted!");
		this->printTaskerInfo("Info", "Affected: " + std::to_string(counter_tasks) + " Tasks.");

	} else {
		this->printTaskerNotify("User `" + user + "` is not defined.");
		this->printTaskerInfo("Advice", "You can use `users` to list all users defined.");
	}
	return true;
}
bool TaskerMain::updateuser(const std::string& _user)
{
	std::string user = trim_copy(_user);
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
		desc = trim_copy(desc);
		mail = trim_copy(mail);

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
	}
	//Print tasks
	for (unsigned i = 0; i < this->thestruct["tasks"].size(); i++) {

		//Selective print:
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
		//Print main row:
		counter_found++;
		std::string user = this->thestruct["tasks"].at(i).at("plan").back().at("user");
		std::string target = this->thestruct["tasks"].at(i).at("plan").back().at("date");
		std::string created = this->thestruct["tasks"].at(i).at("created");
		if (user == "" || user == "\"\"") {
			user = "not assigned";
		}
		if (target == "" || target == "\"\"") {
			target = "not set";
		}
		user.erase(std::remove(user.begin(), user.end(), '"'), user.end());
		target.erase(std::remove(target.begin(), target.end(), '"'), target.end());
		created.erase(std::remove(created.begin(), created.end(), '"'), created.end());

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
			<< std::endl;

		if (theLevel == 2) {
			//Will print task notes
			for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("report").size(); j++) {

				std::string dateout = this->thestruct["tasks"].at(i).at("report").at(j).at("date");
				dateout.erase(std::remove(dateout.begin(), dateout.end(), '"'), dateout.end());
				std::string onlydate = dateout.substr(0, 10);
				std::string onlyhour = dateout.substr(11, 8);
				std::cout
					<< "\t- "
					<< "["
					<< this->usecolor() << this->getcolor("status", (float)this->thestruct["tasks"].at(i).at("report").at(j).at("status"))
					<< std::to_string(
					(int)((float)this->thestruct["tasks"].at(i).at("report").at(j).at("status") * 100)
					).substr(0, 3)
					<< "%"
					<< this->usecolor() << this->getcolor("reset")
					<< "] : "
					<< this->usecolor() << this->getcolor("hour")
					<< onlydate
					<< this->usecolor() << this->getcolor("reset")
					<< " -> "
					<< this->thestruct["tasks"].at(i).at("report").at(j).at("note")
					<< " : "
					<< this->usecolor() << this->getcolor("hour")
					<< onlyhour
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