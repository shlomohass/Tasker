# Tasker - Console based task manager
Tasker is CLI based application which allows you to easily manage work tasks of any project. It enables you to create, update tasks. Manage workloads, Group by Users or Tags and many more.

**Basic features:**

 * Create, Update, Delete, Cancel Tasks.
 * Report progress of the work been done and log it.
 * Create and assign users to Tasks.
 * Group Tasks by Tags.
 * Manage users workload.
 * Show progress stats.
 * Quick tasks overview.
 * Listing tasks by due dates, state, assignment and more.

** Table Of Contents **

[TOC]

### Tasker installation

### Tasker Commands
| Command name | Description                    |
| ------------- | ------------------------------ |
| `--help` `-h`      | Display the help window.       |
| [`--init`](#h4-initializing-a-tasker-project)   | **Destroy your computer!**     |
| `--upgarde`   | **Destroy your computer!**     |
| `--debug`   | **Destroy your computer!**     |
| [`--task` `-t`](#h4-creating-a-new-task)   | **Destroy your computer!**     |
| `--taskid`   | **Destroy your computer!**     |
| `--report`   | **Destroy your computer!**     |
| `--show`   | **Destroy your computer!**     |
| `--refactor` `--ref`  | **Destroy your computer!**     |
| `--cancel` `-c`  | **Destroy your computer!**     |
| `--enable` `-e`  | **Destroy your computer!**     |
| `--update` `-u`  | **Destroy your computer!**     |
| `--deltask` `--dt`  | **Destroy your computer!**     |
| `--tags`   | **Destroy your computer!**     |
| `--addtag`   | **Destroy your computer!**     |
| `--remtag`   | **Destroy your computer!**     |
| `--newtag`   | **Destroy your computer!**     |
| `--deltag`   | **Destroy your computer!**     |
| `--updatetag`   | **Destroy your computer!**     |
| `--users`   | **Destroy your computer!**     |
| `--adduser`   | **Destroy your computer!**     |
| `--deluser`   | **Destroy your computer!**     |
| `--updateuser`   | **Destroy your computer!**     |
| `--listtask` `--lt`  | **Destroy your computer!**     |
| `--listall` `--la`  | **Destroy your computer!**     |
| `--listdone` `--ld`  | **Destroy your computer!**     |
| `--listcancel` `--lc`  | **Destroy your computer!**     |
| `--listuser` `--lu`  | **Destroy your computer!**     |
| `--listtag` `--ltg`  | **Destroy your computer!**     |
| `--listopen` `--lo`  | **Destroy your computer!**     |
| `--listtoday`   | **Destroy your computer!**     |
| `--details` `-d`  | **Destroy your computer!**     |
| `--showclosed`   | **Destroy your computer!**     |
| `--discolor`   | **Destroy your computer!**     |
| `--set_optcolor`   | **Destroy your computer!**     |
| `--set_optdelete`   | **Destroy your computer!**     |
| `--set_optloads`   | **Destroy your computer!**     |
| `--stats`   | **Destroy your computer!**     |

### Tasker documentation

#### Initializing a Tasker project
Command: `$ Tasker --init`
 > A Tasker object must be intialized in the root folder of a target project.
 This init command creates a Tasker file called `.Tasker` which stores the json object.
 You will be walked through an interactive step by step menu to configure your Tasker project.

#### Creating a new task
Command: `$ Tasker --task "The new task title"`
 > Whenever you want to create a new task you use the --task command. When creating a new task - The Tasker program is writing to the json object that is stored in the .Tasker file.
 You will be walked through an interactive step by step menu to define the task.
