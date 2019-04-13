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

	const std::vector<std::string> OBJECT_ATTR = { "name", "description", "note", "system", "users", "tags", "version", "types", "tasks" };

	//Taser Option struct:
	struct moreOpt {
		std::string taskIdStr;
		bool showclosed;
		bool use_colors;
		bool enable_loads;
		bool del_items;
		std::string detailsLevel;
		int limit;
	};

	struct exists {
		int type; // 0 - notset, 1 - task, 2 - report, 3 - note
		float id;
		int taskId;
		int reportId;
	};

	//Finalize struct:
	struct finalOp {
		std::vector<std::string> infoMesNotify;
		std::vector<std::string> infoMesFinal;
		std::vector<std::string> infoMesAdvice;
		std::vector<std::string> infoMesError;
		bool setModified;
		bool setUsed;
		int exitCodeError;
		int exitCodeOk;
		int exitCode;
		//constructor
		finalOp() : setModified(false), setUsed(false), exitCodeError(1), exitCodeOk(1), exitCode(1) {}
		void set(bool mod, bool used) { this->setModified = mod; this->setUsed = used; }
	};

	struct intret {
		int code;
		int result;
		std::string str;
		intret() : code(0), result(0) {}
		intret(int _code) : code(_code), result(0) {}
		intret(const std::string& _str) : code(0), result(0), str(_str) {}
		intret(int _code, const std::string& _str) : code(_code), result(0), str(_str) {}
		intret(int _code, int _result) : code(_code), result(_result) {}
		intret(int _code, int _result, const std::string& _str) : code(_code), result(_result), str(_str) {}
	};
}

#endif /* TaskerAdd_hpp */
