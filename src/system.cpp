#include "system.h"

#include <unistd.h>

#include <cstddef>
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

// Return the system's CPU
const Processor& System::Cpu() const { return cpu_; }

// Return a container composed of the system's processes
const vector<Process>& System::Processes() const { return processes_; }

// Return the system's kernel identifier (string)
std::string System::Kernel() const { return LinuxParser::Kernel(); }

// Returns the system's memory utilization
float System::MemoryUtilization() const {
  return LinuxParser::MemoryUtilization();
}

// Returns the operating system name
std::string System::OperatingSystem() const {
  return LinuxParser::OperatingSystem();
}

// Returns the number of processes actively running on the system
int System::RunningProcesses() const { return LinuxParser::RunningProcesses(); }

// Returna the total number of processes on the system
int System::TotalProcesses() const { return LinuxParser::TotalProcesses(); }

// Returns the number of seconds since the system started running
long int System::UpTime() const { return LinuxParser::UpTime(); }