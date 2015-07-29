#include "test.h"
#include "../src/config.h"

#include <QString>
#include <QtTest/QTest>

#include <iostream>

// TODO
QString conf;

void Test::is_config_valid_test_data()
{
  QTest::addColumn<QString>("conf");

  QTest::newRow("default config") << conf;
}

void Test::is_config_valid_test()
{
  Config config("../drivers");

  GlobalOptions& global_options = config.get_global_options();
  set_option(global_options, "stats-freq", "0");
  set_option(global_options, "flush-lines", "0");
  set_option(global_options, "time-reopen", "10");
  set_option(global_options, "use-dns", "no");
  set_option(global_options, "keep-hostname", "yes");
  set_option(global_options, "perm", "0640");
  set_option(global_options, "group", "log");

  std::vector<Driver> drivers;
  Driver system(config.get_default_driver("system", DriverType::source));
  Driver internal(config.get_default_driver("internal", DriverType::source));

  Driver authlog(config.get_default_driver("file", DriverType::destination));
  set_option(authlog, "file", "/var/log/auth.log");
  Driver syslog(config.get_default_driver("file", DriverType::destination));
  set_option(syslog, "file", "/var/log/syslog.log");
  Driver cron(config.get_default_driver("file", DriverType::destination));
  set_option(cron, "file", "/var/log/crond.log");
  Driver daemon(config.get_default_driver("file", DriverType::destination));
  set_option(daemon, "file", "/var/log/daemon.log");
  Driver kernel(config.get_default_driver("file", DriverType::destination));
  set_option(kernel, "file", "/var/log/kernel.log");
  Driver lpr(config.get_default_driver("file", DriverType::destination));
  set_option(lpr, "file", "/var/log/lpr.log");
  Driver user(config.get_default_driver("file", DriverType::destination));
  set_option(user, "file", "/var/log/user.log");
  Driver uucp(config.get_default_driver("file", DriverType::destination));
  set_option(uucp, "file", "/var/log/uucp.log");
  Driver mail(config.get_default_driver("file", DriverType::destination));
  set_option(mail, "file", "/var/log/mail.log");
  Driver news(config.get_default_driver("file", DriverType::destination));
  set_option(news, "file", "/var/log/news.log");
  Driver ppp(config.get_default_driver("file", DriverType::destination));
  set_option(ppp, "file", "/var/log/ppp.log");
  Driver debug(config.get_default_driver("file", DriverType::destination));
  set_option(debug, "file", "/var/log/debug.log");
  Driver messages(config.get_default_driver("file", DriverType::destination));
  set_option(messages, "file", "/var/log/messages.log");
  Driver errors(config.get_default_driver("file", DriverType::destination));
  set_option(errors, "file", "/var/log/errors.log");
  Driver everything(config.get_default_driver("file", DriverType::destination));
  set_option(everything, "file", "/var/log/everything.log");
  Driver iptables(config.get_default_driver("file", DriverType::destination));
  set_option(iptables, "file", "/var/log/iptables.log");
  Driver acpid(config.get_default_driver("file", DriverType::destination));
  set_option(acpid, "file", "/var/log/acpid.log");
  Driver console(config.get_default_driver("usertty", DriverType::destination));
  set_option(console, "usertty", "root");
  Driver console_all(config.get_default_driver("file", DriverType::destination));
  set_option(console_all, "file", "/dev/tty12");

  std::shared_ptr<Driver> system_ptr = config.add_driver(system);
  std::shared_ptr<Driver> internal_ptr = config.add_driver(internal);

  std::shared_ptr<Driver> authlog_ptr = config.add_driver(authlog);
  std::shared_ptr<Driver> syslog_ptr = config.add_driver(syslog);
  std::shared_ptr<Driver> cron_ptr = config.add_driver(cron);
  std::shared_ptr<Driver> daemon_ptr = config.add_driver(daemon);
  std::shared_ptr<Driver> kernel_ptr = config.add_driver(kernel);
  std::shared_ptr<Driver> lpr_ptr = config.add_driver(lpr);
  std::shared_ptr<Driver> user_ptr = config.add_driver(user);
  std::shared_ptr<Driver> uucp_ptr = config.add_driver(uucp);
  std::shared_ptr<Driver> mail_ptr = config.add_driver(mail);
  std::shared_ptr<Driver> news_ptr = config.add_driver(news);
  std::shared_ptr<Driver> ppp_ptr = config.add_driver(ppp);
  std::shared_ptr<Driver> debug_ptr = config.add_driver(debug);
  std::shared_ptr<Driver> messages_ptr = config.add_driver(messages);
  std::shared_ptr<Driver> errors_ptr = config.add_driver(errors);
  std::shared_ptr<Driver> everything_ptr = config.add_driver(everything);
  std::shared_ptr<Driver> iptables_ptr = config.add_driver(iptables);
  std::shared_ptr<Driver> acpid_ptr = config.add_driver(acpid);
  std::shared_ptr<Driver> console_ptr = config.add_driver(console);
  std::shared_ptr<Driver> console_all_ptr = config.add_driver(console_all);

  std::vector<LogUPtr> logs;
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, acpid_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, authlog_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, syslog_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, cron_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, daemon_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, kernel_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, lpr_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, mail_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, news_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, ppp_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, user_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, uucp_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, debug_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, messages_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, errors_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, console_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, everything_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, iptables_ptr));
  logs.push_back(add_drivers_to_log(config, system_ptr, internal_ptr, console_all_ptr));

  QFETCH(QString, conf);

  std::cerr << config.to_string() << std::endl;// << conf.toStdString() << std::endl;

  QCOMPARE(QString::fromStdString(config.to_string()), conf);
}

void Test::set_option(Driver& driver, const std::string& option_name, const std::string& option_value)
{
  auto& options = driver.get_options();
  auto it = std::find_if(options.begin(), options.end(),
                         [&option_name](const auto& option)->bool {
                           return option->get_name() == option_name;
                         });

  if (it != options.end())
  {
    auto& option = *it;
    // TODO
  }
}

template<typename... Targs>
LogUPtr Test::add_drivers_to_log(Config& config, Targs&... more)
{
  Log new_log = config.get_default_driver("log", DriverType::options);
  LogUPtr log = config.add_log(new_log);
  add_driver_to_log_recursive(log, more...);

  return log;
}

template<typename... Targs>
void Test::add_driver_to_log_recursive(LogUPtr& log, std::shared_ptr<Driver>& driver, Targs&... more)
{
  std::shared_ptr<const Driver> d = driver;
  log->add_driver(d);

  add_driver_to_log_recursive(log, more...);
}

void Test::add_driver_to_log_recursive(LogUPtr& log)
{}

QTEST_MAIN(Test)
