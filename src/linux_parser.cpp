#include "linux_parser.h"

#if defined(__cpp_lib_filesystem)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <dirent.h>
#endif

#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

void LinuxParser::skip(std::istream& istream, int count) {
  string skip;
  for (int i = 0; i < count; i++) istream >> skip;
}

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

vector<int> LinuxParser::Pids() {
  vector<int> pids;
#if defined(__cpp_lib_filesystem)
  fs::path procPath(kProcDirectory);
  if (fs::exists(procPath) && fs::is_directory(procPath)) {
    for (const auto& entry : fs::directory_iterator(procPath)) {
      if (fs::is_directory(entry.status())) {
        auto dirName = entry.path().filename().string();
        if (!dirName.empty() &&
            std::all_of(dirName.begin(), dirName.end(), isdigit)) {
          pids.push_back(std::stoi(dirName));
        }
      }
    }
  }
#else
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);

#endif
  return pids;
}

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

long LinuxParser::RamkB(int pid) {
  string line;
  string key;
  long value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> value;
        return value;
      }
    }
  }
  return 0;
}

int LinuxParser::Uid(int pid) {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
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

std::unordered_map<int, string> userNames{};

string LinuxParser::User(int pid) {
  int uid = Uid(pid);
  auto userName = userNames.find(pid);
  if (userName != userNames.end()) { return userName->second; }

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
        userNames[id] = name;
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
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    skip(linestream, 13);
    linestream >> utime >> stime >> cutime >> cstime;
  }
  return utime + stime + cutime + cstime;
}
