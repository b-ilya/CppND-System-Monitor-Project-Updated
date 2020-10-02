#include "linux_parser.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;
namespace fs = std::filesystem;

void LinuxParser::skip(std::istream& istream, int count) {
  string skip;
  for (int i = 0; i<count; i++, istream >> skip);
}

// Reads and returns OS name
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Reads and returns kernel's identificator
string LinuxParser::Kernel() {
  string skip, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> skip >> skip >> kernel;
  }
  return kernel;
}

// Returns vector of process IDs
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  fs::path procPath(kProcDirectory);
  if (fs::exists(procPath) && fs::is_directory(procPath)) {
    for (const auto& entry : fs::directory_iterator(procPath)) {
      if (fs::is_directory(entry.status())) {
        auto dirName = entry.path().filename().generic_string();
        if (std::all_of(dirName.begin(), dirName.end(), isdigit)) {
          pids.push_back(std::stoi(dirName));
        }
      }
    }
  }
  return pids;
}

// Reads and returns the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  int total;
  int free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "MemTotal:") {
        linestream >> total;
      } else if (key == "MemFree:") {
        linestream >> free;
      }
    }
  }
  if (total > 0 && free > 0) {
    return 1.0 * (total - free) / total;
  }
  return 0;
}

// Reads and returns the system uptime
long LinuxParser::UpTime() {
  string uptime{"0"};
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return stol(uptime);
}


vector<long> LinuxParser::CpuUtilization() {
  vector<long> jiffies = {};
  std::string label;
  long value;
  std::string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    // skip cpu label before numbers
    linestream >> label;
    for (int i = 0; i < 10; i++) {
      linestream >> value;
      jiffies.push_back(value);
    }
  }
  return jiffies;
}

// Reads and returns the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "processes") {
        linestream >> value;
        return value;
      }
    }
  }
  return 0;
}

// Reads and returns the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "procs_running") {
        linestream >> value;
        return value;
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
  }
  return line;
}

int LinuxParser::Ram(int pid) {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> value;
        return value * 1000;
      }
    }
  }
  return 0;
}

int LinuxParser::Uid(int pid) { 
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> value;
        return value;
      }
    }
  }
  return 0;
}

LinuxParser::UserName::UserName(int uid, string name) : uid(uid), name(name) {}

std::vector<LinuxParser::UserName> userNames{};

string LinuxParser::User(int pid) { 
  int uid = Uid(pid);
  for (auto& userRecord : userNames) {
    if (userRecord.uid == uid) {
      return userRecord.name;
    }
  }

  std::ifstream filestream(kPasswordPath);
  string line;
  string name;
  string skip;
  int id;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name;
      linestream >> skip;
      linestream >> id;
      if (id == uid) {
        userNames.push_back(UserName(id, name));
        return name;
      }
    }
  }
  return string();
}

long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  long utime;
  long stime;
  long cutime;
  long cstime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    skip(linestream, 13);
    linestream >> utime >> stime >> cutime >> cstime;
  }
  return utime + stime + cutime + cstime;
}
