//
// Created by wuyua on 2/1/2022.
//

#ifndef MINICLI_CHECKER_H_
#define MINICLI_CHECKER_H_

#include <memory>

namespace minicli {
template <typename T>
class Checker {
 public:
  using CheckerPtr = std::unique_ptr<Checker<T>>;

  Checker() = default;

 public:
  virtual bool Check(const T& value) { return true; };
};

/// Create the unique pointer to the checker
/// \tparam T
/// \tparam Args
/// \param args
/// \return
template <typename T, typename... Args>
typename T::CheckerPtr make_checker(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
class Range : public Checker<T> {
 public:
  Range(T min, T max) : min(min), max(max) {}

 private:
  bool Check(const T& value) override { return (value <= max && value >= min); }

 private:
  T min;
  T max;
};

template <typename T>
class NotEqual : public Checker<T> {
 public:
  explicit NotEqual(T val) : val(val) {}

 private:
  bool Check(const T& value) override { return (value != val); }

 private:
  T val;
};
}  // namespace minicli
#endif  // MINICLI_CHECKER_H_
