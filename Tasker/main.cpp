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

void setMainArgs(cm::ArgvParser *cmd, bool *run_init, bool *enable_debug) {
	if (cmd->foundOption("debug")) {
		*enable_debug = true;
	}
	if (cmd->foundOption("init")) {
		*run_init = true;
	}
	return;
}


int main(int argc, char** argv) {

	//Define args and project settings:
	int exitCode = 0;
	std::string filepath = "";
	cm::ArgvParser cmd;
	bool enable_debug = TASKER_DEBUG;
	bool use_colors = true;
	bool run_init = false;
	int  execution_result = 0;

	cmd.addErrorCode(0, "Success");
	cmd.addErrorCode(1, "Error");
	cmd.setIntroductoryDescription("Tasker Manager - version: " + std::string(TASKER_VERSION) + " - By: " + std::string(TASKER_AUTHOR));
	
	cmd.setHelpOption("h", "help", "Print this help page");
	cmd.defineOption("init", "Initialize a Tasker object in the current path", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("debug", "enable debug.", cm::ArgvParser::NoOptionAttribute);
	
	cmd.defineOption("task", "Add a new task -> will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("report", "Report progress to a task -> will ask for more options snd settings interactivly", cm::ArgvParser::OptionRequiresValue);
	
	cmd.defineOption("users", "Show all users defined", cm::ArgvParser::NoOptionAttribute);
	cmd.defineOption("adduser", "Add a new user -> will ask for more options interactivly", cm::ArgvParser::OptionRequiresValue);

	cmd.defineOption("listall", "List all tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listdone", "List all closed / finished tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listuser", "List user tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listopen", "List all open tasks -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	cmd.defineOption("listtoday", "List tasks that are due to today -> expect integer for display level", cm::ArgvParser::OptionRequiresValue);
	
	cmd.defineOption("discolor", "Disable colored console text.", cm::ArgvParser::NoOptionAttribute);

	cmd.defineOptionAlternative("task", "t");
	cmd.defineOptionAlternative("report", "r");
	cmd.defineOptionAlternative("listall", "la");
	cmd.defineOptionAlternative("listdone", "ld");
	cmd.defineOptionAlternative("listuser", "lu");
	cmd.defineOptionAlternative("listopen", "lo");
	cmd.defineOptionAlternative("listtoday", "lt");

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
		setMainArgs(&cmd, &run_init, &enable_debug);
	}

	//Help is requested?
	if (result == cm::ArgvParser::ParserHelpRequested) {
		exit(exitCode);
	}

	//Set colored output:
	if (cmd.foundOption("discolor")) {
		use_colors = false;
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
			exit(exitCode);
		}
	} else
	//Handle Operations:
	if (hasobj) {
		//Load current obj:
		if (!Task->loadObj()) {
			Task->printTaskerNotify("Oups!");
			Task->printTaskerInfo("Error", "Can't load Tasker object make sure you ran `--init` in this directory before.");
			exit(exitCode);
		}
		//Handle set tasks:
		if (cmd.foundOption("task")) {
			// Write new task:
			std::string taskAdd = cmd.optionValue("task");
			Task->setNewTask(taskAdd);
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to object.");
				exit(exitCode);
			}
		}
		//Handle report to task:
		if (cmd.foundOption("report")) {
			// Write new task report:
			std::string report = cmd.optionValue("report");
			if (!Task->reportToTask(report)) {
				std::cout << " * Tasker said Oups!" << std::endl << "     Error -> Task could not be found or input is invalid." << std::endl;
				exit(exitCode);
			}
			if (!Task->writeObj(true)) {
				Task->printTaskerNotify("Oups!");
				Task->printTaskerInfo("Error", "Could not write to object.");
				exit(exitCode);
			} else {

			}
		}
		//Handle users show:
		if (cmd.foundOption("users")) {
			// Expose the users list:
			Task->showusers();
		}
		//Handle user add:
		if (cmd.foundOption("adduser")) {
			// Expose the users list:
			
		}
		//Handle lists:
		if (cmd.foundOption("listall") ||
			cmd.foundOption("listdone") ||
			cmd.foundOption("listuser") ||
			cmd.foundOption("listopen") ||
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
				exit(exitCode);
			}

		}
	} else {
		Task->printTaskerNotify("Oups!");
		Task->printTaskerInfo("Error", "Can't find Tasker object make sure you ran `--init` in this directory before.");
		exit(exitCode);
	}

	if (enable_debug) {
		system("pause");
	}

	delete Task;
	return execution_result;
}