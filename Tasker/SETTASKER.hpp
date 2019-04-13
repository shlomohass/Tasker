/*
* File:   SETTASKER.hpp
* Author: shlomi
*
*/


#ifndef SETTASKER_H
    #define	SETTASKER_H

#ifdef USEPLATLINUX
    #define PLATLINUX
#endif
#ifdef USEPLATWIN
    #define PLATWIN
#endif
#if !defined(PLATLINUX) && !defined(PLATWIN)
    #define PLATOTHER
#endif

#define TASKER_VERSION "1.0.4"
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

#ifndef TASKER_BASELIST_LEVEL
/**
* Set Default list view depth level -> 1,2
*/
#define TASKER_BASELIST_LEVEL "2" 
#endif

#ifndef TASKER_RESULT_LIMIT
/**
* Set Result limits
*/
#define TASKER_RESULT_LIMIT 100 
#endif

#ifndef TASKER_NAMES_PREFIX
/**
* Set Default obj name
*/
#define TASKER_NAME_MIN_LEN 2
#define TASKER_USER_PREFIX "@"
#define TASKER_USER_NOT_ASSIGNED "not assigned"
#define TASKER_TAG_PREFIX "#"
#define TASKER_TAG_NOT_TAGGED "not tagged"
#endif

#ifndef TASKER_SPLIT_DELI
/**
* Set the list delimeter for inputs:
*/
#define TASKER_SPLIT_DELI ","
#define TASKER_SPLIT_DELI_CHAR ','
#define TASKER_SPLIT_DELI_CHAR_SPACE ", "
#define TASKER_DELI_OF_CHAINED_DETAILS ":"
#endif

#ifndef TASKER_BASIC_ELE
/**
* Set some default objects:
*/
#define TASKER_BASIC_TYPE_NAME "task"
#define TASKER_BASIC_TYPE_DESC "basic task"
#define TASKER_FIXED_EMPTY_TASK_DESC "missing task description"
#endif

/**
* Set the list delimeter for inputs:
*/
#ifdef PLATLINUX
	#define TASKER_BAR_LENGTH 20
	#define TASKER_BAR_OPEN ""
	#define TASKER_BAR_CLOSE ""
	#define TASKER_BAR_EMPTY "\u2591"
	#define TASKER_BAR_FULL "\u2593"
	#define TASKER_BAR_ARROW "\u2593"
	#define TASKER_BAR_CURSSOR "\u2593"
    #define TASKER_FOLDER_SEP "/"
#endif
#ifdef PLATWIN
	#define TASKER_BAR_LENGTH 20
	#define TASKER_BAR_OPEN "["
	#define TASKER_BAR_CLOSE "]"
	#define TASKER_BAR_EMPTY " "
	#define TASKER_BAR_FULL "-"
	#define TASKER_BAR_ARROW ">"
	#define TASKER_BAR_CURSSOR "|"
    #define TASKER_FOLDER_SEP "\\"
#endif
#ifdef PLATOTHER
	#define TASKER_BAR_LENGTH 20
	#define TASKER_BAR_OPEN "["
	#define TASKER_BAR_CLOSE "]"
	#define TASKER_BAR_EMPTY " "
	#define TASKER_BAR_FULL "-"
	#define TASKER_BAR_ARROW ">"
	#define TASKER_BAR_CURSSOR "|"
    #define TASKER_FOLDER_SEP "\\"
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
#define TASKER_COLOR_HIGHLIGHT  "[0;1;43m"
#endif

#endif	/* SETTASKER_H */
