#include "test.h"
#include "../config.h"

#include <QString>
#include <QtTest/QTest>

#include <sstream>
#include <iostream>

QString log1 = ""
"options {\n"
"    log-fifo-size(1000);\n"
"    use-dns(no);\n"
"};\n"
"\n"
"source s_file0 {\n"
"    file(\"test\"\n"
"    );\n"
"};\n"
"\n"
"destination d_network0 {\n"
"    network(\"test\"\n"
"        port(12345)\n"
"    );\n"
"};\n"
"\n"
"log { source(s_file0); destination(d_network0); };\n"
"\n";

void Test::is_config_valid_test_data()
{
  QTest::addColumn<QString>("conf");

  QTest::newRow("log1") << log1;
}

void Test::is_config_valid_test()
{
  Config config("../drivers");

  GlobalOptions& global_options = config.get_global_options();
  for (Option& option : global_options.get_options())
  {
    if (option.get_name() == "use-dns")
    {
      option.set_current_value("no");
    }
    if (option.get_name() == "log-fifo-size")
    {
      option.set_current_value("1000");
    }
  }

  const Driver& dsrc = config.get_default_driver("file", DriverType::source);
  Driver src(dsrc);
  for (Option& option : src.get_options())
  {
    if (option.get_name() == "file")
    {
      option.set_current_value("test");
    }
  }

  const Driver& ddst = config.get_default_driver("network", DriverType::destination);
  Driver dst(ddst);
  for (Option& option : dst.get_options())
  {
    if (option.get_name() == "network")
    {
      option.set_current_value("test");
    }

    if (option.get_name() == "port")
    {
      option.set_current_value("12345");
    }
  }

  Log log;
  log.add_driver(src);
  log.add_driver(dst);

  config.add_driver(src);
  config.add_driver(dst);
  config.add_log(log);

  std::stringstream ss;
  ss << config;

  QFETCH(QString, conf);

  std::cerr << ss.str() << std::endl << conf.toStdString() << std::endl;

  QCOMPARE(ss.str(), conf.toStdString());
}

QTEST_MAIN(Test)
