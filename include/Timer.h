// https://www.youtube.com/watch?v=YG4jexlSAjc&t=711s
#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <iostream>
#include <string>


class Timer {
private:
  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint =
      std::chrono::high_resolution_clock::now();
  std::string msg;

public:
  Timer(const std::string &msg) : msg(msg) {};
  ~Timer() { Stop(); };

  void Stop() {
    auto endTimepoint = std::chrono::high_resolution_clock::now();

    auto start = std::chrono::time_point_cast<std::chrono::microseconds>(
                     m_StartTimepoint)
                     .time_since_epoch()
                     .count();
    auto end =
        std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint)
            .time_since_epoch()
            .count();

    auto duration = end - start;
    double ms = duration * 0.001;
    std::cout << msg << " duration (ms): " << ms << std::endl;
  }
};

#endif
