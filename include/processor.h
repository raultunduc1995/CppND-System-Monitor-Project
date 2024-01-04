#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  Processor() = default;
  ~Processor() = default;
  Processor(const Processor &other) = delete;
  Processor &operator=(const Processor &other) = delete;

  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long previous_total_time{0};
  long previous_idle_time{0};
};

#endif