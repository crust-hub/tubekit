# Directory Structure

```cpp
.
├── CMakeLists.txt //Build the project with cmake
├── Dockerfile
├── LICENSE
├── README.md
├── bin//What you get after building
│   ├── config
│   │   ├── main.ini//Server configuration parameters
│   │   └── workflow.xml//Work engine configuration
│   ├── log//log file
│   │   └── tubekit.log
│   ├── plugin//The dynamic library packaged by the work engine
│   │   └── libechoplugin.so
│   └── tubekit//executable file
├── client//Work Engine Client Example
│   └── client.py
├── doc//Documentation
│   └── schema.pdf
├── external//3rd party dependencies
│   └── http-parser
├── kill.sh//close tubekit script
├── run.sh//start script
├── src
│   ├── engine//work engine
│   │   ├── context.h//work engine context
│   │   ├── plugin.cpp
│   │   ├── plugin.h//plugin interface to write your own plugin will need to inherit it
│   │   ├── plugin_loader.cpp
│   │   ├── plugin_loader.h//plugin loader,Loading dynamic libraries stored internally
│   │   ├── work.cpp
│   │   ├── work.h//work mapping in workflow.xml
│   │   ├── workflow.cpp
│   │   └── workflow.h//The mapping in workflow.xml provides the external work call interface
|   ├── request
|   |   ├── http_request.cpp//http request
|   |   └── http_request.h
│   ├── main.cpp
│   ├── plugin//Custom Job Engine Plugin
│   │   ├── CMakeLists.txt
│   │   ├── echo_plugin.cpp
│   │   └── echo_plugin.h
│   ├── server//Ready, provide main thread entry
│   │   ├── server.cpp
│   │   └── server.h
│   ├── socket//C socket-based encapsulation
│   │   ├── client_socket.cpp//client socket
│   │   ├── client_socket.h
│   │   ├── even_poller.cpp//epoll tool
│   │   ├── event_poller.h
│   │   ├── server_socket.cpp//server socket
│   │   ├── server_socket.h
│   │   ├── socket.cpp//socket base class
│   │   ├── socket.h
│   │   ├── socket_handler.cpp//Internally uses epoll to listen for connected sockets, and decides which work engine to use for connected sockets
│   │   └── socket_handler.h//The main thread works in an infinite loop, accept runs and creates tasks and sends them to idle task threads
│   ├── system
│   │   ├── system.cpp//Log system initialization, and external configuration file parsing.Created at initialization server
│   │   └── system.h
│   ├── task//Each task carries a socket object and provides a run function, and the idle threads in the thread pool call the run function at an appropriate time.
│   │   ├── http_task.cpp//Inherit thread/task
│   │   ├── http_task.h
│   │   ├── task_factory.h//Provides options to return the different kinds of tasks built
│   │   ├── stream_task.cpp//Inherit thread/task
│   │   └── stream_task.h
│   ├── thread
│   │   ├── auto_lock.cpp//RAII style auto_lock mutex
│   │   ├── auto_lock.h
│   │   ├── condition.cpp//Condition Variable Encapsulation
│   │   ├── condition.h
│   │   ├── mutex.cpp//mutex package
│   │   ├── mutex.h
│   │   ├── task.cpp//carry void*data
│   │   ├── task.h
│   │   ├── task_dispatcher.h//Create a thread pool internally, start its own thread to monitor the incoming task, maintain the task queue and use the idle thread in the thread pool to process the task. task_dispatcher is started by the server object.
│   │   ├── thread.cpp
│   │   ├── thread.h//thread encapsulation
│   │   ├── thread_pool.h//Thread pool template, use the specified type of thread to process the specified type of task. Maintain idle thread queues and busy thread queues, and provide an interface for sending tasks to the outside world.
│   │   ├── work_thread.cpp//Is the thread type created by the thread pool in task_dispatcher to process connected socket tasks
│   │   └── work_thread.h
│   ├── utility//tool
│   │   ├── object_pool.h//object pool
│   │   └── singleton.h//Singleton Pattern Template
│   └── xml//xml file parser
│   |   ├── README.md
│   |   ├── document.cpp
│   |   ├── document.h
│   |   ├── element.cpp
│   |   └── element.h
│   ├── inifile//ini file parser
│   │   ├── README.md
│   │   ├── inifile.cpp
│   │   ├── inifile.h
│   │   ├── value.cpp
│   │   └── value.h
│   ├── log//log system
│   │   ├── README.md
│   │   ├── logger.cpp
│   │   └── logger.h
│   ├── buffer//buffer util
│   │   ├── README.md
│   │   ├── buffer.cpp
│   │   └── buffer.h
│   └── timer//timer tool
│            ├── README.md
│       ├── timer.cpp
│       ├── timer.h
│       ├── timer_manager.cpp
│       └── timer_manager.h
└── test//Test samples of some tool classes
```
