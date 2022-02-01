//
// Created by wuyua on 2/1/2022.
//

#ifndef MINICLI_ENABLE_EXCEPTION
#error \
    "This example requires exception enabled. Compile with -DMINICLI_ENABLE_EXCEPTION"
#endif

#include <iostream>
#include <vector>

#include "minicli.hpp"
using namespace minicli;

int main(int argc, char** argv) {
  if (argc == 1) {
    std::cout << "Nothing to do." << std::endl;
    return 0;
  }

  std::vector<std::string> args;

  // ignore the first argument.
  for (int i = 1; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }

  App app;
  double x = 0, y = 0;
  std::string result_name = "z";

  auto add_cmd = app.AddCommand("add");
  add_cmd->AddOption("x", x);
  add_cmd->AddOption("y", y);
  add_cmd->AddOption("result", result_name);
  add_cmd->Callback(
      [&] { std::cout << result_name << "=" << x + y << std::endl; });

  auto sub_cmd = app.AddCommand("sub");
  sub_cmd->AddOption("x", x);
  sub_cmd->AddOption("y", y);
  sub_cmd->AddOption("result", result_name);
  sub_cmd->Callback(
      [&] { std::cout << result_name << "=" << x - y << std::endl; });

  auto mul_cmd = app.AddCommand("mul");
  mul_cmd->AddOption("x", x);
  mul_cmd->AddOption("y", y);
  mul_cmd->AddOption("result", result_name);
  mul_cmd->Callback(
      [&] { std::cout << result_name << "=" << x * y << std::endl; });
  auto div_cmd = app.AddCommand("div");
  div_cmd->AddOption("x", x);
  div_cmd->AddOption("y", y)->AddChecker(make_checker<NotEqual<double>>(0));
  div_cmd->AddOption("result", result_name);
  div_cmd->Callback(
      [&] { std::cout << result_name << "=" << x * y << std::endl; });

  try {
    app.ParseArgs(args);
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}