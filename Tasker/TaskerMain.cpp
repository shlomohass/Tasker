//
//  Tasker.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#include "TaskerMain.hpp"

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

TaskerMain::TaskerMain(tasker::moreOpt moreopt)
{
	this->opt = moreopt;
	this->basepath = "notset";
	this->fullpath = "notset";
}

//Object handling:
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
		this->opt.use_colors = TaskerBase::thestruct["tasker"].at("usecolors");
	}
	//Set load modes:
	this->opt.enable_loads = TaskerBase::thestruct["tasker"].at("enableloads");

	//Set delete flag:
	this->opt.del_items = TaskerBase::thestruct["tasker"].at("allowdelete");
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
		TaskerBase::thestruct["tasker"].at("usecolors") = boolstate;

	} else if (which == "optdelete") {
		TaskerBase::thestruct["tasker"].at("allowdelete") = boolstate;

	} else if (which == "optloads") {
		TaskerBase::thestruct["tasker"].at("enableloads") = boolstate;

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
	TaskerBase::thestruct["tasker"]		= this->getBaseSystemContainer();
	TaskerBase::thestruct["users"]		= json::array();
	TaskerBase::thestruct["types"]		= this->getBaseTypesContainer();
	TaskerBase::thestruct["tasks"]		= json::array();
	TaskerBase::thestruct["tags"]		= json::array();
	TaskerBase::thestruct["note"]		= json::array();
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
	TaskerBase::thestruct["name"]		= projName;
	TaskerBase::thestruct["desc"]		= projDesc;
	TaskerBase::thestruct["version"]	= json::array({ projVersion });
	TaskerBase::thestruct["users"].push_back({ { userName, {{"desc",userDesc },{"mail",userEmail }}} });
}
void TaskerMain::createEmpty(json structure) 
{
	TaskerBase::thestruct = structure;
}
std::string TaskerMain::getSerialized()
{
	return TaskerBase::thestruct.dump();
}
std::string TaskerMain::getSerialized(int pretty_spaces)
{
	return TaskerBase::thestruct.dump(pretty_spaces);
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
int TaskerMain::loadObj(std::string& version)
{
	if (this->checkWriteObj(true)) {
		std::ifstream ifs(this->fullpath);
		std::string content((std::istreambuf_iterator<char>(ifs)),
							(std::istreambuf_iterator<char>()));
		ifs.close();
		TaskerBase::thestruct = json::parse(content.c_str());

		//Check version:
		if (TaskerBase::thestruct["tasker"]["version"] == TASKER_VERSION) {
			return 0;
		} else {
			version = TaskerBase::thestruct["tasker"]["version"];
			return 2;
		}
	}
	return 1; 
	// 0 ok run
	// 1 no permission.
	// 2 version 
}

//Tasker Operations:
bool TaskerMain::setNewTask(const std::string& strTask)
{	
	std::vector<std::string> plan_user;
	std::vector<std::string> tagged_as;
	std::string plan_version			= "";
	std::string plan_duedate			= "";
	std::string task_created			= this->getcurdatetime();
	std::string task_status				= "";
	int			loadint					= 1;
	bool        push_plan				= false;
	float		task_status_num;

	//Interactively get all needed:
	std::cout << std::endl << " > New task: ";

	//Assign a user name:
	std::cout << std::endl << "  1. Assign to user/s (empty, ?, default): ";
	plan_user = this->getUserName(push_plan, false, 0, "");

	//Tag the task:
	std::cout << "  2. Tag the task (empty, ?): ";
	tagged_as = this->getTags("The Tag you typed can't be found.");

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
	if (this->opt.enable_loads) {
		std::cout << "  6. Set load units of this task (use an integer): ";
		loadint = this->getLoad("Bad input - Please enter a positive integer only.");
	}

	//Set plan:
	json::object_t plan_object = this->getBaseTaskPlan(plan_duedate, trim_copy(plan_version));
	plan_object["user"] = plan_user;

	//Create finall Object:
	json taskObj = {
		{ "plan",		json::array({ plan_object })},
		{ "created",	task_created				},
		{ "updated",	task_created				},
		{ "task",		this->trim_gen(trim_copy(strTask), '"')	},
		{ "status",		task_status_num							},
		{ "cancel",		false									},
		{ "load",		loadint									},
		{ "tagged",		tagged_as								},
		{ "report",		json::array()							}
	};

	//Save
	TaskerBase::thestruct["tasks"].push_back(taskObj);

	//Notify:
	this->printTaskerNotify("New task Created!");
	this->printTaskerInfo("Info","Task ID is: " + std::to_string(TaskerBase::thestruct["tasks"].size()));
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
	if ((int)TaskerBase::thestruct["tasks"].size() <= theTask) { return false; }

	//Does the task enabled:
	if (TaskerBase::thestruct["tasks"].at(theTask).at("cancel") == true) { return false; }

	std::string rep_date			= this->getcurdatetime();
	std::vector<std::string>		rep_user;
	std::string rep_note			= "";
	std::string rep_status			= "";
	std::vector<std::string> owner	= TaskerBase::thestruct["tasks"].at(theTask).at("plan").back().at("user");
	float		rep_status_num;
	bool		push_plan	= true;
	
	//Set percision:
	std::cout << std::setprecision(2) << std::fixed;

	//Interactively get all needed:
	std::cout << std::endl << " > Report to task: " << strTask;

	//Set an updated progress:
	std::cout << std::endl << "  1. Set new progress status (0.0 - 1.0, current "; 
	std::cout << usecolor() << getcolor("workbar");
	std::cout << (float)TaskerBase::thestruct["tasks"].at(theTask).at("status");
	std::cout << usecolor() << getcolor("reset");
	std::cout << "): ";
	std::getline(std::cin, rep_status);

	//Normalize progress status:
	rep_status_num = this->normalizeStatus(rep_status);

	//prep user string to show:
	std::string users_str = this->getAssignedUserString(owner, TASKER_USER_PREFIX, true);

	//Get the user who made the report:
	std::cout << "  2. Progress of user (assigned to "; 
	std::cout << usecolor() << getcolor("user");
	std::cout << users_str;
	std::cout << usecolor() << getcolor("reset");
	std::cout << " - empty, ?, default" << "): ";
	rep_user = this->getUserName(push_plan, false, 0, "");
	
	//Get the report note:
	std::cout << "  3. Task progress report note: ";
	rep_note = this->getStrMessage("Please retry. You must add a report note.");

	//Push to plan notes:
	TaskerBase::thestruct["tasks"].at(theTask).at("report").push_back({
		{"date",		this->trim_gen(rep_date, '"')			 },
		{"status",		rep_status_num							 },
		{"note",		this->trim_gen(trim_copy(rep_note), '"') },
		{"by",			rep_user }
	});

	//Update parent status:
	TaskerBase::thestruct["tasks"].at(theTask).at("status") = rep_status_num;

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
		std::string new_task_title			= "";
		std::vector<std::string> plan_user;
		std::string plan_version			= "";
		std::string plan_duedate			= "";
		std::string task_updated			= this->getcurdatetime();
		std::string task_load				= "1";
		int			loadint					= 1;
		bool        push_plan				= false;

		//Interactively update all needed:
		std::cout << std::endl << " > Refactor Task: ";
		std::cout << usecolor() << getcolor("faded") 
				  << TaskerBase::thestruct["tasks"].at(theRow.taskId).at("task")
				  << usecolor() << getcolor("reset");

		//Set new title:
		std::cout << std::endl << "  1. Set New task title (empty for skip): ";
		std::getline(std::cin, new_task_title);
		new_task_title = (new_task_title == "") ? 
			TaskerBase::thestruct["tasks"].at(theRow.taskId).at("task").get<std::string>() :
			this->trim_gen(trim_copy(new_task_title), '"');

		//Change assigned user:
		std::cout << "  2. Change user (skip|empty, clear, default, ?): ";
		plan_user = this->getUserName(push_plan, true, theRow.taskId, "");

		//Update the planned version:
		std::cout << "  3. Update planned for version (empty = current | 'skip' = skip): ";\
		plan_version = this->getStrVersion(
			push_plan, true,
			TaskerBase::thestruct["tasks"].at(theRow.taskId).at("plan").back().at("v").get<std::string>()
		);

		//Update Due date:
		std::string prevDateSet = TaskerBase::thestruct["tasks"].at(theRow.taskId).at("plan").back().at("date").get<std::string>();
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
		if (this->opt.enable_loads) {
			int prevLoadSet = TaskerBase::thestruct["tasks"].at(theRow.taskId).at("load").get<int>();
			std::cout << "  5. Update load units of this task (use an integer) - Currently `"
				<< this->usecolor() << this->getcolor("notify")
				<< prevLoadSet
				<< this->usecolor() << this->getcolor("reset")
				<< "`: ";
			loadint = this->getLoad("Bad input - Please enter a positive integer only.");
		}

		//Create finall Object:
		json taskObj = {
			{ "plan",		TaskerBase::thestruct["tasks"].at(theRow.taskId).at("plan")	},
			{ "created",	TaskerBase::thestruct["tasks"].at(theRow.taskId).at("created")},
			{ "updated",	task_updated											},
			{ "task",		new_task_title											},
			{ "status",		TaskerBase::thestruct["tasks"].at(theRow.taskId).at("status") },
			{ "cancel",		false													},
			{ "load",		loadint													},
			{ "tagged",		TaskerBase::thestruct["tasks"].at(theRow.taskId).at("tagged") },
			{ "report",		TaskerBase::thestruct["tasks"].at(theRow.taskId).at("report") }
		};

		//Add plan if needed:
		if (push_plan) {
			taskObj["plan"].push_back(this->getBaseTaskPlan(plan_duedate, trim_copy(plan_version)));
			taskObj["plan"].back().at("user") = plan_user;
		}

		//Save
		TaskerBase::thestruct["tasks"].at(theRow.taskId) = taskObj;

		//Notify:
		this->printTaskerNotify("Task Updated successfully!");
		this->printTaskerInfo("Info", "Task ID is: " + std::to_string(theRow.taskId + 1));
		std::cout << std::endl;
	} 
	else {

		//Refactor Report:
		json theObjOld = TaskerBase::thestruct["tasks"].at(theRow.taskId).at("report").at(theRow.reportId);
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
		theObjOld["by"] = this->getUserName(push_plan, true, -1, theObjOld["by"].get<std::string>());

		//Get the report note:
		std::cout << "  3. Update task report note (Type `skip` to skip): ";
		rep_note = this->trim_gen(trim_copy(this->getStrMessage("Please retry. You must add a report note.")),'"');
		if (rep_note != "skip") {
			theObjOld["note"] = rep_note;
		}

		//Push to plan notes:
		TaskerBase::thestruct["tasks"].at(theRow.taskId).at("report").at(theRow.reportId) = theObjOld;

		//Update parent status:
		if (theRow.reportId == (int)TaskerBase::thestruct["tasks"].at(theRow.taskId).at("report").size() - 1) {
			TaskerBase::thestruct["tasks"].at(theRow.taskId).at("status") = theObjOld["status"];
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
	if ((int)TaskerBase::thestruct["tasks"].size() <= theTask) { return false; }

	//Set cancel
	if (state) {
		std::cout << std::endl << " > Canceling task: " << theTask << std::endl;
		TaskerBase::thestruct["tasks"].at(theTask).at("cancel") = true;
		this->printTaskerNotify("Task marked as canceled!");
		this->printTaskerInfo("Info", "You can run `--listcancel` to see all canceled tasks");
		this->printTaskerInfo("Info", "You can enable a task again by running `--enabletask {id}`");
	} else {
		std::cout << std::endl << " > Enabling task: " << theTask << std::endl;
		TaskerBase::thestruct["tasks"].at(theTask).at("cancel") = false;
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
	if ((int)TaskerBase::thestruct["tasks"].size() <= theTask) { return false; }
	
	//Validate allowed delete:
	if (this->opt.del_items == false) {
		this->printTaskerNotify("Task delete Prevented!");
		this->printTaskerInfo("Info", "You can't delete tasks because a global option was set tp prevent that - you may want to `--cancel {id}` insted.");
	} else {
		//Delete task
		std::cout << std::endl << " > Deleting task: " << theTask << std::endl;
		TaskerBase::thestruct["tasks"].erase(theTask);
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
	for (json::iterator it = TaskerBase::thestruct["tags"].begin(); it != TaskerBase::thestruct["tags"].end(); ++it) {
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
void TaskerMain::aboutObject() {
	//Print main
	std::cout << std::endl << " > About Project: " << std::endl << std::endl;
	std::string projectName			= TaskerBase::thestruct["name"];
	std::string projectDescription	= TaskerBase::thestruct["desc"];
	//std::string projectCreated;
	std::string definedUsers		= this->getAllUsersStr(TASKER_USER_PREFIX);
	std::string definedTags			= this->getAllTagsStr(TASKER_TAG_PREFIX);
	std::string objectVersion		= TaskerBase::thestruct["tasker"]["version"];
	std::string projectVersion		= TaskerBase::thestruct["version"].back();
	int countOpenTasks		= 0;
	int countClosedTasks	= 0;
	int countCanceledTasks	= 0;
	//count tasks:
	for (unsigned i = 0; i < TaskerBase::thestruct["tasks"].size(); i++) {
		if (TaskerBase::thestruct["tasks"].at(i).at("cancel") == true) {
			countCanceledTasks++;
		}
		else if (TaskerBase::thestruct["tasks"].at(i).at("status") < 1.0) {
			countOpenTasks++;
		}
		else if (TaskerBase::thestruct["tasks"].at(i).at("status") >= 1.0) {
			countClosedTasks++;
		}
	}
	this->printTaskerBasic("Info", "Project Name",			projectName, " -> ");
	this->printTaskerBasic("Info", "Project Description",	projectDescription, " -> ");
	this->printTaskerBasic("Info", "Tasker object version", objectVersion, " -> ");
	this->printTaskerBasic("Info", "Project current version", objectVersion, " -> ");
	this->printTaskerBasic("Info", "Defined Users", definedUsers, " -> ");
	this->printTaskerBasic("Info", "Defined Tags", definedTags, " -> ");
	this->printTaskerBasic("Info", "Total Tasks", std::to_string(countOpenTasks + countClosedTasks + countCanceledTasks), " -> ");
	this->printTaskerBasic("Info", "Open Tasks", std::to_string(countOpenTasks), " -> ");
	this->printTaskerBasic("Info", "Closed Tasks", std::to_string(countClosedTasks), " -> ");
	this->printTaskerBasic("Info", "Canceled Tasks", std::to_string(countCanceledTasks), " -> ");
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
		this->printTaskerNotify("Can't tag! The task/s id's you typed are not set or canceled.");
		this->printTaskerInfo("Advice", "You can use `--listall 2` to list all active tasks.");
	}
	//Validate the tag:
	if (TaskerBase::thestruct["tags"].size() < 1) return false;
	int tagindex = this->findDefinedTag(tag);

	int  counter_tags = 0;
	if (tagindex != -1) {
		for (const auto &taskid : taskList)
		{
			bool addFlag = true;
			json deftags = TaskerBase::thestruct["tasks"].at(taskid).at("tagged");
			if (!deftags.empty()) {
				for (int i = 0; i < (int)deftags.size(); i++) {
					if (deftags.at(i) == tag) 
						addFlag = false;
				}
			}
			if (addFlag) {
				TaskerBase::thestruct["tasks"].at(taskid).at("tagged").push_back(tag);
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

	std::string tag = this->trim_gen(trim_copy(_tag), '"');
	std::string taskListStr;
	std::vector<int> taskList;

	std::cout << std::endl
		<< " > Removing tag: "
		<< this->usecolor() << this->getcolor("tag")
		<< TASKER_TAG_PREFIX << tag
		<< this->usecolor() << this->getcolor("reset");

	if (strTask == "") {
		std::cout << std::endl << std::endl
			<< "  1. Which task to remove tag from (list of integers): ";
		std::getline(std::cin, taskListStr);
		taskListStr = this->trim_gen(trim_copy(taskListStr), '"');
	}
	else {
		taskListStr = this->trim_gen(trim_copy(strTask), '"');
	}

	//Remove spaces:
	std::string::iterator end_pos = std::remove(taskListStr.begin(), taskListStr.end(), ' ');
	taskListStr.erase(end_pos, taskListStr.end());

	//Parse the task list + validate Ids:
	taskList = this->parseTaskListStr(taskListStr);
	if (taskList.empty()) {
		this->printTaskerNotify("Can't remove tags! The task/s id's you typed are not set or canceled.");
		this->printTaskerInfo("Advice", "You can use `--listall 2` to list all active tasks.");
	}
	//Validate the tag:
	if (TaskerBase::thestruct["tags"].size() < 1) return false;
	int tagindex = this->findDefinedTag(tag);

	int  counter_tags = 0;
	if (tagindex != -1) {
		for (const auto &taskid : taskList)
		{
			int remFlag = -1;
			json deftags = TaskerBase::thestruct["tasks"].at(taskid).at("tagged");
			if (!deftags.empty()) {
				for (int i = 0; i < (int)deftags.size(); i++) {
					if (deftags.at(i) == tag)
						remFlag = i;
				}
			}
			if (remFlag > -1) {
				TaskerBase::thestruct["tasks"].at(taskid).at("tagged").erase(
					TaskerBase::thestruct["tasks"].at(taskid).at("tagged").begin() + remFlag
				);
				counter_tags++;
			}
		}
		//print results:
		this->printTaskerNotify("Removed Tags from tasks successfully!");
		this->printTaskerInfo("Info", "Affected: " + std::to_string(counter_tags) + " Tasks.");

	}
	else {
		this->printTaskerNotify("Tag `" + (TASKER_TAG_PREFIX + tag) + "` is not defined.");
		this->printTaskerInfo("Advice", "You can use `--tags` to list all tags defined.");
	}
	std::cout << std::endl;
	return true;
}
bool TaskerMain::newtag(const std::string& _tag)
{
	std::string tag = this->trim_gen(trim_copy(_tag), '"');
	std::string desc;

	//Remove spaces:
	std::string::iterator end_pos = std::remove(tag.begin(), tag.end(), ' ');
	tag.erase(end_pos, tag.end());

	//Early validate:
	if (!this->checkValidTagName(tag)) {
		return false;
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
		TaskerBase::thestruct["tags"].push_back({
			{ tag, {
				{ "desc", desc }
			} }
		});

		//print results:
		this->printTaskerNotify("New tag defined!");
		this->printTaskerInfo("Info", "Tag ID is: " + std::to_string(TaskerBase::thestruct["tags"].size()));

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
	if (TaskerBase::thestruct["tags"].size() < 1) {
		return false;
	}

	//Check if set & delete:
	int  index = this->findDefinedTag(tag);
	int  counter_tags = 0;
	if (index != -1) {

		//Remove tag:
		TaskerBase::thestruct["tags"].erase(index);
		//Remove from assignments:
		for (unsigned i = 0; i < TaskerBase::thestruct["tasks"].size(); i++) {
			//Remove from main
			for (unsigned j = 0; j < TaskerBase::thestruct["tasks"].at(i).at("tagged").size(); j++) {
				if (TaskerBase::thestruct["tasks"].at(i).at("tagged").at(j) == tag) {
					TaskerBase::thestruct["tasks"].at(i).at("tagged").erase(j);
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
	if (!this->checkValidTagName(tag)) {
		return false;
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

		desc_old = this->trim_gen(TaskerBase::thestruct["tags"].at(check).at(tag).at("desc"), '"');
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
		TaskerBase::thestruct["tags"].at(check).at(tag).at("desc") = desc;

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
	for (json::iterator it = TaskerBase::thestruct["users"].begin(); it != TaskerBase::thestruct["users"].end(); ++it) {
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
		for (json::iterator it = TaskerBase::thestruct["users"].begin(); it != TaskerBase::thestruct["users"].end(); ++it) {
			for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
				container.insert(std::pair<std::string, statobj>(ite.key(), statobj()));
			}
		}
		//Add not assigned
		container.insert(std::pair<std::string, statobj>(TASKER_USER_NOT_ASSIGNED, statobj()));
	} else {
		//Build tags:
		for (json::iterator it = TaskerBase::thestruct["tags"].begin(); it != TaskerBase::thestruct["tags"].end(); ++it) {
			for (json::iterator ite = it.value().begin(); ite != it.value().end(); ++ite) {
				container.insert(std::pair<std::string, statobj>(ite.key(), statobj()));
			}
		}
		//Add not tagged
		container.insert(std::pair<std::string, statobj>(TASKER_TAG_NOT_TAGGED, statobj()));
	}

	//Calculate and collect:
	for (unsigned i = 0; i < TaskerBase::thestruct["tasks"].size(); i++) {
		//Skip cancel:
		if (TaskerBase::thestruct["tasks"].at(i).at("cancel")) continue;

		//Calculate this task:
		float _workunitsleft = (float(1.0) - (float)TaskerBase::thestruct["tasks"].at(i).at("status")) * float(100.0);
		float _loadbase		 = (float)TaskerBase::thestruct["tasks"].at(i).at("load");
		float _loadunitsleft = _loadbase * (float(1.0) - (float)TaskerBase::thestruct["tasks"].at(i).at("status"));

		//Set total:
		total.workunits += 100;
		total.workunitsleft += _workunitsleft;
		total.loadunits		+= _loadbase;
		total.loadunitsleft += _loadunitsleft;

		//Set object:
		if (type == "users") {

			//Get assigned:
			std::vector<std::string> users = TaskerBase::thestruct["tasks"].at(i).at("plan").back().at("user");
			if (users.size() > 0) {
				for (auto it = users.begin(); it != users.end(); ++it) {
					container[*it].loadunits		+= _loadbase;
					container[*it].loadunitsleft	+= _loadunitsleft;
					container[*it].workunitsleft	+= _workunitsleft;
					container[*it].workunits		+= 100;
				}
			} else {
				//set not assigned:
				container[TASKER_USER_NOT_ASSIGNED].loadunits		+= _loadbase;
				container[TASKER_USER_NOT_ASSIGNED].loadunitsleft	+= _loadunitsleft;
				container[TASKER_USER_NOT_ASSIGNED].workunitsleft	+= _workunitsleft;
				container[TASKER_USER_NOT_ASSIGNED].workunits		+= 100;
			}

		} else {
			//Get tagged:
			unsigned int tagged_size = (unsigned int)TaskerBase::thestruct["tasks"].at(i).at("tagged").size();
			if (tagged_size > 0) {
				for (unsigned int j = 0; j < tagged_size; j++) {
					std::string _tag = TaskerBase::thestruct["tasks"].at(i).at("tagged").at(j);
					if (this->findDefinedTag(_tag) != -1) {
						container[_tag].loadunits		+= _loadbase;
						container[_tag].loadunitsleft	+= _loadunitsleft;
						container[_tag].workunitsleft	+= _workunitsleft;
						container[_tag].workunits		+= 100;
					}
				}
			} else {
				//set not tagged:
				container[TASKER_TAG_NOT_TAGGED].loadunits		+= _loadbase;
				container[TASKER_TAG_NOT_TAGGED].loadunitsleft	+= _loadunitsleft;
				container[TASKER_TAG_NOT_TAGGED].workunitsleft	+= _workunitsleft;
				container[TASKER_TAG_NOT_TAGGED].workunits		+= 100;
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
	std::string user = this->trim_gen(this->trim_copy(_user), '"');
	std::string desc;
	std::string mail;

	//Remove spaces:
	std::string::iterator end_pos = std::remove(user.begin(), user.end(), ' ');
	user.erase(end_pos, user.end());

	//Early validate:
	if (!this->checkValidUserName(user)) {
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
		TaskerBase::thestruct["users"].push_back({
			{ user, {
				{"desc", desc },
				{"mail", mail }
			}}
		});

		//print results:
		this->printTaskerNotify("New user defined!");
		this->printTaskerInfo("Info", "User ID is: " + std::to_string(TaskerBase::thestruct["users"].size()));

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
		<< TASKER_USER_PREFIX << user
		<< this->usecolor() << this->getcolor("reset")
		<< std::endl;

	//validate first - must be atleast one user:
	if (TaskerBase::thestruct["users"].size() < 2) {
		return false;
	}

	//Check if set & delete:
	int  index = this->findDefinedUser(user);
	int  counter_tasks = 0;
	int  counter_reports = 0;
	if (index != -1) {

		//Remove user:
		TaskerBase::thestruct["users"].erase(index);

		//Remove from assignments:
		for (unsigned i = 0; i < TaskerBase::thestruct["tasks"].size(); i++) {

			//Remove from main
			for (unsigned j = 0; j < TaskerBase::thestruct["tasks"].at(i).at("plan").size(); j++) {
				std::vector<std::string> plan_users = TaskerBase::thestruct["tasks"].at(i).at("plan").at(j).at("user");
				auto itusers = std::find(plan_users.begin(), plan_users.end(), user);
				if (itusers != plan_users.end()) {
					plan_users.erase(itusers);
					counter_tasks++;
					TaskerBase::thestruct["tasks"].at(i).at("plan").at(j).at("user") = plan_users;
				}
			}

			//Remove from reports:
			for (unsigned j = 0; j < TaskerBase::thestruct["tasks"].at(i).at("report").size(); j++) {

				std::vector<std::string> report_users = TaskerBase::thestruct["tasks"].at(i).at("report").at(j).at("by");
				auto itusers = std::find(report_users.begin(), report_users.end(), user);
				if (itusers != report_users.end()) {
					report_users.erase(itusers);
					counter_reports++;
					TaskerBase::thestruct["tasks"].at(i).at("report").at(j).at("by") = report_users;
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
		<< TASKER_USER_PREFIX << user
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
			TaskerBase::thestruct["users"].at(index).at(user).at("desc") = "";
		} else if (desc == "" || desc == "\"\"") {
			//Do nothing
		} else {
			TaskerBase::thestruct["users"].at(index).at(user).at("desc") = desc;
		}
		if (mail == "empty") {
			TaskerBase::thestruct["users"].at(index).at(user).at("mail") = "";
		} else if (mail == "" || mail == "\"\"") {
			//Do nothing
		} else {
			TaskerBase::thestruct["users"].at(index).at(user).at("mail") = mail;
		}

		//print results:
		this->printTaskerNotify("User updated successfully!");
		std::cout << std::endl;

	} else {
		return false;
	}
	return true;
}

bool TaskerMain::searchvalue(const std::string& _value) {

	//Clean the term  
	std::string value = this->trim_gen(this->trim_copy(_value), '"');
	int foundcountintasks = 0;
	int foundcountinreports = 0;
	int limit = 0;
	//Early exit:
	if (value.length() < 1) return false;

	//searchtype-> 1 end with, 2 begin with, 3 any match
	int searchtype = (value.find("*") == 0) ? 1 : 0;
	searchtype = ((value.find("*") == (value.length() - 1)) && searchtype == 0) ? 2 : searchtype;
	searchtype = (searchtype == 0) ? 3 : searchtype;
	value = this->trim_gen(this->trim_copy(value), '*');
	
	//Print header
	this->printTaskerInfo("Searching in Tasks", value);

	//loop defined tasks:
	for (auto &it : TaskerBase::thestruct["tasks"].items()) {
		std::string row = (std::string)it.value().at("task");
		foundcountintasks += this->searchAndPrint(row, value, std::to_string((std::stoi(it.key()) + 1)), searchtype);
		limit = foundcountintasks;
		if (limit == this->opt.limit) {
			std::cout << "        > Limit reached!" << std::endl << std::endl;
			goto limitreached_one;
		}
	}
	if (foundcountintasks == 0) {
		std::cout << "        > No matches found in task titles" << std::endl << std::endl;
	}
	else { std::cout << std::endl; }

	//Print header
	this->printTaskerInfo("Searching in Reports", value);
	//loop defined tasks:
	for (auto &it : TaskerBase::thestruct["tasks"].items()) {
		for (auto &ite : it.value().at("report").items()) {
			std::string row = (std::string)ite.value().at("note");
			limit = foundcountintasks + foundcountinreports;
			if (limit == this->opt.limit) {
				std::cout << "        > Limit reached!" << std::endl << std::endl;
				goto limitreached_one;
			}
			foundcountinreports += this->searchAndPrint(
				row,
				value,
				std::to_string((std::stoi(it.key()) + 1)) + "." + std::to_string((std::stoi(ite.key()) + 1)),
				searchtype
			);
		}
	}
	if (foundcountinreports == 0) {
		std::cout << "        > No matches found in task reports" << std::endl << std::endl;
	}
	else { std::cout << std::endl; }

	limitreached_one:
	this->printTaskerNotify("Found " + std::to_string(foundcountinreports + foundcountintasks) + " Results.");
	
	return true;
}
int TaskerMain::searchAndPrint(const std::string& str, const std::string& value, const std::string& rowindex, int searchtype) {
	std::size_t needle = (searchtype == 1) ? str.rfind(value) : str.find(value);
	if (searchtype == 3 && needle != std::string::npos) {
		std::cout << "        (" << rowindex << ") -> ";
		this->printTaskerHighlighted(str, value, needle, true);
	}
	else if (searchtype == 2 && needle == 0) {
		std::cout << "        (" << rowindex << ") -> ";
		this->printTaskerHighlighted(str, value, needle, false);
	}
	else if (searchtype == 1 && needle == (str.length() - value.length())) {
		std::cout << "        (" << rowindex << ") -> ";
		this->printTaskerHighlighted(str, value, needle, false);
	}
	else {
		return 0;
	}
	return 1;
}
bool TaskerMain::list(const std::string& _level, const std::string& which) {
	return this->list(_level, which, "");
}
bool TaskerMain::list(const std::string& _level, const std::string& which, const std::string& _filter) {

	int theLevel = -1;
	std::vector<std::string> filterCon;
	std::string filter = trim_copy(_filter);
	std::string level = trim_copy(_level);
	int counter_found = 0;

	//Set the default level: 
	if (level == "" || !this->onlyDigits(level)) {
		level = TASKER_BASELIST_LEVEL;
	}

	//Parse the level:
	theLevel = stoi(level);
	if (theLevel < 0 || theLevel > 2) {
		theLevel = stoi(std::string(TASKER_BASELIST_LEVEL));
	}

	//Print main
	std::cout << std::endl << " > Listing matched tasks: " << std::endl;

	//Before start check if we want to parse a filter first:
	if (which == "task" || which == "user" || which == "tag") {
		std::string filterstr = filter;
		std::string filterdeli = TASKER_SPLIT_DELI;
		size_t		filterstrpos = 0;
		std::string tokenfound;
		while ((filterstrpos = filterstr.find(filterdeli)) != std::string::npos) {
			tokenfound = filterstr.substr(0, filterstrpos);
			filterstr.erase(0, filterstrpos + filterdeli.length());
			filterCon.push_back(tokenfound);
		}
		if (filterCon.size() == 0 || filterstr.length() > 0)
			filterCon.push_back(filterstr);
	}
	//Print tasks
	for (unsigned i = 0; i < TaskerBase::thestruct["tasks"].size(); i++) {

		//Selective print:
		if (which != "cancel" && TaskerBase::thestruct["tasks"].at(i).at("cancel") == true) {
			continue;
		}
		if (which == "cancel" && TaskerBase::thestruct["tasks"].at(i).at("cancel") != true) {
			continue;
		}
		if (which == "done" && (float)TaskerBase::thestruct["tasks"].at(i).at("status") < 1.00) {
			continue;
		}
		if (which == "open" && (float)TaskerBase::thestruct["tasks"].at(i).at("status") > 0.99) {
			continue;
		}
		if (which == "today") {
			std::string duedate = TaskerBase::thestruct["tasks"].at(i).at("plan").back().at("date");
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
		if (which == "task") {
			std::string strid = std::to_string((i + 1));
			if (std::find(filterCon.begin(), filterCon.end(), strid) == filterCon.end()) {
				continue;
			}
		}
		if (which == "user") {
			std::vector<std::string> users = TaskerBase::thestruct["tasks"].at(i).at("plan").back().at("user");
			bool userfiltertest = false;
			for (auto itu = users.begin(); itu != users.end(); ++itu) {
				if (std::find(filterCon.begin(), filterCon.end(), *itu) != filterCon.end()) {
					userfiltertest = true;
					break;
				}
			}
			if (!userfiltertest) {
				continue;
			}
		}
		if (which == "tag") {
			bool tagtest = false;
			for (json::iterator it = TaskerBase::thestruct["tasks"].at(i).at("tagged").begin(); it != TaskerBase::thestruct["tasks"].at(i).at("tagged").end(); ++it) {
				std::stringstream t;
				std::string ts;
				t << it.value();
				ts = t.str();
				ts.erase(std::remove(ts.begin(), ts.end(), '"'), ts.end());
				if (std::find(filterCon.begin(), filterCon.end(), ts) != filterCon.end())
					tagtest = true;
			}
			if (!tagtest) {
				continue;
			}
		}

		//Filter Closed & Cancel:
		if (!this->opt.showclosed && (which == "user" || which == "tag")) {
			if ((float)TaskerBase::thestruct["tasks"].at(i).at("status") > 0.99) {
				continue;
			}
			if (TaskerBase::thestruct["tasks"].at(i).at("cancel") == true) {
				continue;
			}
		}

		//Print main row:
		counter_found++;
		std::vector<std::string> usersvec = TaskerBase::thestruct["tasks"].at(i).at("plan").back().at("user");
		std::string users_str = this->getAssignedUserString(usersvec, TASKER_USER_PREFIX, true);
		std::string target = TaskerBase::thestruct["tasks"].at(i).at("plan").back().at("date");
		std::string created = TaskerBase::thestruct["tasks"].at(i).at("created");
		std::stringstream tagged;
		std::string tagged_str;
		for (json::iterator it = TaskerBase::thestruct["tasks"].at(i).at("tagged").begin(); it != TaskerBase::thestruct["tasks"].at(i).at("tagged").end(); ++it) {
			tagged << TASKER_TAG_PREFIX << it.value();
			if (std::next(it) != TaskerBase::thestruct["tasks"].at(i).at("tagged").end()) // last element
				tagged << ", ";
		}
		tagged_str = tagged.str();
		tagged_str.erase(remove(tagged_str.begin(), tagged_str.end(), '\"'), tagged_str.end());
	
		if (target == "" || target == "\"\"") {
			target = "not set";
		}

		target.erase(std::remove(target.begin(), target.end(), '"'), target.end());
		created.erase(std::remove(created.begin(), created.end(), '"'), created.end());

		//Group tags:
		std::cout
			<< " ("
			<< (i + 1)
			<< ") : ["
			<< this->usecolor() << this->getcolor("status", (float)TaskerBase::thestruct["tasks"].at(i).at("status"))
			<< std::to_string(
			(int)((float)TaskerBase::thestruct["tasks"].at(i).at("status") * 100)
			).substr(0, 3)
			<< "%"
			<< this->usecolor() << this->getcolor("reset")
			<< "] -> "
			<< this->usecolor() << ((which == "cancel") ? this->getcolor("error") : this->getcolor("reset"))
			<< ((which == "cancel") ? "CANCELED / " : "")
			<< this->usecolor() << this->getcolor("reset")
			<< TaskerBase::thestruct["tasks"].at(i).at("task")
			<< std::endl;

			
		if (theLevel > 0) {
			std::cout << this->usecolor() << this->getcolor("faded")
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
				<< users_str
				<< this->usecolor() << this->getcolor("reset")
				<< std::endl
				<< this->usecolor() << this->getcolor("faded")
				<< "\t* Tags: "
				<< this->usecolor() << this->getcolor("tag")
				<< (tagged_str.size() > 1 ? tagged_str : "not tagged")
				<< this->usecolor() << this->getcolor("reset")
				<< std::endl;
		}
		if (theLevel > 1) {
			//Will print task notes
			for (unsigned j = 0; j < TaskerBase::thestruct["tasks"].at(i).at("report").size(); j++) {

				//Prepare for print:
				std::vector<std::string> users = TaskerBase::thestruct["tasks"].at(i).at("report").at(j).at("by");
				std::string byuser = this->getAssignedUserString(users, TASKER_USER_PREFIX, true);

				std::string dateout = TaskerBase::thestruct["tasks"].at(i).at("report").at(j).at("date");
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
					<< this->usecolor() << this->getcolor("status", (float)TaskerBase::thestruct["tasks"].at(i).at("report").at(j).at("status"))
					<< std::to_string(
							(int)((float)TaskerBase::thestruct["tasks"].at(i).at("report").at(j).at("status") * 100)
						).substr(0, 3)
					<< "%"
					<< this->usecolor() << this->getcolor("reset")
					<< "] : "
					<< this->usecolor() << this->getcolor("hour")
					<< onlydate << " " << onlyhour
					<< this->usecolor() << this->getcolor("reset")
					<< " -> "
					<< TaskerBase::thestruct["tasks"].at(i).at("report").at(j).at("note")
					<< ", by "
					<< this->usecolor() << this->getcolor("user")
					<< byuser
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