#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>
#include <unordered_map>
#include <vector>

#include <unistd.h> // only effective with Linux, "windows.h" works with Windows

// #include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
#include "sources/Utils/testMy.h"

#define PROFILE_CODE

#define CurrentTimeInProfiler std::chrono::high_resolution_clock::now()
// #define BeginTimerAppInProfiler BeginTimer(__FUNCTION__);
// #define EndTimerAppInProfiler EndTimer(__FUNCTION__);
extern std::mutex mtx_profiler;
struct ProfilerData {
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimerType;
  TimerType begin;
  TimerType end;
  double accum;
  ProfilerData()
      : begin(std::chrono::high_resolution_clock::now()), end(std::chrono::high_resolution_clock::now()),
        accum(0) {}
  void UpdateAccum() {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    accum += double(duration.count()) / 1e6;
  }
};
extern std::unordered_map<std::string, ProfilerData> profilerMap;
void BeginTimer(std::string funcName);

void EndTimer(std::string funcName, bool print = false);

struct TimerDataProfiler {
  std::string name;
  double accum;
};
bool compareProfiler(TimerDataProfiler a, TimerDataProfiler b);
void PrintTimer();

class TimerFunc {
public:
  TimerFunc() : begin(std::chrono::high_resolution_clock::now()) {}
  ~TimerFunc() {
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - begin);
    std::cout << "Accumulated time during the execution is: " << double(duration.count()) / 1e6
              << " seconds!\n";
  }

private:
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimerType;
  TimerType begin;
};