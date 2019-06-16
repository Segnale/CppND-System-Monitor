# CppND-System-Monitor

Starter code for System Monitor Project in the C++ OOP course. Most of the code is provided for you here. Follow along with the classroom lesson to fill out the `ProcessParser` and `Process` classes to complete your project!

## To setup and compile in Udacity Ubuntu workspace:

1. Clone repository into `/home/workspace/`
```
cd /home/workspace/
git clone https://github.com/udacity/CppND-System-Monitor
```
2. Follow along with the lesson and make sure you complete the `ProcessParser` and `Process` classes before attempting to run the project.

3. Install `ncurses` package
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```
4. Compile and run
```
g++ -std="c++17" main.cpp -lncurses
./a.out
```

## System Monitor
The System Monitor application makes use of the `ncurses` package and run straight in the Linux Terminal. It is made of two area, a top window with generic system informations
* OS info
* CPU usage
* Total RAM Usage
* Number of processes active

...and a bottom window with the list of the 10 latest processes actives with a list of their most relevant informations:
* pid
* user of the process
* cpu usage
* memory usage
* process Up Time

![System_Monitor](assets/Monitor,png)
