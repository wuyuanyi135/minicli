//
// Created by wuyua on 1/31/2022.
//
#include "command.hpp"

#include <exception>

namespace minicli {
Command::Command(std::string name) : name_(std::move(name)) {}

Command* minicli::Command::AddCommand(const std::string& name) {
  if (sub_cmd_map_.find(name) != sub_cmd_map_.end()) {
    MINICLI_THROW_OR_RETURN(std::runtime_error("Duplicated commands"), nullptr);
  }
  CommandPtr cmd(new Command(name));
  auto& store = sub_cmd_map_[name];
  store = std::move(cmd);
  return store.get();
}

Command* Command::GetCommand(const std::string& name) {
  auto iterator = sub_cmd_map_.find(name);
  if (iterator == sub_cmd_map_.end()) {
    MINICLI_THROW_OR_RETURN(std::runtime_error("non-existing command"), nullptr);
  }

  return iterator->second.get();
}

Command* Command::Callback(const CallbackType& callback) {
  callback_ = callback;
  return this;
}
BaseOption* Command::GetOption(const std::string& match) {
  auto iterator = option_map_.find(match);
  if (iterator == option_map_.end()) {
    MINICLI_THROW_OR_RETURN(std::runtime_error("Non-existing option"), nullptr);
  }

  return iterator->second.get();
}
const CallbackType& Command::GetCallback() { return callback_; }
}  // namespace minicli
