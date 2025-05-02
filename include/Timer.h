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

  std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimepoint;
  std::string label;

public:
  inline Timer(const std::string &label)
      : m_StartTimepoint(std::chrono::high_resolution_clock::now()),
        label(label) {}

  inline ~Timer() { Stop(); }

  inline static void printData() {
    const size_t borderSize = maxLabelSize + EXPECTED_MAX_DIGITS + 6;
    const std::string border(borderSize, '-');

    std::cout << border << '\n';
    for (const auto &pair : data) {
      std::cout << std::left << std::setw(maxLabelSize) << pair.first << ": "
                << std::right << std::fixed
                << std::setw(EXPECTED_MAX_DIGITS + 1) << std::setprecision(3)
                << pair.second << " ms\n";
    }
    std::cout << border << std::endl;
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
