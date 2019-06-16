#pragma once

#include <algorithm>
#include <iostream>
#include <math.h>
#include <thread>
#include <chrono>
#include <iterator>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cerrno>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"


using namespace std;

class ProcessParser{

private:
    ifstream stream;
    static vector<string> searcher(string path, string ref);

public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static string getVmSize(string pid);
    static string getCpuPercent(string pid);
    static long int getSysUpTime();
    static string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysActiveCpuTime(vector<string> values);
    static float getSysIdleCpuTime(vector<string> values);
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static string PrintCpuStats(vector<string> values1, vector<string>values2);
    static bool isPidExisting(string pid);

};

// TODO: Define all of the above functions below:

vector<string> ProcessParser::searcher(string path, string ref){

    string line;
    ifstream stream = Util::getStream(path);
    while(getline(stream, line)) {
        if (line.compare(0,ref.size(), ref) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> result(beg,end);
            break;
        }
    };

    return result;

}


string ProcessParser::getVmSize(string pid) {

    vector<string> values;
    float result;

    string Path = Path::basePath()+pid+Path::statusPath();
    values = ProcessParser::searcher(Path, "VmData");
    result = stof(values[1])/float(1024^2);
    
    return to_string(result);
}



string ProcessParser::getCpuPercent(string pid) {

    string line;
    string value;
    float result;
    ifstream stream = Util::getStream((Path::basePath()+pid+"/"+Path::statPath()));
    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg,end);
    float stime = stof(values[14]);
    float cutime = stof(values[15]);
    float cstime = stof(values[16]);
    float starttime = stof(values[21]);

    float utime =  stof(ProcessParser::getProcUpTime(pid));
    float uptime = ProcessParser::getSysUpTime();

    float freq = sysconf(_SC_CLK_TCK);

    float total_time = utime + stime + cutime + cstime;
    float seconds = utime -(starttime/freq);
    result = 100*((total_time/freq)/seconds);

    return to_string(result);
}



string ProcessParser::getProcUpTime(string pid){
    
    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid + "/"+Path::statPath()));
    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg,end);
    float uptime = stof(values[13]);

    float freq = sysconf(_SC_CLK_TCK);

    float result = uptime/freq;
    return to_string(result);
}



long int ProcessParser::getSysUpTime(){
    
    string line;
    ifstream stream = Util::getStream((Path::basePath() + Path::upTimePath()));
    getline(stream, line);
    istringstream buf(line);
    istream_iterator<string> beg(buf), end;
    vector<string> values(beg,end);
    // number in the extracted string is truncked to int
    int result = stoi(values[0]);

    return result;
}



string ProcessParser::getProcUser(string pid){

    string line;
    string name = "Uid:";
    string value;
    string Uid;
    string User;

    // Get Uid 
    ifstream stream = Util::getStream((Path::basePath()+pid+Path::statusPath()));
    while(getline(stream, line)) {
        if (line.compare(0,name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            Uid = values[1];
            break;
        }
    };

    // Get User
    stream = Util::getStream("/etc/passwd");
    while(getline(stream, line)) {
        if (line.find(":x:" + Uid) != string::npos) {
            User = line.substr(0,line.find(":x:"+Uid));
            break;
        }
    };

    return User;
}



vector<string> ProcessParser::getPidList(){
    // Return a PID list of process running in the machine as a vector
    
    //Directory of the processes
    DIR* dir;
    //vector process id container declaration
    vector<string> pidContainer;

    //ensure the process directory can be opened
    if(!(dir = opendir("/proc")))
        throw runtime_error(strerror(errno));

    while (dirent* dirp = readdir(dir)){
        //check if it is a directory
        if(dirp->d_type != DT_DIR)
            continue;
        //check if all the char are digist (as per common pid) using lambda function
        if(all_of(dirp->d_name, dirp->d_name + strlen(dirp->d_name),[](char c){return isdigit(c);})){
            pidContainer.push_back(dirp->d_name);
        }

        //close the directory and check it closed
        if(closedir(dir))
            throw runtime_error(strerror(errno));

        return pidContainer;
    };

}



string ProcessParser::getCmd(string pid){

    string line;
    ifstream stream = Util::getStream((Path::basePath() + pid +Path::cmdPath()));
    getline(stream, line);

    return line;
}



int ProcessParser::getNumberOfCores(){
    string line;
    string name = "cpu cores";
    int result;
    ifstream stream = Util::getStream((Path::basePath()+"cpuinfo"));
    while(getline(stream, line)) {
        if (line.compare(0,name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = stoi(values[3]);
            break;
        }
    };
    return result;
}



vector<string> ProcessParser::getSysCpuPercent(string coreNumber){

    string line;
    string name = "cpu"+coreNumber;
    vector<string> result;
    ifstream stream = Util::getStream(Path::basePath()+Path::statPath());
    while(getline(stream, line)) {
        if (line.compare(0,name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> result(beg,end);
            result.erase(result.begin());
            break;
        }
    };
    return result;
}



float ProcessParser::getSysActiveCpuTime(vector<string> values){
    // convert to float an sum the values of the cpu
    return( stof(values[S_USER])+
            stof(values[S_NICE])+
            stof(values[S_SYSTEM])+
            stof(values[S_IRQ])+
            stof(values[S_SOFTIRQ])+
            stof(values[S_STEAL])+
            stof(values[S_GUEST])+
            stof(values[S_GUEST_NICE]));
}

float ProcessParser::getSysIdleCpuTime(vector<string> values){
    // convert to float an sum the values of the cpu
    return( stof(values[S_IDLE])+
            stof(values[S_IOWAIT]));
}



string ProcessParser::PrintCpuStats(vector<string> values1, vector<string>values2){

    float activeTime = ProcessParser::getSysActiveCpuTime(values2)-ProcessParser::getSysActiveCpuTime(values1);
    float idleTime = ProcessParser::getSysIdleCpuTime(values2)-ProcessParser::getSysIdleCpuTime(values1);
    float totalTime = activeTime + idleTime;
    float result =100*(activeTime/totalTime);
    string s = to_string(result);

    return s;

}


float ProcessParser::getSysRamPercent(){

    vector<string> values;
    float result;
    float totalMem;
    float freeMem;
    float buffers;

    string Path = Path::basePath()+Path::memInfoPath();
    values = ProcessParser::searcher(Path, "MemAvailable:");
    totalMem = stof(values[1]);
    values = ProcessParser::searcher(Path, "MemFree:");
    freeMem = stof(values[1]);
    values = ProcessParser::searcher(Path, "Buffers:");
    buffers = stof(values[1]);

    result = 100.0*(1-(freeMem/(totalMem-buffers)));

    return result;
}



string ProcessParser::getSysKernelVersion(){

    vector<string> values;
    string result;

    string Path = Path::basePath()+Path::versionPath();
    values = ProcessParser::searcher(Path, "Linux version ");
    result = values[1];

    return result;
}



string ProcessParser::getOSName(){

    vector<string> values;
    string result;

    string Path = "/etc/os-release";
    values = ProcessParser::searcher(Path, "PRETTY NAME=");
    result = values[1];

    return result;
}



int ProcessParser::getTotalThreads(){

    vector<string> values;
    vector<string> pidList;
    string Path;
    int result = 0;

    pidList = ProcessParser::getPidList();
    for (string pid : pidList){
        Path = Path::basePath()+pid+Path::statusPath();
        values = ProcessParser::searcher(Path, "Threads:");
        result = result + stoi(values[1]);
    }

    return result;
}



int ProcessParser::getTotalNumberOfProcesses(){

    vector<string> values;
    int result;

    string Path = Path::basePath()+Path::statPath();
    values = ProcessParser::searcher(Path, "processes");
    result = stoi(values[1]);

    return result;
}



int ProcessParser::getNumberOfRunningProcesses(){

    vector<string> values;
    int result;

    string Path = Path::basePath()+Path::statPath();
    values = ProcessParser::searcher(Path, "procs_running");
    result = stoi(values[1]);

    return result;
}

 bool ProcessParser::isPidExisting(string pid){

    bool result = false;
     vector<string> pid_List = ProcessParser::getPidList();
     for (string pidE : pid_List){
         if (pid == pidE)
         result = true;
     }
    return result;
 }