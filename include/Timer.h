#pragma once
#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>

class Timer {
private:
  const inline static size_t EXPECTED_MAX_DIGITS = 8;
  inline static size_t maxLabelSize = 0;
  inline static std::unordered_map<std::string, double> data = {};
  inline static std::chrono::time_point<std::chrono::high_resolution_clock>
      m_GlobalStart;

  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
  std::string label;

public:
  inline Timer(const std::string &label)
      : m_StartTimepoint(std::chrono::high_resolution_clock::now()),
        label(label) {}

  inline ~Timer() { Stop(); }

  inline static void printData() {
    const auto endTimepoint = std::chrono::high_resolution_clock::now();
    const auto globalDuration =
        std::chrono::duration<double, std::milli>(endTimepoint - m_GlobalStart)
            .count();

    const size_t borderSize = maxLabelSize + EXPECTED_MAX_DIGITS + 14;
    const std::string border(borderSize, '-');

    std::cout << border << '\n';
    for (const auto &pair : data) {

      std::cout << std::left << std::setw(maxLabelSize) << pair.first << ": "
                << std::right << std::fixed
                << std::setw(EXPECTED_MAX_DIGITS + 1) << std::setprecision(3)
                << pair.second << " ms | " << std::setprecision(1)
                << (pair.second / globalDuration) * 100 << "%\n";
    }
    std::cout << border << std::endl;
  }

  inline static void global() {
    m_GlobalStart = std::chrono::high_resolution_clock::now();
  }

private:
  inline void Stop() {
    const auto endTimepoint = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration<double, std::milli>(
                              endTimepoint - m_StartTimepoint)
                              .count();

    if (label.size() > maxLabelSize) {
      maxLabelSize = label.size();
    }

    data[label] += duration;
  }
};
