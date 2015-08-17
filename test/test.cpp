#include "test.h"
#include "../src/config.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QtTest/QTest>

void Test::is_config_valid_test_data()
{
  QFile file("default.conf");
  file.open(QIODevice::ReadOnly | QIODevice::Text);

  QTextStream in(&file);
  QString conf = in.readAll();

  file.close();

  QTest::addColumn<QString>("conf");
  QTest::newRow("default config") << conf;
}

void Test::is_config_valid_test()
{
  Config config("../objects");

  Options& options_global = config.get_global_options();
  set_option(options_global, "stats-freq", "0");
  set_option(options_global, "flush-lines", "0");
  set_option(options_global, "time-reopen", "10");
  set_option(options_global, "use-dns", "no");
  set_option(options_global, "keep-hostname", "yes");
  set_option(options_global, "perm", "0640");
  set_option(options_global, "group", "log");

  std::shared_ptr<Object> system = add_object(config, "system", "source");
  std::shared_ptr<Object> internal = add_object(config, "internal", "source");

  std::shared_ptr<ObjectStatement> src = add_object_statement(config, "src");
  src->add_object(system, 0);
  src->add_object(internal, 1);

  std::shared_ptr<Object> d_authlog = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_syslog = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_cron = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_daemon = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_kernel = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_lpr = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_user = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_uucp = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_mail = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_news = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_ppp = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_debug = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_messages = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_errors = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_everything = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_iptables = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_acpid = add_object(config, "file", "destination");
  std::shared_ptr<Object> d_console = add_object(config, "usertty", "destination");
  std::shared_ptr<Object> d_console_all = add_object(config, "file", "destination");

  std::shared_ptr<Object> f_auth = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_authpriv = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_syslog = add_object(config, "program", "filter");
  std::shared_ptr<Object> f_cron = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_daemon = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_lpr = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_mail = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_news = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_user = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_uucp = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_ppp = add_object(config, "facility", "filter");
  std::shared_ptr<Object> f_emergency = add_object(config, "level", "filter");
  std::shared_ptr<Object> f_info = add_object(config, "level", "filter");
  std::shared_ptr<Object> f_notice = add_object(config, "level", "filter");
  std::shared_ptr<Object> f_warn = add_object(config, "level", "filter");
  std::shared_ptr<Object> f_crit = add_object(config, "level", "filter");
  std::shared_ptr<Object> f_err = add_object(config, "level", "filter");
  std::shared_ptr<Object> f_acpid = add_object(config, "program", "filter");

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
/*
  add_objects_to_log_statement(config, src, f_acpid, d_acpid);
  add_objects_to_log_statement(config, src, f_authpriv, d_authlog);
  add_objects_to_log_statement(config, src, f_syslog, d_syslog);
  add_objects_to_log_statement(config, src, f_cron, d_cron);
  add_objects_to_log_statement(config, src, f_daemon, d_daemon);
  add_objects_to_log_statement(config, src, d_kernel);
  add_objects_to_log_statement(config, src, f_lpr, d_lpr);
  add_objects_to_log_statement(config, src, f_mail, d_mail);
  add_objects_to_log_statement(config, src, f_news, d_news);
  add_objects_to_log_statement(config, src, f_ppp, d_ppp);
  add_objects_to_log_statement(config, src, f_user, d_user);
  add_objects_to_log_statement(config, src, f_uucp, d_uucp);
  add_objects_to_log_statement(config, src, d_debug);
  add_objects_to_log_statement(config, src, d_messages);
  add_objects_to_log_statement(config, src, f_err, d_errors);
  add_objects_to_log_statement(config, src, f_emergency, d_console);
  add_objects_to_log_statement(config, src, d_everything);
  add_objects_to_log_statement(config, src, d_iptables);
  add_objects_to_log_statement(config, src, d_console_all);
*/
  QFile file("test.conf");
  file.open(QIODevice::WriteOnly | QIODevice::Text);

  QTextStream out(&file);
  out << QString::fromStdString(config.to_string());

  file.close();

  QFETCH(QString, conf);
  QCOMPARE(QString::fromStdString(config.to_string()), conf);
}

void Test::set_option(Object& object, const std::string& option_name, const std::string& option_value)
{
  std::vector< std::unique_ptr<Option> >& options = object.get_options();
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

std::shared_ptr<Object> Test::add_object(Config& config, const std::string& object_name, const std::string& object_type)
{
  const Object& default_object = config.get_default_object(object_name, object_type);
  Object* object = default_object.clone();
  std::shared_ptr<Object> new_object(object);

  return new_object;
}

std::shared_ptr<ObjectStatement> Test::add_object_statement(Config& config, const std::string& object_statement_name)
{
  ObjectStatement* object_statement = new ObjectStatement(object_statement_name);
  std::shared_ptr<ObjectStatement> new_object_statement = config.add_object_statement(object_statement);

  return new_object_statement;
}

template<typename... Targs>
void Test::add_objects_to_log_statement(Config& config, Targs&... more)
{
  const Options& options = static_cast<const Options&>(config.get_default_object("log", "options"));
  LogStatement* new_log_statement = new LogStatement(options);

  std::shared_ptr<LogStatement> log_statement = config.add_log_statement(new_log_statement);
  add_object_to_log_statement_recursive(log_statement, more...);
}

template<typename... Targs>
void Test::add_object_to_log_statement_recursive(std::shared_ptr<LogStatement>& log_statement,
                                                 std::shared_ptr<ObjectStatement>& object_statement, Targs&... more)
{
  const std::shared_ptr<ObjectStatement> new_object_statement = object_statement;
  log_statement->add_object_statement(new_object_statement, 0);

  add_object_to_log_statement_recursive(log_statement, more...);
}

QTEST_MAIN(Test)
