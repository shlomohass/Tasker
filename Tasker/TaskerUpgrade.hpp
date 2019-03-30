//
//  TaskerUpgrade.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#ifndef TaskerUpgrade_hpp
#define TaskerUpgrade_hpp

#include "TaskerBase.hpp"

#include <string>
#include <vector>

namespace tasker {

	using json = nlohmann::json;

	class TaskerUpgrade : public TaskerBase
	{

	public:

		TaskerUpgrade();

		int run(int &removedTags, int &removedTasks);
		bool fix(const std::string& attributesStr);

		//Procedures:
		bool check_name();
		bool check_desc();
		bool check_note();
		bool check_taskersys();
		bool check_users();
		int  check_tags();
		bool check_version();
		bool check_types();
		int  check_tasks();
		bool check_tasks_meta();

		//Helpers:
		std::string getFromUser(const std::string& mes);
		virtual ~TaskerUpgrade();
	};

}
#endif /* TaskerUpgrade_hpp */