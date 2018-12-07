//
//  SIKAst.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#ifndef TaskerMain_hpp
#define TaskerMain_hpp

#include "TaskerAdd.hpp"
#include "json3.4.0.hpp"
#include <string>
#include <vector>
#include <iomanip>      // std::get_time
#include <time.h>

namespace tasker {

	using json = nlohmann::json;

	struct exists {
		int type; // 0 - notset, 1 - task, 2 - report, 3 - note
		float id;
		int taskId;
		int reportId;
	};
	class TaskerMain
	{
		//Options:
		tasker::moreOpt opt;

		//Containers:
		std::string  basepath;
		std::string  fullpath;
		json thestruct;

	public:

		TaskerMain(tasker::moreOpt moreopt);
		void setPath();
		void _basepath(std::string path);
		void _fullpath(std::string path);
		std::string _basepath();
		std::string _fullpath();
		bool loadBase();
		bool closeBase();
		void parseOptions(bool colors_override);
		bool setOption(const std::string& which, const std::string& state);
		void createEmpty();
		void createEmpty(json structure);
		std::string getSerialized();
		std::string getSerialized(int pretty_spaces);
		bool checkWriteObj(bool dir);
		bool checkWriteObj(std::string& path);
		bool writeObj(bool newobj);
		bool loadObj();

		void printTaskerNotify(const std::string& mes);
		void printTaskerInfo(const std::string& type, const std::string& mes);


		std::string getcurdatetime();
		std::string getcurdatetime(const std::string& format);
		int parseDateParts(const std::string& datestr, int& day, int& month, int& year, int& hours, int& minutes, int& seconds);
		int parseDateParts(const std::string& datestr, const std::string& format, int& day, int& month, int& year, int& hours, int& minutes, int& seconds);
		std::string createDateFromInts(int day, int mon, int year, int hour, int min, int sec);
		bool isFloat(std::string str);
		float getFloat(std::string str);
		std::vector<int> parseTaskListStr(std::string str);

		//colors:
		std::string getcolor(const std::string& which);
		std::string getcolor(const std::string& which, const std::string& svalue);
		std::string getcolor(const std::string& which, float fvalue);
		std::string getcolor(const std::string& which, float fvalue, const std::string& svalue);
		char usecolor();

		//Helpers
		float normalizeStatus(std::string str);
		std::string getUserString(std::vector<std::string>& users, std::string prefix, bool addNotAssigned);
		std::time_t getEpochTime(const std::wstring& dateTime);
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
		exists findRow(const std::string& strId);
		bool onlyDigits(const std::string str);

		//Console get:
		std::vector<std::string> getUserName(bool& push_plan, bool allowskip, int taskIdForSkip, const std::string& userFixStr);
		std::string getStrMessage(const std::string& err);
		std::string getStrDate(const std::string& err, bool allowSkip);
		int         getLoad(const std::string& err);
		std::vector<std::string> getTags(const std::string& err);
		std::string getStrVersion(bool& push_plan, bool allowskip, const std::string& versionForSkip);

		//Task Operations:
		bool setNewTask(const std::string& strTask);
		bool reportToTask(const std::string& strTask);
		bool refactorTask(const std::string& strTask);
		bool cancelTask(const std::string& strTask, bool state);
		bool deleteTask(const std::string& strTask);

		//General Operations:
		void showtags();
		bool addtag(const std::string& _tag, const std::string& strTask);
		bool remtag(const std::string& _tag, const std::string& strTask);
		bool newtag(const std::string& _tag);
		bool deltag(const std::string& _tag);
		bool updatetag(const std::string& _tag);

		void showusers();
		bool showstats(const std::string& type);
		bool adduser(const std::string& _user);
		bool deluser(const std::string& _user);
		bool updateuser(const std::string& _user);
		bool list(const std::string& _level, const std::string& which);
		bool list(const std::string& _level, const std::string& which, const std::string& filter);

		virtual ~TaskerMain();
	};

}
#endif /* TaskerMain_hpp */
