# Minimal-Event-Loop-in-C
An Implementation of a basic Event Loop in C that uses cooperative time - interval based scheduling to execute tasks while also maintaining an event queue to manipulate the control flow of the loop.  
Allows :-  
* Priority based scheduling
* Time interval based task scheduling
* A minimal event queue system for the programmer to post events to the loop
* Can easily remove or add priorities just by adding values to the task_priority enum
