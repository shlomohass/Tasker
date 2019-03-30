//
//  TaskerUpgrade.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#include "TaskerUpgrade.hpp"

namespace tasker {

	TaskerUpgrade::TaskerUpgrade()
	{

	}

	int TaskerUpgrade::run(int &removedTags, int &removedTasks)
	{

		/*************************** version 1.0.3 ****************************/
		std::cout << "1. Checking Project Name:" << std::endl;
		this->check_name();

		std::cout << std::endl << "2. Checking Project Description:" << std::endl;
		this->check_desc();

		std::cout << std::endl << "3. Checking Project Notes:" << std::endl;
		this->check_note();

		std::cout << std::endl << "4. Checking Project Tasker system info:" << std::endl;
		this->check_taskersys();

		std::cout << std::endl << "5. Checking Project Users:" << std::endl;
		this->check_users();

		std::cout << std::endl << "6. Checking Project Tags:" << std::endl;
		removedTags = this->check_tags();

		std::cout << std::endl << "7. Checking Version Container:" << std::endl;
		this->check_version();

		std::cout << std::endl << "8. Checking Types Container:" << std::endl;
		this->check_types();

		std::cout << std::endl << "9. Checking Tasks Container:" << std::endl;
		removedTasks = this->check_tasks();

		std::cout << std::endl << "10. Checking Tasks Metadata:" << std::endl;
		this->check_tasks_meta();

		std::cout << std::endl << "11. Saving upgraded object..." << std::endl;
		TaskerBase::thestruct["tasker"]["version"] = TASKER_VERSION;

		return 0;
	}

	bool TaskerUpgrade::fix(const std::string& attributesStr) {

		bool madeChanges = false;
		std::string input = attributesStr;
		this->cleanString(input, { ' ','"','\'','@','#', '_', '-' });
		std::vector<std::string> candid_attr = this->splitString(input, ',');
		std::vector<std::string> allowed_attr = OBJECT_ATTR;
		std::vector<std::string> attributes = this->filterOutNonPresent(candid_attr, allowed_attr);

		if (attributes.empty()) {
			this->printTaskerNotify("You must type attributes - Seperate them with by " + std::string() + TASKER_SPLIT_DELI_CHAR);
			this->printTaskerInfo("Info", "Attributes you can fix: ");
			std::cout << "             " << this->usecolor() << this->getcolor("faded") << this->implodeVecStr(OBJECT_ATTR, TASKER_SPLIT_DELI_CHAR_SPACE);
			std::cout << this->usecolor() << this->getcolor("reset") << std::endl;
		} else {
			for (auto const& a : attributes) {
				bool change = false;
				//name,description,note,system,users,tags,version,types,tasks
				std::string lower_a = this->lowercase(a);
				if (lower_a == "name") {
					std::cout << "1. Fixing Project Name:" << std::endl;
					change = this->check_name();
				}
				else if (lower_a == "description") {
					std::cout << std::endl << "2. Fixing Project Description:" << std::endl;
					change = this->check_desc();
				}
				else if (lower_a == "note") {
					std::cout << std::endl << "2. Fixing Project Notes:" << std::endl;
					change = this->check_note();
				}
				else if (lower_a == "system") {
					std::cout << std::endl << "2. Fixing Project Tasker system info:" << std::endl;
					change = this->check_taskersys();
				}
				else if (lower_a == "users") {
					std::cout << std::endl << "2. Fixing Project Users:" << std::endl;
					change = this->check_users();
				}
				else if (lower_a == "tags") {
					std::cout << std::endl << "2. Fixing Project Tags:" << std::endl;
					int tagsRemoved = this->check_tags();
					if (tagsRemoved > 0)
						this->printTaskerInfo("Info", "Tasker had to remove -> " + std::to_string(tagsRemoved) + " Tags.");
					change = tagsRemoved > 0;
				}
				else if (lower_a == "version") {
					std::cout << std::endl << "2. Fixing Project Version Container:" << std::endl;
					change = this->check_version();
				}
				else if (lower_a == "types") {
					std::cout << std::endl << "2. Fixing Project Types Container:" << std::endl;
					change = this->check_types();
				}
				else if (lower_a == "tasks") {
					std::cout << std::endl << "2. Fixing Project Tasks Container:" << std::endl;
					int tasksRemoved = this->check_tasks();
					if (tasksRemoved > 0)
						this->printTaskerInfo("Info", "Tasker had to remove -> " + std::to_string(tasksRemoved) + " Tasks.");
					change = tasksRemoved > 0;
					change = this->check_tasks_meta() ? true : change;
				}
				if (!madeChanges) madeChanges = change;
			}
		}
		return madeChanges;
	}

	//Validate & fix procedures:
	bool TaskerUpgrade::check_name() {
		//Check name is present:
		if (TaskerBase::thestruct.count("name") == 1) {
			//Check name is valid:
			if (!TaskerBase::thestruct["name"].is_string()) {
				TaskerBase::thestruct["name"] = this->getFromUser("  >>> Project NAME is not set correctly please type it again: ");
				std::cout << "  >>> Project NAME fixed." << std::endl;
			}
			else {
				std::cout << "  >>> Project name is OK." << std::endl;
				return false;
			}
		}
		else {
			//Create name:
			TaskerBase::thestruct.emplace("name", this->getFromUser("  >>> Can't find The project NAME please type it: "));
			std::cout << "  >>> Project NAME fixed." << std::endl;
		}
		return true;
	}
	bool TaskerUpgrade::check_desc() {
		//Check desc is present:
		if (TaskerBase::thestruct.count("desc") == 1) {
			if (!TaskerBase::thestruct["desc"].is_string()) {
				TaskerBase::thestruct["desc"] = this->getFromUser("  >>> Project description is not set correctly please type it again: ");
				std::cout << "  >>> Project DESCRIPTION fixed." << std::endl;
			}
			else {
				std::cout << "  >>> Project description is OK." << std::endl;
				return false;
			}
		}
		else {
			//Create desc:
			TaskerBase::thestruct.emplace("desc", this->getFromUser("  >>> Can't find The project description please type it: "));
			std::cout << "  >>> Project DESCRIPTION fixed." << std::endl;
		}
		return true;
	}
	bool TaskerUpgrade::check_note() {
		//check notes container:
		if (TaskerBase::thestruct.count("note") == 1) {
			if (!TaskerBase::thestruct["note"].is_array()) {
				if (this->promptUser("  >>> Project notes is not compatible - do you want to fix it ? (data will be lost):")) {
					TaskerBase::thestruct["note"] = json::array();
					std::cout << "  >>> Project NOTES fixed." << std::endl;
				}
				else {
					std::cout << "  >>> Skipped project notes." << std::endl;
					return false;
				}
			}
			else {
				std::cout << "  >>> Project Notes is OK." << std::endl;
				return false;
			}
		}
		else {
			//create notes:
			TaskerBase::thestruct.emplace("note", json::array());
			std::cout << "  >>> Project notes is missing -> created new container." << std::endl;
		}
		return true;
	}
	bool TaskerUpgrade::check_taskersys() {
		bool newCreateTaskerSys = false;
		if (TaskerBase::thestruct.count("tasker") == 1) {
			if (TaskerBase::thestruct["tasker"].is_object()) {

				bool madeSysChanges = false;
				//Check version:
				if (TaskerBase::thestruct["tasker"].count("version") == 1) {
					if (!TaskerBase::thestruct["tasker"].at("version").is_string()) {
						TaskerBase::thestruct["tasker"].emplace("version", TASKER_VERSION);
						madeSysChanges = true;
					}
				}
				else {
					TaskerBase::thestruct["tasker"].emplace("version", TASKER_VERSION);
					madeSysChanges = true;
				}
				//Check usecolors:
				if (TaskerBase::thestruct["tasker"].count("usecolors") == 1) {
					if (!TaskerBase::thestruct["tasker"].at("usecolors").is_boolean()) {
						TaskerBase::thestruct["tasker"].emplace("usecolors", true);
						madeSysChanges = true;
					}
				}
				else {
					TaskerBase::thestruct["tasker"].emplace("usecolors", true);
					madeSysChanges = true;
				}
				//Check enableloads:
				if (TaskerBase::thestruct["tasker"].count("enableloads") == 1) {
					if (!TaskerBase::thestruct["tasker"].at("enableloads").is_boolean()) {
						TaskerBase::thestruct["tasker"].emplace("enableloads", true);
						madeSysChanges = true;
					}
				}
				else {
					TaskerBase::thestruct["tasker"].emplace("enableloads", true);
					madeSysChanges = true;
				}
				//Check allowdelete:
				if (TaskerBase::thestruct["tasker"].count("allowdelete") == 1) {
					if (!TaskerBase::thestruct["tasker"].at("allowdelete").is_boolean()) {
						TaskerBase::thestruct["tasker"].emplace("allowdelete", true);
						madeSysChanges = true;
					}
				}
				else {
					TaskerBase::thestruct["tasker"].emplace("allowdelete", true);
					madeSysChanges = true;
				}
				if (madeSysChanges) {
					std::cout << "  >>> Project tasker SYSTEM object fixed." << std::endl;
				}
				else {
					std::cout << "  >>> Project tasker SYSTEM object is OK." << std::endl;
					return false;
				}
			}
			else {
				newCreateTaskerSys = true;
			}
		}
		else {
			newCreateTaskerSys = true;
		}
		if (newCreateTaskerSys) {
			//create system container:
			TaskerBase::thestruct.emplace("note", json::object(
				{
					{ "version",			TASKER_VERSION },
					{ "usecolors",			true },
					{ "enableloads",		true },
					{ "allowdelete",		true }
				}
			));
			std::cout << "  >>> Project tasker SYSTEM is missing -> created default container." << std::endl;
		}
		return true;
	}
	bool TaskerUpgrade::check_users() {
		bool emptyUsers = false;
		//check users container:
		if (TaskerBase::thestruct.count("users") == 1) {
			if (!TaskerBase::thestruct["users"].is_array()) {
				if (this->promptUser("  >>> Project USERS is not compatible - do you want to fix it ? (data will be lost):")) {
					TaskerBase::thestruct["users"] = json::array();
					emptyUsers = true;
				}
				else {
					std::cout << "  >>> Skipped project USERS." << std::endl;
					return false;
				}
			}
			else if (TaskerBase::thestruct["users"].is_array() && TaskerBase::thestruct["users"].size() == 0) {
				//no users in array:
				emptyUsers = true;
			}
			else {
				std::cout << "  >>> Project Users is OK." << std::endl;
				return false;
			}
		}
		else {
			//create users:
			TaskerBase::thestruct.emplace("users", json::array());
			emptyUsers = true;
			std::cout << "  >>> Project users is missing -> created new container." << std::endl;
		}
		if (emptyUsers) {

			//get default user:
			std::string userName = this->getFromUser("    > set default user name: ");
			std::string userDesc = this->getFromUser("    > set user description: ");
			std::string userMail = this->getFromUser("    > set user Email address: ");

			//store:
			TaskerBase::thestruct["users"].push_back(
				{
					userName,{
						{ "desc", userDesc },
						{ "mail", userMail }
					}
				}
			);
			std::cout << "  >>> Project USERS fixed." << std::endl;
		}
		return true;
	}
	int TaskerUpgrade::check_tags() {

		json container = json::array();
		std::vector<int> toremove;

		//check tags container:
		if (TaskerBase::thestruct.count("tags") == 1) {
			if (!TaskerBase::thestruct["tags"].is_array()) {
				if (this->promptUser("  >>> Project tags container is not compatible - do you want to fix it ? (data will be lost):")) {
					TaskerBase::thestruct["tags"] = json::array();
					std::cout << "  >>> Project TAGS fixed." << std::endl;
				}
				else {
					std::cout << "  >>> Skipped project TAGS." << std::endl;
					return 0;
				}
			}
			else {
				//Check defined tags:
				for (auto &it : TaskerBase::thestruct["tags"].items()) {
					if (it.value().is_object()) {
						for (auto &ite : it.value().items()) {
							if (!ite.value().is_object() || !this->checkValidTagName(ite.key())) {
								toremove.push_back(std::stoi(it.key()));
							}
						}
					}
					else {
						toremove.push_back(std::stoi(it.key()));
					}
				}
				//delete if any:
				if (!toremove.empty()) {
					std::sort(toremove.rbegin(), toremove.rend());
					for (int &it : toremove) {
						TaskerBase::thestruct["tags"].erase(
							TaskerBase::thestruct["tags"].begin() + it);
					}
				}
			}
		}
		else {
			//create Tags:
			TaskerBase::thestruct.emplace("tags", json::array());
			std::cout << "  >>> Project Tags container is missing -> Created new tags container." << std::endl;
		}

		//Out putCheck defined tags:
		if (!toremove.empty()) {
			std::cout << "  >>> Erased " << toremove.size() << " non compatible tags. " << std::endl;
		}
		std::string tagsStr = this->getAllTagsStr();
		std::cout << "  >>> Defined tags: " << (tagsStr == "" ? "None" : tagsStr) << std::endl;
		return (int)toremove.size();
	}
	bool TaskerUpgrade::check_version() {
		//Check version is present:
		int count = TaskerBase::thestruct.count("version");
		bool test = TaskerBase::thestruct["version"].is_array();
		if (TaskerBase::thestruct.count("version") == 1) {
			if (!TaskerBase::thestruct["version"].is_array()) {
				std::string version = this->getFromUser("  >>> Project VERSION is not set correctly. Enter the current version: ");
				TaskerBase::thestruct["version"] = json::array();
				TaskerBase::thestruct["version"].push_back(version);
				std::cout << "  >>> Project VERSION fixed." << std::endl;
			} else {
				std::cout << "  >>> Project VERSION is OK." << std::endl;
				return false;
			}
		} else {
			std::string version = this->getFromUser("  >>> Project VERSION is can't be found. Enter the current version: ");
			TaskerBase::thestruct.emplace("version", json::array());
			TaskerBase::thestruct["version"].push_back(version);
			std::cout << "  >>> Project VERSION fixed." << std::endl;
			
		}
		return true;
	}
	bool TaskerUpgrade::check_types() {
		//Check types is present:
		if (TaskerBase::thestruct.count("types") == 1) {
			if (!TaskerBase::thestruct["types"].is_array()) {
				if (this->promptUser("  >>> Project TYPES is not set correctly. Type yes to fix it: ")) {
					TaskerBase::thestruct["types"] = this->getBaseTypesContainer();
					std::cout << "  >>> Project TYPES fixed." << std::endl;
				} else {
					std::cout << "  >>> Skipped fixing Project TYPES." << std::endl;
					return false;
				}
			} else {
				std::cout << "  >>> Project TYPES is OK." << std::endl;
				return false;
			}
		} else {
			if (this->promptUser("  >>> Project TYPES can't be found. Type yes to fix it: ")) {
				TaskerBase::thestruct.emplace("types", this->getBaseTypesContainer());
				std::cout << "  >>> Project TYPES fixed." << std::endl;
			}
		}
		return true;
	}
	int TaskerUpgrade::check_tasks() {

		json container = json::array();
		std::vector<int> toremove_tasks;

		//check tasks container:
		if (TaskerBase::thestruct.count("tasks") == 1) {
			if (!TaskerBase::thestruct["tasks"].is_array()) {
				if (this->promptUser("  >>> Project TASKS container is not compatible - do you want to fix it ? (data will be lost):")) {
					TaskerBase::thestruct["tasks"] = json::array();
					std::cout << "  >>> Project TASKS fixed." << std::endl;
				}
				else {
					std::cout << "  >>> Skipped project TASKS." << std::endl;
					return 0;
				}
			}
			else {
				//Check defined tasks:
				for (auto &it : TaskerBase::thestruct["tasks"].items()) {
					if (!it.value().is_object()) {
						toremove_tasks.push_back(std::stoi(it.key()));
					}
				}
				//delete if any non object tasks:
				if (!toremove_tasks.empty()) {
					std::sort(toremove_tasks.rbegin(), toremove_tasks.rend());
					for (int &it : toremove_tasks) {
						TaskerBase::thestruct["tasks"].erase(
							TaskerBase::thestruct["tasks"].begin() + it);
					}
				}
			}
		}
		else {
			//create Tasks container:
			TaskerBase::thestruct.emplace("tasks", json::array());
			std::cout << "  >>> Project TASKS container is missing -> Created new TASKS container." << std::endl;
		}

		//Output erased non object tasks:
		if (!toremove_tasks.empty()) {
			std::cout << "  >>> Erased " << toremove_tasks.size() << " non compatible Tasks. " << std::endl;
		}

		return (int)toremove_tasks.size();
	}
	bool TaskerUpgrade::check_tasks_meta() {
		//loop defined tasks:
		bool madeChange = false;
		for (auto &it : TaskerBase::thestruct["tasks"].items()) {
			bool f_cancel	= false,
				f_created	= false,
				f_load		= false,
				f_plan		= false,
				f_report	= false,
				f_status	= false,
				f_tagged	= false,
				f_task		= false,
				f_updated	= false;

			std::vector<std::string> remove_keys;

			for (auto &ite : it.value().items()) {
				//ite.key() / property name;
				//ite.value() / value
				bool change = false;
				if (ite.key() == "cancel") {
					f_cancel = true;
					if (!ite.value().is_boolean()) {
						ite.value() = false;
						madeChange = true;
					}
				}
				else if (ite.key() == "created") {
					f_created = true;
					if (!ite.value().is_string()) {
						ite.value() = this->getcurdatetime();
						madeChange = true;
					}
				}
				else if (ite.key() == "load") {
					f_load = true;
					if (!ite.value().is_number()) {
						ite.value() = 1;
						madeChange = true;
					}
				}
				else if (ite.key() == "plan") {
					f_plan = true;
					if (!ite.value().is_array() || ite.value().empty()) {
						ite.value() = json::array();
						ite.value().push_back(this->getBaseTaskPlan());
						madeChange = true;
					}
				}
				else if (ite.key() == "report") {
					f_report = true;
					if (!ite.value().is_array()) {
						ite.value() = json::array();
						madeChange = true;
					}
				}
				else if (ite.key() == "status") {
					f_status = true;
					if (!ite.value().is_number()) {
						ite.value() = 0.0;
						madeChange = true;
					}
				}
				else if (ite.key() == "tagged") {
					f_tagged = true;
					if (!ite.value().is_array()) {
						if (ite.value().is_string()) {
							//If old tagged as string include it in the new container.
							ite.value() = json::array({ ite.value() });
						} else {
							ite.value() = json::array();
						}
						madeChange = true;
					}
				}
				else if (ite.key() == "task") {
					f_task = true;
					if (!ite.value().is_string()) {
						ite.value() = TASKER_FIXED_EMPTY_TASK_DESC;
						madeChange = true;
					}
				}
				else if (ite.key() == "updated") {
					f_updated = true;
					if (!ite.value().is_string()) {
						ite.value() = this->getcurdatetime();
						madeChange = true;
					}
				}
				else {
					//unknown property delete it.
					remove_keys.push_back(ite.key());
					madeChange = true;
				}
			}

			//Remove unknown keys
			for (auto& rem : remove_keys) {
				it.value().erase(it.value().find(rem));
			}

			//Add missing properties:
			if (!f_cancel) {
				it.value()["cancel"] = false;
				madeChange = true;
			}
			if (!f_created) {
				it.value()["created"] = this->getcurdatetime();
				madeChange = true;
			}
			if (!f_load) {
				it.value()["load"] = 1;
				madeChange = true;
			}
			if (!f_plan) {
				it.value()["plan"] = json::array();
				it.value()["plan"].push_back(this->getBaseTaskPlan());
				madeChange = true;
			}
			if (!f_report) {
				it.value()["report"] = json::array();
				madeChange = true;
			}
			if (!f_status) {
				it.value()["status"] = 0.0;
				madeChange = true;
			}
			if (!f_tagged) {
				it.value()["tagged"] = json::array();
				madeChange = true;
			}
			if (!f_task) {
				it.value()["task"] = TASKER_FIXED_EMPTY_TASK_DESC;
				madeChange = true;
			}
			if (!f_updated) {
				it.value()["updated"] = this->getcurdatetime();
				madeChange = true;
			}
		}

		return madeChange;
	}

	//Helpers:
	std::string TaskerUpgrade::getFromUser(const std::string& mes) {
		std::string buffer = "";
		std::cout << mes;
		std::getline(std::cin, buffer);
		return buffer;
	}
	
	TaskerUpgrade::~TaskerUpgrade()
	{
	}

}
