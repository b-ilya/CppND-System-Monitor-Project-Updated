#include "processor.h"

#include <vector>

#include "linux_parser.h"

using LinuxParser::CPUStates;

Processor::Processor() { Update(); }

long Processor::Update() {
  auto currentJiffies = LinuxParser::CpuUtilization();
  auto deltas = currentJiffies;
  if (!lastTotalJiffies.empty()) {
    for (unsigned i = 0; i < lastTotalJiffies.size(); i++) {
      deltas[i] = deltas[i] - lastTotalJiffies[i];
    }
  }

  long idle = deltas[CPUStates::kIdle_] + deltas[CPUStates::kIOwait_];

  long busy = deltas[CPUStates::kUser_] + deltas[CPUStates::kNice_] +
              deltas[CPUStates::kSystem_] + deltas[CPUStates::kIRQ_] +
              deltas[CPUStates::kSoftIRQ_] + deltas[CPUStates::kSteal_];

  utilization = 1.0 * busy / (busy + idle);

  lastTotalJiffies = currentJiffies;
  return busy + idle;
}

float Processor::Utilization() const { return utilization; }