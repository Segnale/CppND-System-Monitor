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

#include "util.h"


using namespace std;

class ProcessParser{
private:
    std::ifstream stream;
    public:
    static string getCmd(string pid);
    static vector<string> getPidList();
    static std::string getVmSize(string pid);
    static std::string getCpuPercent(string pid);
    static long int getSysUpTime();
    static std::string getProcUpTime(string pid);
    static string getProcUser(string pid);
    static vector<string> getSysCpuPercent(string coreNumber = "");
    static float getSysRamPercent();
    static string getSysKernelVersion();
    static int getNumberOfCores();
    static int getTotalThreads();
    static int getTotalNumberOfProcesses();
    static int getNumberOfRunningProcesses();
    static string getOSName();
    static std::string PrintCpuStats(std::vector<std::string> values1, std::vector<std::string>values2);
    static bool isPidExisting(string pid);
};

// TODO: Define all of the above functions below:


string ProcessParser::getVmSize(string pid) {
    string line;
    string name = "VmData";
    string value;
    float result;
    ifstream stream = Util::getStream((Path::basePath()+pid+Path::statusPath()));
    while(getline(stream, line)) {
        if (line.compare(0,name.size(), name) == 0) {
            istringstream buf(line);
            istream_iterator<string> beg(buf), end;
            vector<string> values(beg,end);
            result = stof(values[1])/float(1024);
            break;
        }
    };
    return to_string(result);
}



string ProcessParser::getCpuPercent(string pid){

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