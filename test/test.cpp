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
"destination d_sql0 {\n"
"    sql(\n"
"        password(\"test\")\n"
"        table(\"test\")\n"
"        username(\"test\")\n"
"    );\n"
"};\n"
"\n"
"log { source(s_file0); destination(d_sql0); };\n"
"\n";

void Test::is_config_valid_test_data()
{
  QTest::addColumn<QString>("conf");

  QTest::newRow("log1") << log1;
}

void Test::is_config_valid_test()
{
  Config config;

  const auto src_cit = std::find_if(config.get_default_drivers().cbegin(), config.get_default_drivers().cend(),
                                    [](const DefaultDriver& driver)->bool {
                                      return driver.get_name() == "file" && driver.get_type() == "source";
                                    });
  Driver src(*src_cit, 0);
  for (Option& option : src.get_options())
  {
    if (option.get_name() == "file")
    {
      option.set_current_value("test");
    }
  }

  const auto dst_cit = std::find_if(config.get_default_drivers().cbegin(), config.get_default_drivers().cend(),
                                    [](const DefaultDriver& driver)->bool {
                                      return driver.get_name() == "sql" && driver.get_type() == "destination";
                                    });
  Driver dst(*dst_cit, 0);
  for (Option& option : dst.get_options())
  {
    if (option.get_name() == "username")
    {
      option.set_current_value("test");
    }

    if (option.get_name() == "password")
    {
      option.set_current_value("test");
    }

    if (option.get_name() == "table")
    {
      option.set_current_value("test");
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
