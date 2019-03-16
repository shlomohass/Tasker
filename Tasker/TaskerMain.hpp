//
//  SIKAst.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#ifndef TaskerMain_hpp
#define TaskerMain_hpp

#include "TaskerBase.hpp"

#include <string>
#include <vector>

namespace tasker {

	using json = nlohmann::json;

	class TaskerMain : public TaskerBase
	{

		//Containers:
		std::string  basepath;
		std::string  fullpath;

	public:

		TaskerMain(tasker::moreOpt moreopt);

		//Object handling:
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
		int loadObj(std::string& version);

		//Task Operations:
		bool setNewTask(const std::string& strTask);
		bool reportToTask(const std::string& strTask);
		bool refactorTask(const std::string& strTask);
		bool cancelTask(const std::string& strTask, bool state);
		bool deleteTask(const std::string& strTask);

		//Tasker Operations:
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
		bool searchvalue(const std::string& _value);
		int searchAndPrint(const std::string& str, const std::string& value, const std::string& rowindex, int searchtype);
		bool list(const std::string& _level, const std::string& which);
		bool list(const std::string& _level, const std::string& which, const std::string& filter);

		virtual ~TaskerMain();
	};

}
#endif /* TaskerMain_hpp */
