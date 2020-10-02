#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H


#include <iostream>
#include <fstream>
#include <regex>
#include <string>


namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

void skip(std::istream&, int);

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<long> CpuUtilization();

// Processes
struct UserName {
  UserName(int uid, std::string name);
  int uid;
  std::string name;
};

std::string Command(int pid);
int Ram(int pid);
int Uid(int pid);
std::string User(int pid);
long ActiveJiffies(int pid);
}; 

#endif //SYSTEM_PARSER_H