#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System()
    : kernelName(LinuxParser::Kernel()),
      osName(LinuxParser::OperatingSystem()) {
  Update();
}

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

const Processor& System::Cpu() const { return cpu; }

const vector<Process>& System::Processes() const { return processes; }

std::string System::Kernel() const { return kernelName; }

float System::MemoryUtilization() const { return memLoad; }

std::string System::OperatingSystem() const { return osName; }

int System::RunningProcesses() const { return runningProc; }

int System::TotalProcesses() const { return totalProc; }

long int System::UpTime() const { return uptime; }