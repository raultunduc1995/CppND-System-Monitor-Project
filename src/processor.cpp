#include "processor.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using LinuxParser::CPUStates;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long current_total_time = LinuxParser::ActiveJiffies();
  long current_idle_time = LinuxParser::IdleJiffies();
  long total_time = current_total_time - previous_total_time;
  long idle_time = current_idle_time - previous_idle_time;
  previous_total_time = current_total_time;
  previous_idle_time = current_idle_time;

  return (total_time - idle_time) / (float)total_time;
}