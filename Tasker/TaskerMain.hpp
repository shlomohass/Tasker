//
//  SIKAst.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#ifndef TaskerMain_hpp
#define TaskerMain_hpp

#include "json.hpp"
#include <string>
#include <vector>

namespace tasker {

	using json = nlohmann::json;

	class TaskerMain
	{
		bool color;

		std::string  basepath;
		std::string  fullpath;
		json thestruct;

	public:
		TaskerMain(bool _color);
		void setPath();
		void _basepath(std::string path);
		void _fullpath(std::string path);
		std::string _basepath();
		std::string _fullpath();
		bool loadBase();
		bool closeBase();
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

		//colors:
		std::string getcolor(const std::string& which);
		std::string getcolor(const std::string& which, const std::string& svalue);
		std::string getcolor(const std::string& which, float fvalue);
		std::string getcolor(const std::string& which, float fvalue, const std::string& svalue);
		char usecolor();

		//Helpers
		float normalizeStatus(std::string str);
		std::time_t getEpochTime(const std::wstring& dateTime);
		int findDefinedUser(const std::string& user);
		std::string getDefindUserName(int index);

		//Task Operations:
		bool setNewTask(const std::string& strTask);
		bool reportToTask(const std::string& strTask);
		
		//General Operations:
		void showusers();
		bool adduser(const std::string& _user);
		bool deluser(const std::string& _user);
		bool updateuser(const std::string& _user);
		bool list(const std::string& level, const std::string& which);
		bool list(const std::string& level, const std::string& which, const std::string& filter);

		virtual ~TaskerMain();
	};

}
#endif /* TaskerMain_hpp */
