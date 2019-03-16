//
//  TaskerAdd.hpp
//  Tasker
//
//  Created by Shlomo Hassid on 06/11/2017 .
//  Copyright © 2017 Shlomo Hassid. All rights reserved.
//


#ifndef TaskerAdd_hpp
#define TaskerAdd_hpp

#include <iostream>
#include <vector>

namespace tasker
{
	enum OperatingModes { ODEBUG, ONORMAL };

	enum ExecCodes { SUCCESS, GENEROOR };

	const std::vector<std::string> RESERVE_USER_NAMES = { "default", "unknown", "?", "@", "#" };
	const std::vector<std::string> RESERVE_TAG_NAMES = { "default", "not tagged", "?", "@", "#" };

	struct moreOpt {
		std::string taskIdStr;
		bool showclosed;
		bool use_colors;
		bool enable_loads;
		bool del_items;
		std::string detailsLevel;
	};

	struct exists {
		int type; // 0 - notset, 1 - task, 2 - report, 3 - note
		float id;
		int taskId;
		int reportId;
	};
}

#endif /* TaskerAdd_hpp */
