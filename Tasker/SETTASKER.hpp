/*
* File:   SETTASKER.hpp
* Author: shlomi
*
*/


#ifndef SETTASKER_H
#define	SETTASKER_H

#define TASKER_VERSION "1.0.0"
#define TASKER_AUTHOR "Shlomi Hassid"

#ifndef TASKER_DEBUG
/**
* Set Defaul debuger output
*/
#define TASKER_DEBUG true
#endif


#ifndef TASKER_OBJNAME
/**
* Set Default obj name
*/
#define TASKER_OBJNAME ".tasker"
#endif

#ifndef TASKER_SPLIT_DELI
/**
* Set the list delimeter for inputs:
*/
#define TASKER_SPLIT_DELI ","
#endif

/**
* Set the list delimeter for inputs:
*/

	#ifdef LINUX
		#define TASKER_BAR_LENGTH 20
		#define TASKER_BAR_OPEN ""
		#define TASKER_BAR_CLOSE ""
		#define TASKER_BAR_EMPTY "\u2591"
		#define TASKER_BAR_FULL "\u2593"
		#define TASKER_BAR_ARROW "\u2593"
	#else
		#define TASKER_BAR_LENGTH 20
		#define TASKER_BAR_OPEN "["
		#define TASKER_BAR_CLOSE "]"
		#define TASKER_BAR_EMPTY " "
		#define TASKER_BAR_FULL "-"
		#define TASKER_BAR_ARROW ">"
	#endif

#ifndef TASKER_CONSOLE_COLORS
/**
* Set Console colors:
*/
#define TASKER_CONSOLE_COLORS
#define TASKER_COLOR_RESET		"[0m"
#define TASKER_COLOR_BLACK		"[0;30m"
#define TASKER_COLOR_GREY		"[0;1;30m"
#define TASKER_COLOR_RED		"[0;1;31m"
#define TASKER_COLOR_GREEN		"[0;32m"
#define TASKER_COLOR_YELLOW		"[0;1;33m"
#define TASKER_COLOR_MAGENTA	"[0;1;35m"
#define TASKER_COLOR_BLUE		"[0;1;34m"
#define TASKER_COLOR_CYAN		"[0;1;36m"
#endif

#endif	/* SETTASKER_H */
