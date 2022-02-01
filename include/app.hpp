//
// Created by wuyua on 2/1/2022.
//

#ifndef MINICLI_APP_H_
#define MINICLI_APP_H_

#include "command.hpp"
namespace minicli {

/// Error code returned by ParseLine and ParseArgs Only useful when
/// MINICLI_ENABLE_EXCEPTION is not defined.
enum ErrorCode {
  OK = 0,
  INVALID_OPTION,
  FAILED_CHECK,
  INVALID_SUBCOMMAND_POS,
  INVALID_COMMAND,
  MALFORMED_OPTION,
};

inline const char* kErrorMessages[] = {
    "No error",
    "Invalid option",
    "Failed check",
    "Subcommand appears after an option",
    "Invalid command",
    "Malformed option (missing argument)"};

/// Translate error code into string
/// \param ec
/// \return
inline const char* TranslateErrorCode(ErrorCode ec) {
  return kErrorMessages[ec];
}

class App : public Command {
 public:
  App() = default;
  /// Parse a space-delimited string
  /// \param line
  /// \return
  ErrorCode ParseLine(const std::string& line);

  /// Parse a vector of parsed tokens
  /// \param args
  /// \return
  ErrorCode ParseArgs(const std::vector<std::string>& args);
};
}  // namespace minicli

#endif  // MINICLI_APP_H_
