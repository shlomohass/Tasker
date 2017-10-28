//
//  main.cpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright © 2017 Shlomo Hassid. All rights reserved.
//  
//  Command Line Examples:
//

#ifdef __GNUC__
	#define LINUX
#else
	#define WINDOWS
#endif

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

namespace cm = CommandLineProcessing;

void setMainArgs(cm::ArgvParser *cmd, bool *run_init, bool *enable_debug, bool *use_colors) {
	if (cmd->foundOption("debug")) {
		*enable_debug = true;
	}
	if (cmd->foundOption("init")) {
		*run_init = true;
	}
	if (cmd->foundOption("discolor")) {
		*use_colors = false;
	}
	return;
}


int main(int argc, char** argv) {

	//Define args and project settings:
	int exitCodeError	= 1;
	int exitCodeOk		= 0;
	int exitCode		= exitCodeOk;

	std::string filepath = "";
	cm::ArgvParser cmd;
	bool enable_debug	= TASKER_DEBUG;
	bool use_colors		= true;
	bool enable_loads	= true;
	bool run_init		= false;

	cmd.addErrorCode(exitCodeOk,	"Success"	);
	cmd.addErrorCode(exitCodeError, "Error"		);
	cmd.setIntroductoryDescription("Tasker Manager - version: " + std::string(TASKER_VERSION) + " - By: " + std::string(TASKER_AUTHOR));
	
	cmd.setHelpOption("h", "help", "Print this help page");
	cmd.defineOption("init", "Initialize a Tasker object in the current path", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("debug", "enable debug.", cm::ArgvParser::NoOptionAttribute);
	
	cmd.defineOption("task", "Add a new task -> will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("report", "Report progress to a task -> will ask for more options snd settings interactivly", cm::ArgvParser::OptionRequiresValue);
	
	cmd.defineOption("cancel", "Cancel a task -> will be reserved and later could be activated.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("enable", "Enable a cnaceled task.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("deltask", "Delete a task -> will completely delete from records", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("users", "Show all users defined", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("adduser", "Add a new user -> will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("deluser", "Delete a user -> will remove the user from tasks also.", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("updateuser", "Update a user credentials -> will ask for more options interactivly.", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("listall", "List all tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listdone", "List all closed / finished tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listcancel", "List all canceled tasks.", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("listuser", "List user tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listopen", "List all open tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listtoday", "List tasks that are due to today -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	
	cmd.defineOption("discolor", "Disable colored console text for one execution only.", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("set_optcolor", "Set option whether use colored console text. Expect true|false OR 1|0", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("set_optdelete", "Set option whether to allow task delete. Expect true|false OR 1|0", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("set_optloads", "Set option whether to use task loads. Expect true|false OR 1|0", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOptionAlternative("task",			"t"	);
	cmd.defineOptionAlternative("report",		"r"	);
	cmd.defineOptionAlternative("cancel",		"c"	);
	cmd.defineOptionAlternative("enable",		"e"	);
	cmd.defineOptionAlternative("deltask",		"dt");
	cmd.defineOptionAlternative("listall",		"la");
	cmd.defineOptionAlternative("listdone",		"ld");
	cmd.defineOptionAlternative("listcancel",	"lc");
	cmd.defineOptionAlternative("listuser",		"lu");
	cmd.defineOptionAlternative("listopen",		"lo");
	cmd.defineOptionAlternative("listtoday",	"lt");

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
		setMainArgs(&cmd, &run_init, &enable_debug, &use_colors);
	}

	//Help is requested?
	if (result == cm::ArgvParser::ParserHelpRequested) {
		exit(exitCode);
	}

	//Main Tasker Object:
	tasker::TaskerMain* Task = new tasker::TaskerMain(use_colors);
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
	} else
	//Handle Operations:
	if (hasobj) {
		//Load current obj:
		if (!Task->loadObj()) {
			Task->printTaskerNotify("Oups!");
			Task->printTaskerInfo("Error", "Can't load Tasker object make sure you ran `--init` in this directory before.");
			exit(exitCodeError);
		} else {
			//Get local options:
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
		//Handle users show:
		if (cmd.foundOption("users")) {
			// Expose the users list:
			Task->showusers();
		}
		//Handle add user:
		if (cmd.foundOption("adduser")) {
			// Add new user:
			std::string user = cmd.optionValue("adduser");
			if (!Task->adduser(user)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "User name must be at least 2 chars long without spaces and not a reserve name.");
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
			// Delete user:
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
		//Handle lists:
		if (cmd.foundOption("listall")		||
			cmd.foundOption("listdone")		||
			cmd.foundOption("listcancel")	||
			cmd.foundOption("listuser")		||
			cmd.foundOption("listopen")		||
			cmd.foundOption("listtoday")
		) {
			// Get level:
			std::string listlevel; 
			std::string which;
			std::string filter = "";
			//Grab:
			if (cmd.foundOption("listall")) {
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
			} else if (cmd.foundOption("listopen")) {
				listlevel = cmd.optionValue("listopen");
				which = "open";
			} else if (cmd.foundOption("listtoday")) {
				listlevel = cmd.optionValue("listtoday");
				which = "today";
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
		system("pause");
	}

	delete Task;
	return exitCodeOk;
}