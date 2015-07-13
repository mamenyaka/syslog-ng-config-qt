#include "test.h"
#include "../config.h"

#include <QString>
#include <QtTest/QTest>

#include <sstream>
#include <iostream>

QString log1 = ""
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

  const DefaultDriver& dsrc = config.get_default_driver("file", "source");
  Driver src(dsrc, 0);
  for (Option& option : src.get_options())
  {
    if (option.get_name() == "file")
    {
      option.set_current_value("test");
    }
  }

  const DefaultDriver& ddst = config.get_default_driver("network", "destination");
  Driver dst(ddst, 0);
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
  log.add_driver(&src);
  log.add_driver(&dst);

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
