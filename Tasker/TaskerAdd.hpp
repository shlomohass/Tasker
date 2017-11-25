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

	const std::vector<std::string> reserve_user_names = { "default", "unknown" };
	const std::vector<std::string> reserve_tag_names  = { "default", "not tagged" };
}

#endif /* TaskerAdd_hpp */
