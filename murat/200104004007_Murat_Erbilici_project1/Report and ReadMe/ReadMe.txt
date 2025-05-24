In the kernalMain in kernel.cpp, I created task object as init task and add them to tasks array using addTask.
-taskPartA: for part A.
-Task st_1: for first strategy in part B.
-Task st_2: for second strategy in part B.
-Task st_3: for third strategy in part B.
-Task st_dynamic: for Dynamic Priority Strategy in part B.
-Task testpidexecve: to test getPid and execve.

I couldn't implement part C. For testing each part, please make comment line all parts expect The task object that you will test.

In TaskManager::Schedule, I commented out the schedule algorithms. Please do these while testing different strategies:	
-If you test partA, st_1 and st_2, please return Schedule_Strategy_1_2 function in the TaskManager::Schedule function in multitasking.cpp.
-If you test st_3 please return Schedule_Strategy_3 function in the TaskManager::Schedule function in multitasking.cpp.
-If you test st_dynamic please return Schedule_Strategy_Dynamic function in the TaskManager::Schedule function in multitasking.cpp.

In some test cases, I used two collatz program to make programs longer. For strategy3 and dynamic strategy parts, Only first collatz program has the lowest priority, other collatz program has same priority as the other programs(liner search, long_running program etc.).

In begining of the each schedule functions, There is a function to print process tables. I commented them to make the output clear. If you want to see process tables, Please uncomment these lines.

COMPILING: Please write 'make' and add the iso file to your vm.