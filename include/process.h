#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int);
  void Update(long);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long CpuTime() const;
  bool operator<(Process const& a) const;

 private:
  // fixed state
  int pid;
  std::string userName;
  std::string command;
  // variable state
  long prevJiffsTotal;
  long cpuTime;
  std::string memLoad;
  float cpuLoad;
  // private methods
  void updateCpu(long);
  void updateMemory();
};

#endif