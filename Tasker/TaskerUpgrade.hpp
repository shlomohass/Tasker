//
//  TaskerUpgrade.hpp
//  SIK
//
//  Created by Shlomo Hassid on 06/11/2017.
//  Copyright 2017 Shlomo Hassid. All rights reserved.
//

#ifndef TaskerUpgrade_hpp
#define TaskerUpgrade_hpp

#include "TaskerAdd.hpp"
#include "json3.4.0.hpp"
#include <string>
#include <vector>

namespace tasker {

	using json = nlohmann::json;

	class TaskerUpgrade
	{
		json* thestruct;
	public:

		TaskerUpgrade(json* _thestruct);
		int run();
		
		//Procedures:
		bool check_name();
		bool check_desc();
		bool check_note();
		bool check_taskersys();
		bool check_users();
		bool check_tags();
		//Helpers:
		std::string getFromUser(const std::string& mes);
		bool promptUser(const std::string& mes);
		virtual ~TaskerUpgrade();
	};

}
#endif /* TaskerUpgrade_hpp */