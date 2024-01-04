#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <filesystem>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include "format.h"

using std::getline;
using std::ifstream;
using std::istringstream;
using std::replace;
using std::string;
using std::vector;

template <typename T>
T GetValueFromFileByTag(string filename, string value_tag, T default_value) {
  ifstream stream(filename);
  if (!stream.is_open()) {
    return default_value;
  }

  string line{string()};
  istringstream line_stream(line);
  string tag{string()}, value_string{string()};
  while (getline(stream, line)) {
    istringstream linestream(line);
    linestream >> tag >> value_string;

    if (value_tag.compare(tag) == 0 && std::is_same<T, int>::value) {
      return std::stoi(value_string);
    }
  }

  return default_value;
}

std::vector<string> GetProcessStatValues(int pid) {
  ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                  LinuxParser::kStatFilename);
  if (!stream.is_open()) {
    return {};
  }

  std::vector<string> tokens{};
  string current_token{string()};
  while (stream >> current_token) {
    tokens.emplace_back(current_token);
  }

  return tokens;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (getline(filestream, line)) {
      replace(line.begin(), line.end(), ' ', '_');
      replace(line.begin(), line.end(), '=', ' ');
      replace(line.begin(), line.end(), '"', ' ');
      istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    getline(stream, line);
    istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  std::filesystem::path pids_path{kProcDirectory};

  if (!std::filesystem::exists(pids_path) ||
      !std::filesystem::is_directory(pids_path)) {
    return pids;
  }

  for (auto& file : std::filesystem::directory_iterator(pids_path)) {
    // Is every character of the name a digit?
    string filename(file.path().filename().string());
    if (std::all_of(filename.begin(), filename.end(), isdigit)) {
      pids.emplace_back(stoi(filename));
    }
  }

  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  ifstream stream(kProcDirectory + kMeminfoFilename);
  if (!stream.is_open()) {
    return 0.0;
  }

  string line{string()};
  string tag{string()}, value_string{string()};
  float mem_total{0.0}, mem_free{0.0}, buffers{0.0};
  while (getline(stream, line)) {
    replace(line.begin(), line.end(), ':', ' ');
    istringstream linestream(line);
    linestream >> tag >> value_string;
    if (tag.compare(kMemTotalTag) == 0) {
      mem_total = std::stof(value_string);
    } else if (tag.compare(kMemFreeTag) == 0) {
      mem_free = std::stof(value_string);
    } else if (tag.compare(kBuffersTag) == 0) {
      buffers = std::stof(value_string);
    }
  }

  return 1 - (mem_free / (mem_total - buffers));
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  ifstream stream(kProcDirectory + kUptimeFilename);
  if (!stream.is_open()) {
    return 0;
  }

  string line{string()};
  string system_uptime_string{string()};
  long system_uptime{0};
  if (getline(stream, line)) {
    istringstream linestream(line);
    linestream >> system_uptime_string;
    system_uptime = std::stol(system_uptime_string);
    return (long)(system_uptime);
  }

  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long up_time = UpTime();
  return up_time / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  std::vector<string> tokens{GetProcessStatValues(pid)};

  if (tokens.size() < 16) {
    return 0;
  }

  long utime = std::stol(tokens[13]);
  long stime = std::stol(tokens[14]);
  long cutime = std::stol(tokens[15]);
  long cstime = std::stol(tokens[16]);
  long active_jeffies = utime + stime + cutime + cstime;  // active jiffies

  return active_jeffies / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  auto timestamps = CpuUtilization();
  long total_time = std::stol(timestamps[CPUStates::kUser_]) +
                    std::stol(timestamps[CPUStates::kNice_]) +
                    std::stol(timestamps[CPUStates::kSystem_]) +
                    std::stol(timestamps[CPUStates::kIdle_]) +
                    std::stol(timestamps[CPUStates::kIOwait_]) +
                    std::stol(timestamps[CPUStates::kIRQ_]) +
                    std::stol(timestamps[CPUStates::kSoftIRQ_]) +
                    std::stol(timestamps[CPUStates::kSteal_]) +
                    std::stol(timestamps[CPUStates::kGuest_]) +
                    std::stol(timestamps[CPUStates::kGuestNice_]);

  return total_time;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto timestamps = CpuUtilization();
  long idle_time = std::stol(timestamps[CPUStates::kIdle_]) +
                   std::stol(timestamps[CPUStates::kSteal_]);

  return idle_time;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  ifstream stream(kProcDirectory + kStatFilename);
  if (!stream.is_open()) {
    return {};
  }

  string line{string()};
  getline(stream, line);

  vector<string> cpu_utilization{};
  istringstream line_stream(line);
  string tag, value;
  line_stream >> tag;
  if (tag.compare(kProcessCpuTag) != 0) {
    return cpu_utilization;
  }
  while (line_stream >> value) {
    if (std::all_of(value.begin(), value.end(), isdigit)) {
      cpu_utilization.emplace_back(value);
    }
  }

  return cpu_utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return GetValueFromFileByTag<int>(kProcDirectory + kStatFilename,
                                    kProcessesTag, 0);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return GetValueFromFileByTag<int>(kProcDirectory + kStatFilename,
                                    kRunningProcessesTag, 0);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string command{string()};
  ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  stream >> command;

  return command;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (!stream.is_open()) {
    return "0";
  }

  string line{string()}, tag{string()}, value{string()}, unit{string()};
  while (getline(stream, line)) {
    replace(line.begin(), line.end(), ':', ' ');
    istringstream linestream(line);
    linestream >> tag;
    if (tag.compare(kVmSizeTag) == 0) {
      linestream >> value >> unit;
      if (unit.compare("kB") == 0) {
        return Format::ToMB(std::stol(value));
      }
    }
  }

  return "0";
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (!stream.is_open()) {
    return string();
  }

  string line{string()}, tag{string()}, value_string{string()};
  while (getline(stream, line)) {
    replace(line.begin(), line.end(), ':', ' ');
    istringstream linestream(line);
    linestream >> tag;
    if (tag.compare(kProcessUidTag) == 0) {
      linestream >> value_string;
      return value_string;
    }
  }

  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  const string uid = Uid(pid);
  ifstream stream(kPasswordPath);
  if (!stream.is_open()) {
    return string();
  }

  string line{string()}, user{string()}, encrypted_pass{string()},
      user_uid{string()};
  while (getline(stream, line)) {
    replace(line.begin(), line.end(), ' ', '_');
    replace(line.begin(), line.end(), ':', ' ');
    istringstream linestream(line);
    linestream >> user >> encrypted_pass >> user_uid;
    if (uid.compare(user_uid) == 0) {
      return user;
    }
  }

  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  auto tokens{GetProcessStatValues(pid)};

  if (tokens.size() < 22) {
    return 0;
  }
  auto starttime = std::stol(tokens[21]);  // starttime
  return starttime / sysconf(_SC_CLK_TCK);
}
