#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  System();
  void Update();

  const Processor& Cpu() const;                   // TODO: See src/system.cpp
  const std::vector<Process>& Processes() const;  // TODO: See src/system.cpp
  float MemoryUtilization() const;
  long UpTime() const;
  int TotalProcesses() const;
  int RunningProcesses() const;
  std::string Kernel() const;
  std::string OperatingSystem() const;

  // TODO: Define any necessary private members
 private:
  Processor cpu{};
  std::vector<Process> processes{};
  long uptime;
  float memLoad;
  int totalProc;
  int runningProc;
  std::string kernelName;
  std::string osName;
};

#endif