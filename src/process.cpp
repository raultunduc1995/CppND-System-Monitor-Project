#include "process.h"

#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>

#include "format.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

float Process::ComputeCpuUtilization() const {
  auto current_process_active_jiffies =
      static_cast<float>(LinuxParser::ActiveJiffies(pid_));
  auto current_process_starttime =
      static_cast<float>(LinuxParser::UpTime(pid_));
  auto current_system_starttime = static_cast<float>(LinuxParser::UpTime());

  auto process_active_jiffies =
      current_process_active_jiffies - previous_active_jiffies;
  auto system_jiffies = current_system_starttime - previous_system_jiffies;

  previous_active_jiffies = current_process_active_jiffies;
  previous_system_jiffies = current_system_starttime;

  return process_active_jiffies / (system_jiffies - current_process_starttime);
}

// TODO: Construct a Process object
Process::Process(int pid) : pid_(pid) {
  user_ = LinuxParser::User(pid_);
  command_ = LinuxParser::Command(pid_);
  cpu_utilization_ = ComputeCpuUtilization();
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_utilization_; }

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(const Process& a) const {
  return CpuUtilization() < a.CpuUtilization();
}

bool Process::operator>(const Process& other) const {
  return CpuUtilization() > other.CpuUtilization();
}