//
//  TaskerUpgrade.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#include "TaskerUpgrade.hpp"

namespace tasker {

	TaskerUpgrade::TaskerUpgrade(json* _thestruct)
	{
		this->thestruct = _thestruct;
	}
	
	int TaskerUpgrade::run()
	{
		/*************************** version 1.0.3 ****************************/
		std::cout << "1. Checking Project Name:" << std::endl;
		this->check_name();
		
		std::cout << "2. Checking Project Description:" << std::endl;
		this->check_desc();
		
		std::cout << "3. Checking Project Notes:" << std::endl;
		this->check_note();
		
		std::cout << "4. Checking Project Tasker system info:" << std::endl;
		this->check_taskersys();

		std::cout << "5. Checking Project Users:" << std::endl;
		this->check_users();

		std::cout << "5. Checking Project Tags:" << std::endl;
		this->check_tags();

		/*
		if (this->thestruct->count("types") == 1) {

		}
		if (this->thestruct->count("version") == 1) {

		}
		if (this->thestruct->count("tasks") == 1) {

		}
		*/
		return 0;
		
		/*
		//Loop Tasks:
		for (unsigned i = 0; i < this->thestruct["tasks"].size(); i++) {

		//Remove from main
		for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("plan").size(); j++) {
		std::vector<std::string> plan_users = this->thestruct["tasks"].at(i).at("plan").at(j).at("user");
		auto itusers = std::find(plan_users.begin(), plan_users.end(), user);
		if (itusers != plan_users.end()) {
		plan_users.erase(itusers);
		counter_tasks++;
		this->thestruct["tasks"].at(i).at("plan").at(j).at("user") = plan_users;
		}
		}

		//Remove from reports:
		for (unsigned j = 0; j < this->thestruct["tasks"].at(i).at("report").size(); j++) {

		std::vector<std::string> report_users = this->thestruct["tasks"].at(i).at("report").at(j).at("by");
		auto itusers = std::find(report_users.begin(), report_users.end(), user);
		if (itusers != report_users.end()) {
		report_users.erase(itusers);
		counter_reports++;
		this->thestruct["tasks"].at(i).at("report").at(j).at("by") = report_users;
		}
		}
		}
		*/
		
	}

	//Validate & fix procedures:
	bool TaskerUpgrade::check_name() {
		//Check name is present:
		if (this->thestruct->count("name") == 1) {
			//Check name is valid:
			if (!this->thestruct->at("name").is_string()) {
				this->thestruct->at("name") = this->getFromUser("  >>> Project NAME is not set correctly please type it again: ");
			}
			else {
				std::cout << "  >>> Project name is OK." << std::endl;
			}
		}
		else {
			//Create name:
			this->thestruct->emplace("name", this->getFromUser("  >>> Can't find The project NAME please type it: "));
		}
		return true;
	}
	bool TaskerUpgrade::check_desc() {
		//Check desc is present:
		if (this->thestruct->count("desc") == 1) {
			if (!this->thestruct->at("desc").is_string()) {
				this->thestruct->at("desc") = this->getFromUser("  >>> Project description is not set correctly please type it again: ");
			}
			else {
				std::cout << "  >>> Project description is OK." << std::endl;
			}
		}
		else {
			//Create desc:
			this->thestruct->emplace("desc", this->getFromUser("  >>> Can't find The project description please type it: "));
		}
		return true;
	}
	bool TaskerUpgrade::check_note() {
		//check notes container:
		if (this->thestruct->count("note") == 1) {
			if (!this->thestruct->at("note").is_array()) {
				if (this->promptUser("  >>> Project notes is not compatible - do you want to fix it ? (data will be lost):")) {
					this->thestruct->at("note") = json::array();
					std::cout << "  >>> Project Notes is fixed." << std::endl;
				}
				else {
					std::cout << "  >>> Skipped project notes." << std::endl;
				}
			}
			else {
				std::cout << "  >>> Project Notes is OK." << std::endl;
			}
		}
		else {
			//create notes:
			this->thestruct->emplace("note", json::array());
			std::cout << "  >>> Project notes is missing -> created new container." << std::endl;
		}
		return true;
	}
	bool TaskerUpgrade::check_taskersys() {
		bool newCreateTaskerSys = false;
		if (this->thestruct->count("tasker") == 1) {
			if (this->thestruct->at("tasker").is_object()) {

				bool madeSysChanges = false;
				//Check version:
				if (this->thestruct->at("tasker").count("version") == 1) {
					if (!this->thestruct->at("tasker").at("version").is_string()) {
						this->thestruct->at("tasker").emplace("version", TASKER_VERSION);
						madeSysChanges = true;
					}
				}
				else {
					this->thestruct->at("tasker").emplace("version", TASKER_VERSION);
					madeSysChanges = true;
				}
				//Check usecolors:
				if (this->thestruct->at("tasker").count("usecolors") == 1) {
					if (!this->thestruct->at("tasker").at("usecolors").is_boolean()) {
						this->thestruct->at("tasker").emplace("usecolors", true);
						madeSysChanges = true;
					}
				}
				else {
					this->thestruct->at("tasker").emplace("usecolors", true);
					madeSysChanges = true;
				}
				//Check enableloads:
				if (this->thestruct->at("tasker").count("enableloads") == 1) {
					if (!this->thestruct->at("tasker").at("enableloads").is_boolean()) {
						this->thestruct->at("tasker").emplace("enableloads", true);
						madeSysChanges = true;
					}
				}
				else {
					this->thestruct->at("tasker").emplace("enableloads", true);
					madeSysChanges = true;
				}
				//Check allowdelete:
				if (this->thestruct->at("tasker").count("allowdelete") == 1) {
					if (!this->thestruct->at("tasker").at("allowdelete").is_boolean()) {
						this->thestruct->at("tasker").emplace("allowdelete", true);
						madeSysChanges = true;
					}
				}
				else {
					this->thestruct->at("tasker").emplace("allowdelete", true);
					madeSysChanges = true;
				}
				if (madeSysChanges)
					std::cout << "  >>> Project tasker system object was fixed." << std::endl;
				else
					std::cout << "  >>> Project tasker system object is OK." << std::endl;
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
			this->thestruct->emplace("note", json::object(
				{
					{ "version",			TASKER_VERSION },
				{ "usecolors",			true },
				{ "enableloads",		true },
				{ "allowdelete",		true }
				}
			));
			std::cout << "  >>> Project tasker system is missing -> created default container." << std::endl;
		}
		return true;
	}
	bool TaskerUpgrade::check_users() {
		bool emptyUsers = false;
		//check users container:
		if (this->thestruct->count("users") == 1) {
			if (!this->thestruct->at("users").is_array()) {
				if (this->promptUser("  >>> Project users is not compatible - do you want to fix it ? (data will be lost):")) {
					this->thestruct->at("users") = json::array();
					emptyUsers = true;
				}
				else {
					std::cout << "  >>> Skipped project Users." << std::endl;
				}
			}
			else if (this->thestruct->at("users").is_array() && this->thestruct->at("users").size() == 0) {
				//no users in array:
				emptyUsers = true;
			} else {
				std::cout << "  >>> Project Users is OK." << std::endl;
			}
		}
		else {
			//create users:
			this->thestruct->emplace("users", json::array());
			emptyUsers = true;
			std::cout << "  >>> Project users is missing -> created new container." << std::endl;
		}
		if (emptyUsers) {

			//get default user:
			std::string userName = this->getFromUser("    > set default user name: ");
			std::string userDesc = this->getFromUser("    > set user description: ");
			std::string userMail = this->getFromUser("    > set user Email address: ");

			//store:
			this->thestruct->at("users").push_back(
				{
					userName,{
						{ "desc", userDesc },
						{ "mail", userMail }
					}
				}
			);
			std::cout << "  >>> Project Users is fixed." << std::endl;
		}
		return true;
	}
	bool TaskerUpgrade::check_tags() {

		json container = json::array();
		//check tags container:
		if (this->thestruct->count("tags") == 1) {
			if (!this->thestruct->at("tags").is_array()) {
				if (this->promptUser("  >>> Project tags container is not compatible - do you want to fix it ? (data will be lost):")) {
					this->thestruct->at("tags") = json::array();
				}
				else {
					std::cout << "  >>> Skipped project Tags." << std::endl;
				}
			}
			else {
				//Check defined tags:
				std::vector<int> toremove;
				for (auto &it : this->thestruct->at("tags").items()) {
					if (it.value().is_object()) {
						for (auto &ite : it.value().items()) {
							if (ite.value().is_object() && ite.key()) {
								std::cout << "good value: " << ite.value() << '\n';
							} else {

							}
						}
					}
					else {
						std::cout << "found bad value: " << it.value() <<  " deleting..." << '\n';
						toremove.push_back(std::stoi(it.key()));
					}
				}
				//delete if any:
				for (int &it : toremove) {
					this->thestruct->at("tags").erase(
						this->thestruct->at("tags").begin() + it);
				}
			}
		} else {
			//create Tags:
			this->thestruct->emplace("tags", json::array());
			std::cout << "  >>> Project Tags container is missing -> created new container." << std::endl;
		}

		//checks...
		//Check defined tags:
		for (auto &it : this->thestruct->at("tags").items()) {
			std::cout << "found value: " << it.value() << '\n';
		}
		return true;
	}
	//Helpers:
	std::string TaskerUpgrade::getFromUser(const std::string& mes) {
		std::string buffer = "";
		std::cout << mes;
		std::getline(std::cin, buffer);
		std::cout << std::endl;
		return buffer;
	}
	bool TaskerUpgrade::promptUser(const std::string& mes) {
		std::string buffer = "";
		std::cout << mes;
		std::getline(std::cin, buffer);
		std::cout << std::endl;
		return buffer == "y" || buffer == "yes" || buffer == "ok" || buffer == "go";
	}
	TaskerUpgrade::~TaskerUpgrade()
	{
	}

}
