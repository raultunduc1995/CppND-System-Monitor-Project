#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using std::string;
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);                        // TODO: See src/process.cpp
  int Pid();                               // TODO: See src/process.cpp
  string User();                           // TODO: See src/process.cpp
  string Command();                        // TODO: See src/process.cpp
  float CpuUtilization() const;            // TODO: See src/process.cpp
  string Ram();                            // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(const Process &a) const;  // TODO: See src/process.cpp
  bool operator>(const Process &other) const;

  // TODO: Declare any necessary private members
 private:
  int pid_{0};
  string command_{string()};
  string uid_{string()};
  string user_{string()};
  float cpu_utilization_{0.0};
  mutable float previous_active_jiffies{0.0};
  mutable float previous_system_jiffies{0.0};

  float ComputeCpuUtilization() const;
};

#endif