//
// Created by wuyua on 1/31/2022.
//

#ifndef MINICLI_COMMAND_H_
#define MINICLI_COMMAND_H_
#include <functional>

#include "option.hpp"
#include "util.hpp"

namespace minicli {

/// Callback type of a command
using CallbackType = std::function<void(void)>;

class Command {
 public:
  using CommandPtr = std::unique_ptr<Command>;
  using BaseOptionPtr = std::unique_ptr<BaseOption>;

 public:
  /// Register a new command in this command. If already exists, return
  /// nullptr.
  /// \param name
  /// \return
  Command* AddCommand(const std::string& name);

  /// Retrieve an command using the name
  /// \param name
  /// \return nullptr if failed.
  Command* GetCommand(const std::string& name);

  /// Set the callback of the command
  /// \param callback
  /// \return this
  Command* Callback(const CallbackType& callback);

  /// Get the callback of the command
  /// \return
  const CallbackType& GetCallback();

  /// Add an option to the command (invoked by "-{name} {value}"
  /// \tparam T
  /// \param match
  /// \param variable
  /// \return
  template <typename T>
  Option<T>* AddOption(const std::string& match, T& variable);

  /// Retrieve an command with the match name
  /// \param match
  /// \return nullptr if not exists
  BaseOption* GetOption(const std::string& match);

  /// Register a new command using the custom callback
  /// \tparam T
  /// \param match
  /// \param callback
  /// \return
  template <typename T>
  Option<T>* AddOptionCallback(
      const std::string& match,
      const OptionCallback<T>& callback);

 protected:
  /// Stores name -> subcommand pointers
  std::unordered_map<std::string, CommandPtr> sub_cmd_map_;

  /// Stores match -> Option pointers
  std::unordered_map<std::string, BaseOptionPtr> option_map_;

  /// Command name, currently is not useful
  std::string name_;

  /// Command callback with default to doing nothing
  CallbackType callback_{[] {}};

 private:
  /// Unexposed constructor. Use AddCommand to construct the object
  /// \param name
  explicit Command(std::string name);

 protected:
  /// For use in Application class where name is not important.
  Command() = default;
};  // class Command

/*  Implementations  */

template <typename T>
Option<T>* Command::AddOption(const std::string& match, T& variable) {
  auto cb = ([&](const T& v) -> bool {
    variable = v;
    return true;
  });
  return AddOptionCallback<T>(match, cb);
}

template <typename T>
Option<T>* Command::AddOptionCallback(
    const std::string& match,
    const OptionCallback<T>& callback) {
  if (option_map_.find(match) != option_map_.end()) {
    // option already exists
    MINICLI_THROW_OR_RETURN(std::runtime_error("Duplicated options"), nullptr);
  }

  auto& ptr = option_map_[match];

  ptr = std::make_unique<Option<T>>(callback);

  return dynamic_cast<Option<T>*>(ptr.get());
}
}  // namespace minicli
#endif  // MINICLI_COMMAND_H_
