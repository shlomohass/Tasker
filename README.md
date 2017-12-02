# **Tasker - console based task manager.**
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

### **Tasker installation:**


###
###
###
### **Tasker documentation:**
###
**Initializing a Tasker project:**
 > The Tasker must be intialized in the root folder of a project.
 This command creates a Tasker Object called `.Tasker` which stores a json object.
 Usage: `$ Tasker --init`
 You will be walked through an interactive step by step menu to configure your Tasker project.
 ###
**Creating a new task:**
 > When creating a new task - The Tasker is basically writing to the Tasker object that is located in the current directory.
 Usage: `$ Tasker --task "The task to be created"`
 You will be walked through an interactive step by step menu to define the task.
