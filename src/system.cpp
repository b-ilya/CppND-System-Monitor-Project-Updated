#include "system.h"

#include <unistd.h>
#include <iostream>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <algorithm>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System(): kernelName(LinuxParser::Kernel()), osName(LinuxParser::OperatingSystem()) { Update(); }

void System::Update() {
  uptime = LinuxParser::UpTime();
  memLoad = LinuxParser::MemoryUtilization();
  totalProc = LinuxParser::TotalProcesses();
  runningProc = LinuxParser::RunningProcesses();

  long jiffsDelta = cpu.Update();

  auto pids = LinuxParser::Pids();
  vector<Process> newProcesses = {};

  for (int i : pids) {
    bool found = false;
    for (auto process : processes) {
      if (process.Pid() == i) {
        process.Update(jiffsDelta);
        newProcesses.push_back(process);
        found = true;
        break;
      }
    }
    if (!found) {
      //    newProcesses.push_back(new Process(i));
      Process process(i);
      process.Update(jiffsDelta);
      newProcesses.push_back(process);
    }
  }
  std::sort(newProcesses.begin(), newProcesses.end());
  processes = newProcesses;
}

// Return the system's CPU
const Processor& System::Cpu() const { return cpu; }

// Return a container composed of the system's processes
const vector<Process>& System::Processes() const { return processes; }

// Return the system's kernel identifier (string)
std::string System::Kernel() const { return kernelName; }

// Returns the system's memory utilization
float System::MemoryUtilization() const { return memLoad; }

// Returns the operating system name
std::string System::OperatingSystem() const { return osName; }

// Returns the number of processes actively running on the system
int System::RunningProcesses() const { return runningProc; }

// Returna the total number of processes on the system
int System::TotalProcesses() const { return totalProc; }

// Returns the number of seconds since the system started running
long int System::UpTime() const { return uptime; }