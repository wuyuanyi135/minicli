//
// Created by wuyua on 2/1/2022.
//
#include "app.hpp"

#include <sstream>
#include <utility>
namespace minicli {

ErrorCode App::ParseLine(const std::string& line) {
  // Segment the string by spaces.
  std::stringstream ss(line);
  std::vector<std::string> args{};
  std::string arg;
  while (ss >> arg) {
    args.push_back(arg);
  }
  return ParseArgs(args);
}
ErrorCode App::ParseArgs(const std::vector<std::string>& args) {
  Command* current_cmd = this;

  // queued callbacks for options. Only after all checks passed will the options
  // commit.
  std::vector<BaseOption*> actions;

  // with this flag, no more subcommand should be followed.
  bool option_start_flag = false;

  for (auto arg = args.begin(); arg != args.end(); arg++) {
    if (arg->compare(0, 1, "-") == 0) {
      // begin with "-", it is an option
      std::string stripped(arg->begin() + 1, arg->end());

      auto opt = current_cmd->GetOption(stripped);
      if (opt == nullptr) {
        MINICLI_THROW_OR_RETURN(
            std::runtime_error(kErrorMessages[INVALID_OPTION]), INVALID_OPTION);
      }

      // extract next token as the argument of this option.
      if (++arg == args.end()) {
        MINICLI_THROW_OR_RETURN(
            std::runtime_error(kErrorMessages[MALFORMED_OPTION]),
            MALFORMED_OPTION);
      }
      auto val = *arg;

      if (!opt->Check(val)) {
        MINICLI_THROW_OR_RETURN(
            std::runtime_error(kErrorMessages[FAILED_CHECK]), FAILED_CHECK);
      }
      // All check has passed for this option
      actions.push_back(opt);
      option_start_flag = true;

    } else {
      // is a subcommand
      if (option_start_flag) {
        MINICLI_THROW_OR_RETURN(
            std::runtime_error(kErrorMessages[INVALID_SUBCOMMAND_POS]),
            INVALID_SUBCOMMAND_POS);
      }

      auto cmd = current_cmd->GetCommand(*arg);
      if (cmd == nullptr) {
        MINICLI_THROW_OR_RETURN(
            std::runtime_error(kErrorMessages[INVALID_COMMAND]),
            INVALID_COMMAND);
      }

      current_cmd = cmd;
    }
  }

  // Execute queued actions
  for (const auto& action : actions) {
    action->Commit();
  }

  // Execute subcommand callback
  current_cmd->GetCallback()();
  return OK;
}
}  // namespace minicli