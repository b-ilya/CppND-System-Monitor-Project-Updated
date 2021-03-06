#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)
    : pid(pid),
      userName(LinuxParser::User(pid)),
      command(LinuxParser::Command(pid)) {
  Update(0);
}

void Process::Update(long jiffsDelta) {
  updateCpu(jiffsDelta);
  updateMemory();
}

void Process::updateMemory() {
  // convert number of kiloytes to string with number of megabytes
  std::stringstream stream;
  stream << std::fixed << std::setprecision(1) << std::setw(7)
         << (LinuxParser::RamkB(pid) * 0.001);
  memLoad = stream.str();
}

void Process::updateCpu(long jiffsDelta) {
  long curJiffsTotal = LinuxParser::ActiveJiffies(pid);
  cpuLoad = 1.0 * (curJiffsTotal - prevJiffsTotal) / jiffsDelta;
  prevJiffsTotal = curJiffsTotal;
  cpuTime = curJiffsTotal / sysconf(_SC_CLK_TCK);
}

int Process::Pid() const { return pid; }

float Process::CpuUtilization() const { return cpuLoad; }

string Process::Command() const { return command; }

string Process::Ram() const { return memLoad; }

string Process::User() const { return userName; }

long Process::CpuTime() const { return cpuTime; }

bool Process::operator<(Process const& a) const { return cpuLoad > a.cpuLoad; }