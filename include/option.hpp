//
// Created by wuyua on 1/31/2022.
//

#ifndef MINICLI_OPTION_HPP_
#define MINICLI_OPTION_HPP_

#include <sstream>

#include "checker.hpp"

namespace minicli {
class Command;

template <typename T>
using OptionCallback = std::function<bool(const T&)>;

/// Base option class handles string token as the option argument by dispatching
/// the parse to the templated derived classes
class BaseOption {
 protected:
  friend Command;

 public:
  virtual bool Check(const std::string& content) = 0;
  virtual void Commit() = 0;
};

template <typename T>
class Option : public BaseOption {
 public:
  explicit Option(const OptionCallback<T>& cb);

  /// Update the bound variable once all check has passed.
  void Commit() override;

  ///
  /// \param checker only accept rvalue (lvalue should be moved)
  /// \return
  Option<T>* AddChecker(typename Checker<T>::CheckerPtr&& checker);

  /// Go through the checkers to validate the argument
  /// \param content
  /// \return true for passed check and vice versa.
  bool Check(const std::string& content) override;

 protected:
  /// Stores checker pointers. For calling the virtual methods in the derived
  /// checkers, it must be stored as a pointer
  std::vector<typename Checker<T>::CheckerPtr> checkers_;

  /// Value stored in Check method, which will be assigned during Commit.
  T pre_commit_value_;

  /// Store the assignment callback. The std::function manages the internal
  /// callable by itself so we do not need to wrap it in a unique_ptr
  OptionCallback<T> cb_;
};

/*  Implementations */

template <typename T>
Option<T>* Option<T>::AddChecker(typename Checker<T>::CheckerPtr&& checker) {
  // TODO: why && should be converted to an rvalue?
  checkers_.push_back(std::move(checker));
  return this;
}

template <typename T>
Option<T>::Option(const OptionCallback<T>& cb) : cb_(cb) {}

template <typename T>
bool Option<T>::Check(const std::string& content) {
  // Parse the argument using stringstream and check if the conversion is
  // successful
  std::stringstream ss(content);
  T value;
  ss >> value;
  if (ss.fail()) {
    return false;
  }

  // Run through all checkers and interrupt if any fails
  for (auto& checker : checkers_) {
    if (!checker->Check(value)) {
      return false;
    }
  }

  // Record the parsed value for commit method.
  pre_commit_value_ = value;
  return true;
}

template <typename T>
void Option<T>::Commit() {
  // Call the assignment callback
  cb_(pre_commit_value_);
}

}  // namespace minicli
#endif  // MINICLI_OPTION_HPP_
