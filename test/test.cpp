#include "test.h"
#include "../src/config.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QtTest/QTest>

#include <iostream>

void Test::is_config_valid_test_data()
{
  QFile file("default.conf");
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    return;
  }

  QTextStream in(&file);
  QString conf = in.readAll();

  QTest::addColumn<QString>("conf");

  QTest::newRow("default config") << conf;
}

void Test::is_config_valid_test()
{
  Config config("../drivers");

  Options& options_global = config.get_global_options()->get_options();
  set_option(options_global, "stats-freq", "0");
  set_option(options_global, "flush-lines", "0");
  set_option(options_global, "time-reopen", "10");
  set_option(options_global, "use-dns", "no");
  set_option(options_global, "keep-hostname", "yes");
  set_option(options_global, "perm", "0640");
  set_option(options_global, "group", "log");

  std::shared_ptr<Driver> system = add_driver(config,"system", "source");
  std::shared_ptr<Driver> internal = add_driver(config,"internal", "source");

  std::shared_ptr<Driver> d_authlog = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_syslog = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_cron = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_daemon = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_kernel = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_lpr = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_user = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_uucp = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_mail = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_news = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_ppp = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_debug = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_messages = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_errors = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_everything = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_iptables = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_acpid = add_driver(config, "file", "destination");
  std::shared_ptr<Driver> d_console = add_driver(config, "usertty", "destination");
  std::shared_ptr<Driver> d_console_all = add_driver(config, "file", "destination");

  std::shared_ptr<Driver> f_auth = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_authpriv = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_syslog = add_driver(config, "program", "filter");
  std::shared_ptr<Driver> f_cron = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_daemon = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_lpr = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_mail = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_news = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_user = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_uucp = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_ppp = add_driver(config, "facility", "filter");
  std::shared_ptr<Driver> f_emergency = add_driver(config, "level", "filter");
  std::shared_ptr<Driver> f_info = add_driver(config, "level", "filter");
  std::shared_ptr<Driver> f_notice = add_driver(config, "level", "filter");
  std::shared_ptr<Driver> f_warn = add_driver(config, "level", "filter");
  std::shared_ptr<Driver> f_crit = add_driver(config, "level", "filter");
  std::shared_ptr<Driver> f_err = add_driver(config, "level", "filter");
  std::shared_ptr<Driver> f_acpid = add_driver(config, "program", "filter");

  set_option(*d_authlog, "file", "/var/log/auth.log");
  set_option(*d_syslog, "file", "/var/log/syslog.log");
  set_option(*d_cron, "file", "/var/log/crond.log");
  set_option(*d_daemon, "file", "/var/log/daemon.log");
  set_option(*d_kernel, "file", "/var/log/kernel.log");
  set_option(*d_lpr, "file", "/var/log/lpr.log");
  set_option(*d_user, "file", "/var/log/user.log");
  set_option(*d_uucp, "file", "/var/log/uucp.log");
  set_option(*d_mail, "file", "/var/log/mail.log");
  set_option(*d_news, "file", "/var/log/news.log");
  set_option(*d_ppp, "file", "/var/log/ppp.log");
  set_option(*d_debug, "file", "/var/log/debug.log");
  set_option(*d_messages, "file", "/var/log/messages.log");
  set_option(*d_errors, "file", "/var/log/errors.log");
  set_option(*d_everything, "file", "/var/log/everything.log");
  set_option(*d_iptables, "file", "/var/log/iptables.log");
  set_option(*d_acpid, "file", "/var/log/acpid.log");
  set_option(*d_console, "usertty", "root");
  set_option(*d_console_all, "file", "/dev/tty12");

  set_option(*f_auth, "facility", "auth");
  set_option(*f_authpriv, "facility", "auth, authpriv");
  set_option(*f_syslog, "program", "syslog-ng");
  set_option(*f_cron, "facility", "cron");
  set_option(*f_daemon, "facility", "daemon");
  set_option(*f_lpr, "facility", "lpr");
  set_option(*f_mail, "facility", "mail");
  set_option(*f_news, "facility", "news");
  set_option(*f_user, "facility", "user");
  set_option(*f_uucp, "facility", "uucp");
  set_option(*f_ppp, "facility", "local2");
  set_option(*f_emergency, "level", "emerg");
  set_option(*f_info, "level", "info");
  set_option(*f_notice, "level", "notice");
  set_option(*f_warn, "level", "warn");
  set_option(*f_crit, "level", "crit");
  set_option(*f_err, "level", "err");
  set_option(*f_acpid, "program", "acpid");

  std::vector< std::shared_ptr<Log> > logs;
  logs.push_back(add_drivers_to_log(config, system, internal, f_acpid, d_acpid));
  logs.push_back(add_drivers_to_log(config, system, internal, f_authpriv, d_authlog));
  logs.push_back(add_drivers_to_log(config, system, internal, f_syslog, d_syslog));
  logs.push_back(add_drivers_to_log(config, system, internal, f_cron, d_cron));
  logs.push_back(add_drivers_to_log(config, system, internal, f_daemon, d_daemon));
  logs.push_back(add_drivers_to_log(config, system, internal, d_kernel));
  logs.push_back(add_drivers_to_log(config, system, internal, f_lpr, d_lpr));
  logs.push_back(add_drivers_to_log(config, system, internal, f_mail, d_mail));
  logs.push_back(add_drivers_to_log(config, system, internal, f_news, d_news));
  logs.push_back(add_drivers_to_log(config, system, internal, f_ppp, d_ppp));
  logs.push_back(add_drivers_to_log(config, system, internal, f_user, d_user));
  logs.push_back(add_drivers_to_log(config, system, internal, f_uucp, d_uucp));
  logs.push_back(add_drivers_to_log(config, system, internal, d_debug));
  logs.push_back(add_drivers_to_log(config, system, internal, d_messages));
  logs.push_back(add_drivers_to_log(config, system, internal, f_err, d_errors));
  logs.push_back(add_drivers_to_log(config, system, internal, f_emergency, d_console));
  logs.push_back(add_drivers_to_log(config, system, internal, d_everything));
  logs.push_back(add_drivers_to_log(config, system, internal, d_iptables));
  logs.push_back(add_drivers_to_log(config, system, internal, d_console_all));

  QFETCH(QString, conf);

  std::cerr << config.to_string() << std::endl << conf.toStdString() << std::endl;

  QCOMPARE(QString::fromStdString(config.to_string()), conf);
}

void Test::set_option(Driver& driver, const std::string& option_name, const std::string& option_value)
{
  std::vector< std::unique_ptr<Option> >& options = driver.get_options();
  auto it = std::find_if(options.begin(), options.end(),
                         [&option_name](std::unique_ptr<Option>& option)->bool {
                           return option->get_name() == option_name;
                         });

  if (it != options.end())
  {
    std::unique_ptr<Option>& option = *it;
    option->set_current(option_value);
  }
}

std::shared_ptr<Driver> Test::add_driver(Config& config, const std::string& driver_name, const std::string& driver_type)
{
  const Driver& default_driver = config.get_default_driver(driver_name, driver_type);
  std::unique_ptr<Driver> new_driver(default_driver.clone());

  return config.add_driver(new_driver);
}

template<typename... Targs>
std::shared_ptr<Log> Test::add_drivers_to_log(Config& config, Targs&... more)
{
  const Driver& log_options = config.get_default_driver("log", "options");
  std::unique_ptr<Log> new_log = std::make_unique<Log>(static_cast<const Options&>(log_options));

  std::shared_ptr<Log> log = config.add_log(new_log);
  add_driver_to_log_recursive(log, more...);

  return log;
}

template<typename... Targs>
void Test::add_driver_to_log_recursive(std::shared_ptr<Log>& log, std::shared_ptr<Driver>& driver, Targs&... more)
{
  const std::shared_ptr<Driver> d = driver;
  log->add_driver(d);

  add_driver_to_log_recursive(log, more...);
}

QTEST_MAIN(Test)
