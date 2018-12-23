//
//  TaskerUpgrade.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#ifndef TaskerBase_hpp
#define TaskerBase_hpp

#include "SETTASKER.hpp"
#include "TaskerAdd.hpp"
#include "json3.4.0.hpp"

#include <string>
#include <vector>
#include <iomanip>      // std::get_time
#include <time.h>

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

	class TaskerBase
	{
		public:

			//Validation stuff:
			bool checkValidUserName(const std::string& name);
			bool checkValidTagName(const std::string& name);
			bool onlyDigits(const std::string str);

			//Time related
			std::string getcurdatetime();
			std::string getcurdatetime(const std::string& format);
			int parseDateParts(const std::string& datestr, int& day, int& month, int& year, int& hours, int& minutes, int& seconds);
			int parseDateParts(const std::string& datestr, const std::string& format, int& day, int& month, int& year, int& hours, int& minutes, int& seconds);
			std::string createDateFromInts(int day, int mon, int year, int hour, int min, int sec);
			std::time_t getEpochTime(const std::wstring& dateTime);
			

			//console colors:
			std::string getcolor(const std::string& which);
			std::string getcolor(const std::string& which, const std::string& svalue);
			std::string getcolor(const std::string& which, float fvalue);
			std::string getcolor(const std::string& which, float fvalue, const std::string& svalue);
			char usecolor();

			//Console get:
			std::vector<std::string> getUserName(bool& push_plan, bool allowskip, int taskIdForSkip, const std::string& userFixStr);
			std::string getStrMessage(const std::string& err);
			std::string getStrDate(const std::string& err, bool allowSkip);
			int         getLoad(const std::string& err);
			std::vector<std::string> getTags(const std::string& err);
			std::string getStrVersion(bool& push_plan, bool allowskip, const std::string& versionForSkip);


			//Helpers
			bool isFloat(std::string str);
			float getFloat(std::string str);
			float normalizeStatus(std::string str);
			std::string getUserString(std::vector<std::string>& users, std::string prefix, bool addNotAssigned);
			bool findDefinedUser(const std::string& user, bool multi);
			int findDefinedUser(const std::string& user);
			bool findDefinedTag(const std::string& tag, bool multi);
			int findDefinedTag(const std::string& tag);
			std::string getDefindUserName(int index);
			std::string getReservedUserNames(const std::string& deli);
			std::string getReservedTagNames(const std::string& deli);
			std::string trim_gen(const std::string& str, const char rem);
			void cleanString(std::string& str, const std::vector<char>& rem);
			std::vector<std::string> splitString(const std::string &text, char sep);
			std::vector<int> parseTaskListStr(std::string str);

			virtual ~TaskerBase();
	};

}
#endif /* TaskerBase_hpp */