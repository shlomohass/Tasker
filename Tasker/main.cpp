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
	int exitCodeError	= 1;
	int exitCodeOk		= 0;
	int exitCode		= exitCodeOk;

	std::string filepath = "";
	cm::ArgvParser cmd;
	bool enable_debug	= TASKER_DEBUG;
	bool run_init		= false;
	bool run_upgrade	= false;
	//Additional options struct:
	tasker::moreOpt moreopt;

	cmd.addErrorCode(exitCodeOk,	"Success"	);
	cmd.addErrorCode(exitCodeError, "Error"		);
	cmd.setIntroductoryDescription("Tasker Manager - version: " + std::string(TASKER_VERSION) + " - By: " + std::string(TASKER_AUTHOR));
	
	cmd.setHelpOption("h",		"help", "Print this help page");
	cmd.defineOption("init",	"Initialize a `Tasker` object in the current path", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("upgrade", "Try upgrade a Tasker object to current version", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("debug",	"Enable debug mode.", cm::ArgvParser::NoOptionAttribute);
	
	cmd.defineOption("task",	"Add a new task -> Will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("taskid",  "Defines a task id to target -> is used with several procedures.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("report",	"Report progress to a task -> Will ask for more options and settings interactivly", cm::ArgvParser::OptionRequiresValue);
	
	cmd.defineOption("show", "Show in detail a single task information -> Expects task ID.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("search", "Search related taskd -> Expects task ID.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("refactor", "Refactor a Task or a report progress of a task Expect integer that represents the task id or a float that represets the report.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("cancel",	"Cancel a task -> Will be reserved and later could be activated.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("enable",	"Enable a canceled task.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("update",	"Update a task -> Will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("deltask", "Delete a task -> Will completely delete from records.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("tags",		"Show all defined tags", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("addtag",		"Tag a task -> Expect a defined tag name, Optional add --taskid {integer}.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("remtag",		"Tag a task -> Expect a defined tag name, Optional add --taskid {integer}.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("newtag",		"Add a new tag -> Will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("deltag",		"Delete a tag -> Will remove the tag from tasks also.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("updatetag",	"Update a tag credentials -> Will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);


	cmd.defineOption("users",		"Show all defined users", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("adduser",		"Add a new user -> Will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);
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
		std::cout << cmd.parseErrorDescription(result);
		if (result == cm::ArgvParser::ParserHelpRequested) {
			exitCode = 0;
		}
		exitCode = 1;
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

	//Help is requested?
	if (result == cm::ArgvParser::ParserHelpRequested) {
		exit(exitCode);
	}

	//Parse Additional Options - does it has a task id predefined ?
	if (cmd.foundOption("taskid")) {
		//Get a task id string:
		moreopt.taskIdStr = cmd.optionValue("taskid");
	}
	else {
		moreopt.taskIdStr = "";
	} // does it has a detail predefined ?
	if (cmd.foundOption("details")) {
		//Get the details level string:
		moreopt.detailsLevel = cmd.optionValue("details");
	}
	else {
		moreopt.detailsLevel = "";
	}

	//Main Tasker Object:
	tasker::TaskerMain* Task = new tasker::TaskerMain(moreopt);
	Task->setPath();
	bool hasobj = Task->loadBase();

	//Main Program Starts here:
	if (run_init) {
		//Check that we are not redeclaring the local object:
		if (!hasobj) {
			Task->createEmpty();
			if (Task->writeObj(false)) {
				Task->printTaskerNotify("Tasker Created!");
				Task->printTaskerInfo("Advice", "Start by adding some tasks by running `--task`.");
			}
		} else {
			Task->printTaskerNotify("No can do! Allready initialized");
			Task->printTaskerInfo("Advice", "You can run '--destroy' to completely remove the instance.");
			exit(exitCodeOk);
		}
	}
	else
	//Handle Operations:
	if (hasobj) {
		//Load current obj:
		std::string fileversion = "";
		int loadParse = Task->loadObj(fileversion);
		if (loadParse == 1) { //permissions
			Task->printTaskerNotify("Oups!");
			Task->printTaskerInfo("Error", "Can't Read the Tasker object check permissions please.");
			exit(exitCodeError);
		}
		else if (run_upgrade) {
			tasker::TaskerUpgrade TaskerUp = tasker::TaskerUpgrade();
			int upgradeCode = TaskerUp.run();
			exit(exitCodeOk);
		}
		else if (loadParse == 2) { //Version mismatch
			Task->printTaskerNotify("Version Mismatch!");
			Task->printTaskerInfo("Error", "The tasker file you are targeting is not compatible with this version.");
			Task->printTaskerInfo("Info", std::string("File Version: ") + fileversion + std::string(" != Tasker Version: ") + std::string(TASKER_VERSION));
			Task->printTaskerInfo("Advice", "You can run '--upgrade' to try repare the Tasker object file.");
			exit(exitCodeError);
		} else { // Ok load options:
			Task->parseOptions(cmd.foundOption("discolor"));
		}

		//Handle set tasks:
		if (cmd.foundOption("task")) {
			// Write new task:
			std::string taskAdd = cmd.optionValue("task");
			Task->setNewTask(taskAdd);
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle cancel a tasks:
		if (cmd.foundOption("cancel")) {
			std::string taskId = cmd.optionValue("cancel");
			if (!Task->cancelTask(taskId, true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Task could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle enable a tasks:
		if (cmd.foundOption("enable")) {
			std::string taskId = cmd.optionValue("enable");
			if (!Task->cancelTask(taskId, false)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Task could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}

		//Handle delete tasks:
		if (cmd.foundOption("deltask")) {
			std::string taskId = cmd.optionValue("deltask");
			if (!Task->deleteTask(taskId)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Task could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle refactor tasks:
		if (cmd.foundOption("refactor")) {
			std::string taskId = cmd.optionValue("refactor");
			if (!Task->refactorTask(taskId)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Task or Report could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle report to task:
		if (cmd.foundOption("report")) {
			// Write new task report:
			std::string report = cmd.optionValue("report");
			if (!Task->reportToTask(report)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Task could not be found or input is invalid. Task may also be canceled.");
				Task->printTaskerInfo("Info", "You can run `--listall {level 1,2}` to see all listed tasks");
				Task->printTaskerInfo("Info", "You can run `--listcancel` to see all canceled tasks");
				Task->printTaskerInfo("Info", "You can enable a task again by running `--enabletask {id}`");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle tags show:
		if (cmd.foundOption("tags")) {
			// Expose the tags list:
			Task->showtags();
		}
		//Handle add tag to a task:
		if (cmd.foundOption("addtag")) {
			// Add a tag to task:
			std::string tag = cmd.optionValue("addtag");
			if (!Task->addtag(tag, moreopt.taskIdStr)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Tag could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle removing a tag from task:
		if (cmd.foundOption("remtag")) {
			// Remove a tag:
			std::string tag = cmd.optionValue("remtag");
			if (!Task->remtag(tag, moreopt.taskIdStr)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Tag could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle defining a new tag:
		if (cmd.foundOption("newtag")) {
			// Add new tag:
			std::string tag = cmd.optionValue("newtag");
			if (!Task->newtag(tag)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Tag name must be at least 2 chars long without spaces and not a reserved name.");
				Task->printTaskerInfo("Info", "Reserved names: " + Task->getReservedTagNames(", ") + ".");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle delete tags:
		if (cmd.foundOption("deltag")) {
			std::string tagId = cmd.optionValue("deltag");
			if (!Task->deltag(tagId)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Tag could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle update tag:
		if (cmd.foundOption("updatetag")) {
			std::string tagId = cmd.optionValue("updatetag");
			if (!Task->updatetag(tagId)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Tag could not be found or input is invalid.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle users show:
		if (cmd.foundOption("users")) {
			// Expose the users list:
			Task->showusers();
		}
		//Handle stats show:
		if (cmd.foundOption("stats")) {
			// Expose the stats list:
			std::string statstype = cmd.optionValue("stats");
			Task->showstats(statstype);
		}
		//Handle add user:
		if (cmd.foundOption("adduser")) {
			// Add new user:
			std::string user = cmd.optionValue("adduser");
			if (!Task->adduser(user)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "User name must be at least 2 chars long without spaces and not a reserved name.");
				Task->printTaskerInfo("Info", "Reserved names: " + Task->getReservedUserNames(", ") + ".");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle delete user:
		if (cmd.foundOption("deluser")) {
			// Delete user:
			std::string user = cmd.optionValue("deluser");
			if (!Task->deluser(user)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "You can't delete all users - must be at least 1 user defined.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle update user:
		if (cmd.foundOption("updateuser")) {
			// Update user creds:
			std::string user = cmd.optionValue("updateuser");
			if (!Task->updateuser(user)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Can't find user: " + user);
				Task->printTaskerInfo("Advice", "You can use `users` to list all users defined.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
		//Handle report to task:
		if (cmd.foundOption("search")) {
			// Write new task report:
			std::string searchVal = cmd.optionValue("search");
			if (!Task->searchvalue(searchVal)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "No search term found.");
				Task->printTaskerInfo("Info", "Try typing `--search \"value\"` to perform a search.");
				Task->printTaskerInfo("Info", "You can limit the results by adding `--limit 10`.");
				exit(exitCodeError);
			}
		}
		//Handle lists:
		if (cmd.foundOption("listtask")		||
			cmd.foundOption("listall")		||
			cmd.foundOption("listdone")		||
			cmd.foundOption("listcancel")	||
			cmd.foundOption("listuser")		||
			cmd.foundOption("listtag")		||
			cmd.foundOption("listopen")		||
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
			} else if (cmd.foundOption("listall")) {
				listlevel = cmd.optionValue("listall");
				which = "all";
			} else if (cmd.foundOption("listdone")) {
				listlevel = cmd.optionValue("listdone");
				which = "done";
			} else if (cmd.foundOption("listcancel")) {
				listlevel = "1";
				which = "cancel";
			} else if (cmd.foundOption("listuser")) {
				listlevel = "2";
				filter = cmd.optionValue("listuser");
				which = "user";
			} else if (cmd.foundOption("listtag")) {
				listlevel = "2";
				filter = cmd.optionValue("listtag");
				which = "tag";
			}
			else if (cmd.foundOption("listopen")) {
				listlevel = cmd.optionValue("listopen");
				which = "open";
			} else if (cmd.foundOption("listtoday")) {
				listlevel = cmd.optionValue("listtoday");
				which = "today";
			}
			//OverWrite list level if set:
			if (moreopt.detailsLevel != "") {
				listlevel = moreopt.detailsLevel;
			}
			//Expose the list:
			if (!Task->list(listlevel, which, filter)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Some error occured can't expose the requested task list.");
				exit(exitCodeError);
			}

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
			} else if (cmd.foundOption("set_optdelete")) {
				state = cmd.optionValue("set_optdelete");
				which = "optdelete";
			}
			else if (cmd.foundOption("set_optloads")) {
				state = cmd.optionValue("set_optloads");
				which = "optloads";
			}

			//Set the option:
			if (!Task->setOption(which, state)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Bad input - can't set option.");
				Task->printTaskerInfo("Advice", "You should use -> true|false OR 1|0.");
				exit(exitCodeError);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to Tasker object.");
				exit(exitCodeError);
			}
		}
	} else {
		Task->printTaskerNotify("Oups!");
		Task->printTaskerInfo("Error", "Can't find Tasker object make sure you ran `--init` in this directory before.");
		exit(exitCodeError);
	}

	if (enable_debug) {
        #ifdef PLATWIN
            system("pause");
            
        #else
            std::cout << "Enter to exit the program: ";
            std::cin.ignore().get(); //Pause Command for Linux Terminal
        #endif
	}

	delete Task;
	return exitCodeOk;
	#endif
}