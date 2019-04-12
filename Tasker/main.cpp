//
//  main.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright © 2017 Shlomo Hassid. All rights reserved.
//  
//  Command Line Examples:
//

#define _UNICODE
#define UNICODE


#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "argvparser.hpp"
#include "SETTASKER.hpp"
#include "TaskerAdd.hpp"
#include "TaskerMain.hpp"
#include "TaskerUpgrade.hpp"
/*
#ifndef _WIN32
inline int _pipe(int fildes[2], unsigned psize, int textmode) {
	return pipe(fildes);
}
inline FILE* _popen(const char* command, const char* type) {
	return popen(command, type);
}
inline void _pclose(FILE* file) {
	pclose(file);
}
#endif
*/

namespace cm = CommandLineProcessing;

void setMainArgs(
	cm::ArgvParser *cmd, 
	bool *run_init, 
	bool *run_upgrade,
	bool *enable_debug, 
	bool *use_colors, 
	bool *showclosed, 
	bool *enable_loads
) {
	if (cmd->foundOption("debug")) {
		*enable_debug = true;
	}
	if (cmd->foundOption("init")) {
		*run_init = true;
	}
	if (cmd->foundOption("upgrade")) {
		*run_upgrade = true;
	}
	if (cmd->foundOption("discolor")) {
		*use_colors = false;
	}
	else {
		*use_colors = true;
	}
	if (cmd->foundOption("showclosed")) {
		*showclosed = true;
	}
	else {
		*showclosed = false;
	}
	*enable_loads = true;
	return;
}

int main(int argc, char** argv) {

	#ifdef PLATOTHER
		std::cout << "This program was not compiled correctly" << std::endl;
		return 0;
	#else

	//Define args and project settings:
	tasker::finalOp finalOp;  // Project finalization object
	std::string filepath = "";
	cm::ArgvParser cmd;
	bool enable_debug	= TASKER_DEBUG;
	bool run_init		= false;
	bool run_upgrade	= false;
	tasker::moreOpt moreopt;
	
	cmd.addErrorCode(finalOp.exitCodeOk,	"Success"	);
	cmd.addErrorCode(finalOp.exitCodeError, "Error"		);
	cmd.setIntroductoryDescription("Tasker Manager - version: " + std::string(TASKER_VERSION) + " - By: " + std::string(TASKER_AUTHOR));
	
	cmd.setHelpOption("h",		"help", "Print this help page");
	cmd.defineOption("init",	"Initialize a `Tasker` object in the current path", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("upgrade", "Try upgrade a Tasker object to current version", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("fix",		"Fix a specific object container -> ''|correct attribute", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("debug",	"Enable debug mode.", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("about",	"Display the project details", cm::ArgvParser::NoOptionAttribute);

	cmd.defineOption("task",	"Add a new task -> Will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("taskid",  "Defines a task id to target -> is used with several procedures.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("report",	"Report progress to a task -> Will ask for more options and settings interactivly", cm::ArgvParser::OptionRequiresValue);
	
	cmd.defineOption("show",	"Show in detail a single task information -> Expects task ID.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("search",  "Search related taskd -> Expects a search term.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("limit",   "Limit the number of results that will be returned -> Expects a number.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("refactor", "Refactor a Task or a report progress of a task Expect integer that represents the task id or a float that represets the report.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("cancel",	"Cancel a task -> Will be reserved and later could be activated.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("enable",	"Enable a canceled task.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("update",	"Update a task -> Will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("deltask", "Delete a task -> Will completely delete from records.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("tags",		"Show all defined tags", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("tag",			"Tag a task -> Expect a defined tag name, Optional add --taskid {integer}.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("remtag",		"Tag a task -> Expect a defined tag name, Optional add --taskid {integer}.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("newtag",		"Add a new tag -> Will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("deltag",		"Delete a tag -> Will remove the tag from tasks also.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("updatetag",	"Update a tag credentials -> Will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);


	cmd.defineOption("users",		"Show all defined users", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("newuser",		"Add a new user -> Will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("deluser",		"Delete a user -> Will remove the user from tasks also.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("updateuser",	"Update a user credentials -> Will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("listtask",	"List selected tasks -> Expect an integer or a comma separated list of them. Combine with --details {0,1,2}", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listall",		"List all tasks -> Expect an integer for details level {0,1,2}", cm::ArgvParser::OptionalValue);
	cmd.defineOption("listdone",	"List all closed / finished tasks -> Expect an integer for details level", cm::ArgvParser::OptionalValue);
	cmd.defineOption("listcancel",	"List all canceled tasks. Combine with --details {0,1,2}", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("listuser",	"List user tasks -> Expects the users string to be shown. Combine with --details {0,1,2}", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listtag",		"List tagged tasks -> Expects the tags string to be shown. Combine with --details {0,1,2}", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listopen",	"List all open tasks -> Expect an integer for display level {0,1,2}", cm::ArgvParser::OptionalValue);
	cmd.defineOption("listtoday",	"List tasks that are due to today -> Expect an integer for display level {0,1,2}", cm::ArgvParser::OptionalValue);
	cmd.defineOption("details",		"List details level	-> Expect an integer for details display level {0,1,2}", cm::ArgvParser::OptionalValue);
	
	cmd.defineOption("showclosed",		"Dont filter out canceled or closed tasks.", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("discolor",		"Disable colored console text for one execution only.", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("set_optcolor",	"Set option whether use colored console text. Expect true|false OR 1|0", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("set_optdelete",	"Set option whether to allow task delete. Expect true|false OR 1|0", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("set_optloads",	"Set option whether to use task loads. Expect true|false OR 1|0", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("stats", "Show project work stats -> Expect tags|users", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOptionAlternative("task",			"t"	);
	cmd.defineOptionAlternative("report",		"r"	);
	cmd.defineOptionAlternative("refactor",		"ref");
	cmd.defineOptionAlternative("cancel",		"c"	);
	cmd.defineOptionAlternative("enable",		"e"	);
	cmd.defineOptionAlternative("update",		"u");
	cmd.defineOptionAlternative("deltask",		"dt");
	cmd.defineOptionAlternative("details",		"d");
	cmd.defineOptionAlternative("search",		"s");
	cmd.defineOptionAlternative("limit",		"lim");
	cmd.defineOptionAlternative("listall",		"la");
	cmd.defineOptionAlternative("listdone",		"ld");
	cmd.defineOptionAlternative("listcancel",	"lc");
	cmd.defineOptionAlternative("listuser",		"lu");
	cmd.defineOptionAlternative("listtag",		"ltg");
	cmd.defineOptionAlternative("listopen",		"lo");
	cmd.defineOptionAlternative("listtask",		"lt");

	int result = cmd.parse(argc, argv);

	

	//Parse and set Argu:
	if (result != cm::ArgvParser::NoParserError)
	{
		if (result == cm::ArgvParser::ParserHelpRequested) {
			finalOp.exitCode = finalOp.exitCodeOk;
			std::cout << cmd.parseErrorDescription(result);
		}
		else {
			finalOp.exitCode = finalOp.exitCodeError;
			std::cout << "\t* " << cmd.parseErrorDescription(result) << std::endl;
			std::cout << "\t** " << "Type `--help` to see all options and commands." << std::endl;
		}
		goto ExitQuick;
	} else {
		setMainArgs(
			&cmd, 
			&run_init, 
			&run_upgrade,
			&enable_debug, 
			&moreopt.use_colors, 
			&moreopt.showclosed, 
			&moreopt.enable_loads
		);
	}

	//Parse Additional Options - does it has a task id predefined ?
	if (cmd.foundOption("taskid")) {
		//Get a task id string:
		moreopt.taskIdStr = cmd.optionValue("taskid");
	}
	else moreopt.taskIdStr = "";
	
	// does it has a detail predefined ?
	if (cmd.foundOption("details")) {
		//Get the details level string:
		moreopt.detailsLevel = cmd.optionValue("details");
	}
	else moreopt.detailsLevel = "";

	// does it has a limit predefined ?
	if (cmd.foundOption("limit") && tasker::TaskerBase::isInteger(cmd.optionValue("limit"))) {
		//Get the limit integer string:
		moreopt.limit = std::stoi(cmd.optionValue("limit"));
	} else moreopt.limit = TASKER_RESULT_LIMIT;


	//Main Tasker Object:
	tasker::TaskerMain* Task = new tasker::TaskerMain(moreopt);
	Task->setPath();
	bool hasobj = Task->loadBase();

	//Main Program Starts here:
	if (run_init) {
		//Check that we are not redeclaring the local object:
		if (!hasobj) {
			Task->createEmpty();
			finalOp.set(true, true);
			finalOp.infoMesNotify.push_back("Tasker Created!");
			finalOp.infoMesAdvice.push_back("Start by adding some tasks by running `--task`.");
		} else {
			finalOp.set(false, true);
			finalOp.infoMesNotify.push_back("No can do! Allready initialized");
			finalOp.infoMesAdvice.push_back("You can run '--destroy' to completely remove the instance.");
		}
	}
	else {
		//Handle Operations:
		if (hasobj) {
			//Load current obj:
			std::string fileversion = "";
			int loadParse = Task->loadObj(fileversion);
			if (loadParse == 1) { //permissions
				finalOp.infoMesNotify.push_back("Oups!");
				finalOp.infoMesError.push_back("Can't Read the Tasker object check permissions please.");
				finalOp.exitCode = finalOp.exitCodeError;
				goto Finalize;
			}
			else if (run_upgrade) {
				tasker::TaskerUpgrade TaskerUp = tasker::TaskerUpgrade();
				int removedTags = 0;
				int removedTasks = 0;
				int upgradeCode = TaskerUp.run(removedTags, removedTasks);
				//Final messages:
				finalOp.set(true, true);
				finalOp.infoMesFinal.push_back("Tasker had to remove ->" + std::to_string(removedTasks) + " Tasks.");
				finalOp.infoMesFinal.push_back("Tasker had to remove ->" + std::to_string(removedTags) + " Tags.");
				goto Finalize;
			}
			else if (loadParse == 2) { //Version mismatch
				finalOp.infoMesNotify.push_back("Version Mismatch!");
				finalOp.infoMesError.push_back("The tasker file you are targeting is not compatible with this version.");
				finalOp.infoMesFinal.push_back(std::string("File Version: ") + fileversion + std::string(" != Tasker Version: ") + std::string(TASKER_VERSION));
				finalOp.infoMesAdvice.push_back("You can run '--upgrade' to try repare the Tasker object file.");
				finalOp.exitCode = finalOp.exitCodeError;
				goto Finalize;
			} else { // Ok load options:
				Task->parseOptions(cmd.foundOption("discolor"));
			}

		//Parse Options:
			if (cmd.foundOption("fix")) {
				// Write new task:
				std::string objectfix = cmd.optionValue("fix");
				tasker::TaskerUpgrade TaskerUp = tasker::TaskerUpgrade();
				bool madeChanges = TaskerUp.fix(objectfix);
				finalOp.set(madeChanges, true);
				finalOp.infoMesNotify.push_back("Finished!");
				goto Finalize;
			}
			//Display the project details:
			if (cmd.foundOption("about")) {
				Task->aboutObject();
				finalOp.set(false, true);
				goto Finalize;
			}
			//Handle set tasks:
			if (cmd.foundOption("task")) {
				// Write new task:
				std::string taskAdd = cmd.optionValue("task");
				Task->setNewTask(taskAdd);
				finalOp.set(true, true);
				goto Finalize;
			}
			//Handle cancel a tasks:
			if (cmd.foundOption("cancel")) {
				std::string taskId = cmd.optionValue("cancel");
				bool resultCancel = Task->cancelTask(taskId, true);
				finalOp.set(resultCancel, true);
				if (!resultCancel) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Task could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle enable a tasks:
			if (cmd.foundOption("enable")) {
				std::string taskId = cmd.optionValue("enable");
				bool resultEnable = Task->cancelTask(taskId, false);
				finalOp.set(resultEnable, true);
				if (!resultEnable) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Task could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle delete tasks:
			if (cmd.foundOption("deltask")) {
				std::string taskId = cmd.optionValue("deltask");
				bool resultDelete = Task->deleteTask(taskId);
				finalOp.set(resultDelete, true);
				if (!resultDelete) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Task could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle refactor tasks:
			if (cmd.foundOption("refactor")) {
				std::string taskId = cmd.optionValue("refactor");
				bool resultRefactor = Task->refactorTask(taskId);
				finalOp.set(resultRefactor, true);
				if (!resultRefactor) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Task or Report could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle report to task:
			if (cmd.foundOption("report")) {
				// Write new task report:
				std::string taskId = cmd.optionValue("report");
				bool resultReport = Task->reportToTask(taskId);
				finalOp.set(resultReport, true);
				if (!resultReport) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Task could not be found or input is invalid. Task may also be canceled.");
					finalOp.infoMesAdvice.push_back("You can run `--listall {level 1,2}` to see all listed tasks");
					finalOp.infoMesAdvice.push_back("You can run `--listcancel` to see all canceled tasks");
					finalOp.infoMesAdvice.push_back("You can enable a task again by running `--enabletask {id}`");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle tags show:
			if (cmd.foundOption("tags")) {
				Task->showtags();
				finalOp.set(false, true);
				goto Finalize;
			}
			//Handle add tag to a task:
			if (cmd.foundOption("tag")) {
				// Add a tag to task:
				std::string tag = cmd.optionValue("tag");
				bool resultTagging = Task->addtag(tag, moreopt.taskIdStr);
				finalOp.set(resultTagging, true);
				if (!resultTagging) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Tag could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle removing a tag from task:
			if (cmd.foundOption("remtag")) {
				// Remove a tag:
				std::string tag = cmd.optionValue("remtag");
				bool resultRemove = Task->remtag(tag, moreopt.taskIdStr);
				finalOp.set(resultRemove, true);
				if (!resultRemove) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Tag could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle defining a new tag:
			if (cmd.foundOption("newtag")) {
				// Add new tag:
				std::string tag = cmd.optionValue("newtag");
				bool resultNewTag = Task->newtag(tag);
				finalOp.set(resultNewTag, true);
				if (!resultNewTag) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Tag name must be at least 2 chars long without spaces and not a reserved name.");
					finalOp.infoMesFinal.push_back("Reserved names: " + Task->getReservedTagNames(", ") + ".");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle delete tags:
			if (cmd.foundOption("deltag")) {
				std::string tag = cmd.optionValue("deltag");
				bool resultDelete = Task->deltag(tag);
				finalOp.set(resultDelete, true);
				if (!resultDelete) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Tag could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle update tag:
			if (cmd.foundOption("updatetag")) {
				std::string tagId = cmd.optionValue("updatetag");
				bool resultUpdate = Task->updatetag(tagId);
				finalOp.set(resultUpdate, true);
				if (!resultUpdate) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Tag could not be found or input is invalid.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle users show:
			if (cmd.foundOption("users")) {
				// Expose the users list:
				Task->showusers();
				finalOp.set(false, true);
				goto Finalize;
			}
			//Handle stats show:
			if (cmd.foundOption("stats")) {
				// Expose the stats list:
				std::string statstype = cmd.optionValue("stats");
				bool resultStats = Task->showstats(statstype);
				finalOp.set(false, true);
				if (!resultStats) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Stat type `" + statstype + "` is invalid.");
					finalOp.infoMesAdvice.push_back("Use one of those: `users`, `tags`.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle add user:
			if (cmd.foundOption("newuser")) {
				// Add new user:
				std::string username = cmd.optionValue("newuser");
				bool resultNewUser = Task->adduser(username);
				finalOp.set(resultNewUser, true);
				if (!resultNewUser) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("User name must be at least 2 chars long without spaces and not a reserved name.");
					finalOp.infoMesFinal.push_back("Reserved names: " + Task->getReservedUserNames(", ") + ".");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle delete user:
			if (cmd.foundOption("deluser")) {
				// Delete user:
				std::string username = cmd.optionValue("deluser");
				bool resultDelUser = Task->deluser(username);
				finalOp.set(resultDelUser, true);
				if (!resultDelUser) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("You can't delete all users - must be at least 1 user defined.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle update user:
			if (cmd.foundOption("updateuser")) {
				// Update user creds:
				std::string username = cmd.optionValue("updateuser");
				bool resultUpdateUser = Task->updateuser(username);
				finalOp.set(resultUpdateUser, true);
				if (!resultUpdateUser) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Can't find user: " + username);
					finalOp.infoMesAdvice.push_back("You can use `--users` to list all users defined.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle report to task:
			if (cmd.foundOption("search")) {
				// Write new task report:
				std::string searchVal = cmd.optionValue("search");
				bool resultSearch = Task->searchvalue(searchVal);
				finalOp.set(resultSearch, true);
				if (!resultSearch) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("No search term found.");
					finalOp.infoMesAdvice.push_back("Try typing `--search \"value\"` to perform a search.");
					finalOp.infoMesAdvice.push_back("You can limit the results by adding `--limit 10`.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}
			//Handle lists:
			if (cmd.foundOption("listtask") ||
				cmd.foundOption("listall") ||
				cmd.foundOption("listdone") ||
				cmd.foundOption("listcancel") ||
				cmd.foundOption("listuser") ||
				cmd.foundOption("listtag") ||
				cmd.foundOption("listopen") ||
				cmd.foundOption("listtoday")
				) {
				// Get level:
				std::string listlevel;
				std::string which;
				std::string filter = "";

				//Grab:
				if (cmd.foundOption("listtask")) {
					listlevel = "2";
					filter = cmd.optionValue("listtask");
					which = "task";
				}
				else if (cmd.foundOption("listall")) {
					listlevel = cmd.optionValue("listall");
					which = "all";
				}
				else if (cmd.foundOption("listdone")) {
					listlevel = cmd.optionValue("listdone");
					which = "done";
				}
				else if (cmd.foundOption("listcancel")) {
					listlevel = "1";
					which = "cancel";
				}
				else if (cmd.foundOption("listuser")) {
					listlevel = "2";
					filter = cmd.optionValue("listuser");
					which = "user";
				}
				else if (cmd.foundOption("listtag")) {
					listlevel = "2";
					filter = cmd.optionValue("listtag");
					which = "tag";
				}
				else if (cmd.foundOption("listopen")) {
					listlevel = cmd.optionValue("listopen");
					which = "open";
				}
				else if (cmd.foundOption("listtoday")) {
					listlevel = cmd.optionValue("listtoday");
					which = "today";
				}
				//OverWrite list level if set:
				if (moreopt.detailsLevel != "") {
					listlevel = moreopt.detailsLevel;
				}

				bool resultList = Task->list(listlevel, which, filter);
				finalOp.set(false, true);
				if (!resultList) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Some error occured can't expose the requested task list.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}

			//Handle Options:
			if (cmd.foundOption("set_optcolor") ||
				cmd.foundOption("set_optdelete") ||
				cmd.foundOption("set_optloads")
				) {
				// Get option:
				std::string state;
				std::string which;
				//Grab:
				if (cmd.foundOption("set_optcolor")) {
					state = cmd.optionValue("set_optcolor");
					which = "optcolor";
				}
				else if (cmd.foundOption("set_optdelete")) {
					state = cmd.optionValue("set_optdelete");
					which = "optdelete";
				}
				else if (cmd.foundOption("set_optloads")) {
					state = cmd.optionValue("set_optloads");
					which = "optloads";
				}

				//Set the option:
				bool resultOptionSet = Task->setOption(which, state);
				finalOp.set(resultOptionSet, true);
				if (!resultOptionSet) {
					finalOp.infoMesNotify.push_back("Oups!");
					finalOp.infoMesError.push_back("Bad input - can't set option.");
					finalOp.infoMesAdvice.push_back("You should use -> true|false OR 1|0.");
					finalOp.exitCode = finalOp.exitCodeError;
				}
				goto Finalize;
			}

		}
		else {
			finalOp.infoMesNotify.push_back("Oups!");
			finalOp.infoMesError.push_back("Can't find Tasker object make sure you ran `--init` in this directory before.");
			finalOp.exitCode = finalOp.exitCodeError;
			goto Finalize;
		}
	}

Finalize:

	//Write Object:
	if (hasobj && !Task->writeObj(true, finalOp.setModified, finalOp.setUsed)) {
		finalOp.infoMesNotify.clear();
		finalOp.infoMesFinal.clear();
		finalOp.infoMesAdvice.clear();
		finalOp.infoMesError.clear();
		finalOp.infoMesNotify.push_back("Oups!");
		finalOp.infoMesError.push_back("Could not write to Tasker object.");
		finalOp.exitCode = finalOp.exitCodeError;
	}
	
	//Print System messages;
	for (auto const& mes : finalOp.infoMesNotify) Task->printTaskerNotify(mes);
	for (auto const& mes : finalOp.infoMesError) Task->printTaskerInfo("Error", mes);
	for (auto const& mes : finalOp.infoMesFinal) Task->printTaskerInfo("Info", mes);
	for (auto const& mes : finalOp.infoMesAdvice) Task->printTaskerInfo("Advice", mes);

	if (enable_debug) {
        #ifdef PLATWIN
            system("pause");
            
        #else
            std::cout << "Enter to exit the program: ";
            std::cin.ignore().get(); //Pause Command for Linux Terminal
        #endif
	}

	delete Task;

ExitQuick:

	return finalOp.exitCode;
	#endif
}