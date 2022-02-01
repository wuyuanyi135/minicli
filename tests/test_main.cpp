#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "minicli.hpp"

using namespace minicli;

#ifdef MINICLI_ENABLE_EXCEPTION
#define CHECK_EXCEPTION_OR_ERROR_CODE(expr, ex, ec) CHECK_THROWS_AS(expr, ex);
#else
#define CHECK_EXCEPTION_OR_ERROR_CODE(expr, ex, ec) CHECK(expr == ec);
#endif

// Normal usage cases
TEST_CASE("Subcommand without options") {
  App app;
  bool called = false;
  app.AddCommand("hello")->Callback([&] { called = true; });

  app.ParseLine("hello");

  CHECK(called);
}

TEST_CASE("Subcommand with one option") {
  App app;
  int v;
  auto cmd = app.AddCommand("hello");
  cmd->AddOption("v", v);
  app.ParseLine("hello -v 10");

  CHECK_EQ(v, 10);
}

TEST_CASE("Subcommand with multiple options") {
  App app;
  int v;
  std::string se;
  auto cmd = app.AddCommand("hello");
  cmd->AddOption("v", v);
  cmd->AddOption("se", se);
  app.ParseLine("hello -v 10 -se bye");

  CHECK_EQ(v, 10);
  CHECK_EQ(se, "bye");
}

TEST_CASE("Nested subcommands with options") {
  App app;
  int v;
  auto cmd0 = app.AddCommand("hello");
  auto cmd1 = cmd0->AddCommand("world");
  auto cmd2 = cmd1->AddCommand("minicli");
  cmd2->AddOption("v", v);

  bool cmd1_called = false;
  bool cmd2_called = false;
  cmd1->Callback([&] { cmd1_called = true; });
  cmd2->Callback([&] { cmd2_called = true; });

  app.ParseLine("hello world minicli -v 10");
  CHECK(!cmd1_called);
  CHECK(cmd2_called);
  CHECK_EQ(v, 10);
}

TEST_CASE("Option with check") {
  App app;
  int v;
  auto cmd = app.AddCommand("hello");
  auto opt =
      cmd->AddOption("v", v)->AddChecker(make_checker<Range<int>>(0, 100));

  // when passing a variable, it must be moved.
  auto checker = make_checker<Range<int>>(0, 10);
  opt->AddChecker(std::move(checker));

  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.ParseLine("hello -v 101"), const std::runtime_error&, FAILED_CHECK);

  CHECK_NE(v, 101);
}

// Failure cases
TEST_CASE("Non existing subcommand") {
  App app;
  ErrorCode ret;
  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.GetCommand("hello"), const std::runtime_error&, nullptr);
  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.ParseLine("hello"), const std::runtime_error&, INVALID_COMMAND)
}

TEST_CASE("Non existing option") {
  App app;
  ErrorCode ret;
  app.AddCommand("hello");
  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.GetOption("v"), const std::runtime_error&, nullptr);
  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.ParseLine("hello -v"), const std::runtime_error&, INVALID_OPTION)
}

TEST_CASE("Duplicating commands") {
  App app;
  ErrorCode ret;
  auto cmd = app.AddCommand("hello");
  CHECK(cmd != nullptr);
  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.AddCommand("hello"), const std::runtime_error&, nullptr);
}

TEST_CASE("Duplicating options") {
  App app;
  ErrorCode ret;
  int v;
  auto cmd = app.AddCommand("hello");
  auto opt = cmd->AddOption("v", v);
  CHECK(opt != nullptr);
  CHECK_EXCEPTION_OR_ERROR_CODE(
      cmd->AddOption("v", v), const std::runtime_error&, nullptr);
}

TEST_CASE("Incorrect subcommand position") {
  App app;
  int v, w;
  auto cmd0 = app.AddCommand("hello");
  cmd0->AddOption("v", v);

  auto cmd1 = cmd0->AddCommand("world");
  cmd1->AddOption("w", w);

  CHECK_EQ(app.ParseLine("hello world -w 10"), OK);
  CHECK_EQ(w, 10);

  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.ParseLine("hello -v 5 world"),
      const std::runtime_error&,
      INVALID_SUBCOMMAND_POS);
}

TEST_CASE("Invalid option type") {
  App app;
  int v;
  auto cmd0 = app.AddCommand("hello");
  cmd0->AddOption("v", v);

  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.ParseLine("hello -v not-a-int"),
      const std::runtime_error&,
      FAILED_CHECK);
}

TEST_CASE("Malformed option (missing argument)") {
  App app;
  int v;
  auto cmd0 = app.AddCommand("hello");
  cmd0->AddOption("v", v);

  CHECK_EXCEPTION_OR_ERROR_CODE(
      app.ParseLine("hello -v"),
      const std::runtime_error&,
      MALFORMED_OPTION);
}