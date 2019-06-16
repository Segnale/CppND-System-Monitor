#pragma once

#include <string>
#include "ProcessParser.h"


using namespace std;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
private:
    string pid;
    string user;
    string cmd;
    string cpu;
    string mem;
    string upTime;

public:
    Process(string pid){
        this->pid = pid;
        this->user = ProcessParser::getProcUser(pid);
        this->mem = ProcessParser::getVmSize(pid);
        this->cmd = ProcessParser::getCmd(pid);
        this->upTime = ProcessParser::getProcUpTime(pid);
        this->cpu = ProcessParser::getCpuPercent(pid);
    }
    void setPid(int pid);
    string getPid()const;
    string getUser()const;
    string getCmd()const;
    float getCpu()const;
    int getMem()const;
    string getUpTime()const;
    string getProcess();
};

// Setters and Getters Implementation
void Process::setPid(int pid){
    this->pid = pid;
}
string Process::getPid()const {
    return this->pid;
}

string Process::getUser()const {
    return this->user;
}

string Process::getCmd()const {
    return this->cmd;
}

float Process::getCpu()const {
    return stof(this->cpu);
}

int Process::getMem()const {
    return stoi(this->mem);
}

string Process::getUpTime()const {
    return this->upTime;
}

string Process::getProcess(){
    if(!ProcessParser::isPidExisting(this->pid))
        return "";
    this->user = ProcessParser::getProcUser(this->pid);
    this->mem = ProcessParser::getVmSize(this->pid);
    this->upTime = ProcessParser::getProcUpTime(this->pid);
    this->cpu = ProcessParser::getCpuPercent(this->pid);
    string for_empties = this->pid;
    string pid_empties(7-for_empties.size(),' ');
    for_empties = this->user;
    string user_empties(7-for_empties.size(),' ');


    return (this->pid + pid_empties  
            + this->user.substr(0,5) + user_empties 
            + this->cpu.substr(0,5) + "     " 
            + this->mem.substr(0,5) + "     " 
            + this->upTime.substr(0,5) + "    "
            + this->cmd.substr(0,30) + "...");
}
