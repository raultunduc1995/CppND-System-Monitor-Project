#ifndef SYSTEM_H
#define SYSTEM_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  System();
  ~System() = default;
  System(const System &other) = delete;
  System &operator=(const System &other) = delete;

  Processor &Cpu();                   // TODO: See src/system.cpp
  std::vector<Process> &Processes();  // TODO: See src/system.cpp
  float MemoryUtilization();          // TODO: See src/system.cpp
  long UpTime();                      // TODO: See src/system.cpp
  int TotalProcesses();               // TODO: See src/system.cpp
  int RunningProcesses();             // TODO: See src/system.cpp
  std::string Kernel();               // TODO: See src/system.cpp
  std::string OperatingSystem();      // TODO: See src/system.cpp

  // TODO: Define any necessary private members
 private:
  Processor cpu_;
  std::vector<Process> processes_;
  std::string kernel_;
  std::string os_;
};

#endif