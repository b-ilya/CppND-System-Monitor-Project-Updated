#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  Processor();
  long Update();
  float Utilization() const;

 private:
  float utilization{0};
  std::vector<long> lastTotalJiffies{};
};

#endif
