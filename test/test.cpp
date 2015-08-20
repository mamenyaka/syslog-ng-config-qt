/*
 * Copyright (C) 2015 Andras Mamenyak
 *
 * This file is part of syslog-ng-config-qt.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

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

  std::vector<std::string> object_names {
    "d_authlog", "d_syslog", "d_cron", "d_daemon", "d_kernel", "d_lpr", "d_user", "d_uucp", "d_mail", "d_news", "d_ppp",
    "d_debug", "d_messages", "d_errors", "d_everything", "d_iptables", "d_acpid", "d_console", "d_console_all",

    "f_auth", "f_authpriv", "f_syslog", "f_cron", "f_daemon", "f_lpr", "f_mail", "f_news", "f_user", "f_uucp",
    "f_ppp", "f_emergency", "f_info", "f_notice", "f_warn", "f_crit", "f_err", "f_acpid"
  };

  std::map< std::string, std::pair<std::string, std::string> > objects_map;
  objects_map.emplace("d_authlog", std::make_pair("file", "destination"));
  objects_map.emplace("d_syslog", std::make_pair("file", "destination"));
  objects_map.emplace("d_cron", std::make_pair("file", "destination"));
  objects_map.emplace("d_daemon", std::make_pair("file", "destination"));
  objects_map.emplace("d_kernel", std::make_pair("file", "destination"));
  objects_map.emplace("d_lpr", std::make_pair("file", "destination"));
  objects_map.emplace("d_user", std::make_pair("file", "destination"));
  objects_map.emplace("d_uucp", std::make_pair("file", "destination"));
  objects_map.emplace("d_mail", std::make_pair("file", "destination"));
  objects_map.emplace("d_news", std::make_pair("file", "destination"));
  objects_map.emplace("d_ppp", std::make_pair("file", "destination"));
  objects_map.emplace("d_debug", std::make_pair("file", "destination"));
  objects_map.emplace("d_messages", std::make_pair("file", "destination"));
  objects_map.emplace("d_errors", std::make_pair("file", "destination"));
  objects_map.emplace("d_everything", std::make_pair("file", "destination"));
  objects_map.emplace("d_iptables", std::make_pair("file", "destination"));
  objects_map.emplace("d_acpid", std::make_pair("file", "destination"));
  objects_map.emplace("d_console", std::make_pair("usertty", "destination"));
  objects_map.emplace("d_console_all", std::make_pair("file", "destination"));

  objects_map.emplace("f_auth", std::make_pair("facility", "filter"));
  objects_map.emplace("f_authpriv", std::make_pair("facility", "filter"));
  objects_map.emplace("f_syslog", std::make_pair("program", "filter"));
  objects_map.emplace("f_cron", std::make_pair("facility", "filter"));
  objects_map.emplace("f_daemon", std::make_pair("facility", "filter"));
  objects_map.emplace("f_lpr", std::make_pair("facility", "filter"));
  objects_map.emplace("f_mail", std::make_pair("facility", "filter"));
  objects_map.emplace("f_news", std::make_pair("facility", "filter"));
  objects_map.emplace("f_user", std::make_pair("facility", "filter"));
  objects_map.emplace("f_uucp", std::make_pair("facility", "filter"));
  objects_map.emplace("f_ppp", std::make_pair("facility", "filter"));
  objects_map.emplace("f_emergency", std::make_pair("level", "filter"));
  objects_map.emplace("f_info", std::make_pair("level", "filter"));
  objects_map.emplace("f_notice", std::make_pair("level", "filter"));
  objects_map.emplace("f_warn", std::make_pair("level", "filter"));
  objects_map.emplace("f_crit", std::make_pair("level", "filter"));
  objects_map.emplace("f_err", std::make_pair("level", "filter"));
  objects_map.emplace("f_acpid", std::make_pair("program", "filter"));

  std::map< std::string, std::pair<std::string, std::string> > options_map;
  options_map.emplace("d_authlog", std::make_pair("file", "/var/log/auth.log"));
  options_map.emplace("d_syslog", std::make_pair("file", "/var/log/syslog.log"));
  options_map.emplace("d_cron", std::make_pair("file", "/var/log/crond.log"));
  options_map.emplace("d_daemon", std::make_pair("file", "/var/log/daemon.log"));
  options_map.emplace("d_kernel", std::make_pair("file", "/var/log/kernel.log"));
  options_map.emplace("d_lpr", std::make_pair("file", "/var/log/lpr.log"));
  options_map.emplace("d_user", std::make_pair("file", "/var/log/user.log"));
  options_map.emplace("d_uucp", std::make_pair("file", "/var/log/uucp.log"));
  options_map.emplace("d_mail", std::make_pair("file", "/var/log/mail.log"));
  options_map.emplace("d_news", std::make_pair("file", "/var/log/news.log"));
  options_map.emplace("d_ppp", std::make_pair("file", "/var/log/ppp.log"));
  options_map.emplace("d_debug", std::make_pair("file", "/var/log/debug.log"));
  options_map.emplace("d_messages", std::make_pair("file", "/var/log/messages.log"));
  options_map.emplace("d_errors", std::make_pair("file", "/var/log/errors.log"));
  options_map.emplace("d_everything", std::make_pair("file", "/var/log/everything.log"));
  options_map.emplace("d_iptables", std::make_pair("file", "/var/log/iptables.log"));
  options_map.emplace("d_acpid", std::make_pair("file", "/var/log/acpid.log"));
  options_map.emplace("d_console", std::make_pair("usertty", "root"));
  options_map.emplace("d_console_all", std::make_pair("file", "/dev/tty12"));

  options_map.emplace("f_auth", std::make_pair("facility", "auth"));
  options_map.emplace("f_authpriv", std::make_pair("facility", "auth, authpriv"));
  options_map.emplace("f_syslog", std::make_pair("program", "syslog-ng"));
  options_map.emplace("f_cron", std::make_pair("facility", "cron"));
  options_map.emplace("f_daemon", std::make_pair("facility", "daemon"));
  options_map.emplace("f_lpr", std::make_pair("facility", "lpr"));
  options_map.emplace("f_mail", std::make_pair("facility", "mail"));
  options_map.emplace("f_news", std::make_pair("facility", "news"));
  options_map.emplace("f_user", std::make_pair("facility", "user"));
  options_map.emplace("f_uucp", std::make_pair("facility", "uucp"));
  options_map.emplace("f_ppp", std::make_pair("facility", "local2"));
  options_map.emplace("f_emergency", std::make_pair("level", "emerg"));
  options_map.emplace("f_info", std::make_pair("level", "info"));
  options_map.emplace("f_notice", std::make_pair("level", "notice"));
  options_map.emplace("f_warn", std::make_pair("level", "warn"));
  options_map.emplace("f_crit", std::make_pair("level", "crit"));
  options_map.emplace("f_err", std::make_pair("level", "err"));
  options_map.emplace("f_acpid", std::make_pair("program", "acpid"));

  std::vector< std::vector<std::string> > logs {
    {"src", "f_acpid", "d_acpid"},
    {"src", "f_authpriv", "d_authlog"},
    {"src", "f_syslog", "d_syslog"},
    {"src", "f_cron", "d_cron"},
    {"src", "f_daemon", "d_daemon"},
    {"src", "f_kernel", "d_kernel"},
    {"src", "f_lpr", "d_lpr"},
    {"src", "f_mail", "d_mail"},
    {"src", "f_news", "d_news"},
    {"src", "f_ppp", "d_ppp"},
    {"src", "f_user", "d_user"},
    {"src", "f_uucp", "d_uucp"},
    {"src", "f_debug", "d_debug"},
    {"src", "f_messages", "d_messages"},
    {"src", "f_err", "d_errors"},
    {"src", "f_emergency", "d_console"},
    {"src", "f_everything", "d_everything"},
    {"src", "f_iptables", "d_iptables"},
    {"src", "d_console_all"}
  };

  std::shared_ptr<ObjectStatement> src = add_object_statement(config, "src");
  {
    std::shared_ptr<Object> system = add_object(config, "system", "source");
    std::shared_ptr<Object> internal = add_object(config, "internal", "source");

    src->add_object(system, 0);
    src->add_object(internal, 1);
  }

  for (const std::string& name : object_names)
  {
    std::pair<std::string, std::string> object_pair = objects_map[name];
    std::pair<std::string, std::string> option_pair = options_map[name];

    std::shared_ptr<Object> object = add_object(config, object_pair.first, object_pair.second);
    set_option(*object, option_pair.first, option_pair.second);

    std::shared_ptr<ObjectStatement>& object_statement = add_object_statement(config, name);
    object_statement->add_object(object, 0);
  }

  std::shared_ptr<ObjectStatement> f_kernel = add_object_statement(config, "f_kernel");
  {
    std::shared_ptr<Object> f_fac = add_object(config, "facility", "filter");
    set_option(*f_fac, "facility", "kern");
    static_cast<Filter&>(*f_fac).set_next("and");

    std::shared_ptr<Object> f_filt = add_object(config, "filter", "filter");
    set_option(*f_filt, "filter", "f_iptables");
    static_cast<Filter&>(*f_filt).set_invert(true);

    f_kernel->add_object(f_fac, 0);
    f_kernel->add_object(f_filt, 1);
  }

  std::shared_ptr<ObjectStatement> f_debug = add_object_statement(config, "f_debug");
  {
    std::shared_ptr<Object> f_fac = add_object(config, "facility", "filter");
    set_option(*f_fac, "facility", "auth, authpriv, news, mail");
    static_cast<Filter&>(*f_fac).set_invert(true);

    f_debug->add_object(f_fac, 0);
  }

  std::shared_ptr<ObjectStatement> f_messages = add_object_statement(config, "f_messages");
  {
    std::shared_ptr<Object> f_lev = add_object(config, "level", "filter");
    set_option(*f_lev, "level", "warning, notice, info");
    static_cast<Filter&>(*f_lev).set_next("and");

    std::shared_ptr<Object> f_fac = add_object(config, "facility", "filter");
    set_option(*f_fac, "facility", "auth, authpriv, mail, news, cron");
    static_cast<Filter&>(*f_fac).set_invert(true);
    static_cast<Filter&>(*f_fac).set_next("and");

    std::shared_ptr<Object> f_prog = add_object(config, "program", "filter");
    set_option(*f_prog, "program", "syslog-ng");
    static_cast<Filter&>(*f_prog).set_invert(true);
    static_cast<Filter&>(*f_prog).set_next("and");

    std::shared_ptr<Object> f_filt = add_object(config, "filter", "filter");
    set_option(*f_filt, "filter", "f_iptables");
    static_cast<Filter&>(*f_filt).set_invert(true);

    f_messages->add_object(f_lev, 0);
    f_messages->add_object(f_fac, 1);
    f_messages->add_object(f_prog, 2);
    f_messages->add_object(f_filt, 3);
  }

  std::shared_ptr<ObjectStatement> f_everything = add_object_statement(config, "f_everything");
  {
    std::shared_ptr<Object> f_lev = add_object(config, "level", "filter");
    set_option(*f_lev, "level", "emerg, alert, crit, err, warning, notice, info, debug");
    static_cast<Filter&>(*f_lev).set_next("and");

    std::shared_ptr<Object> f_fac = add_object(config, "facility", "filter");
    set_option(*f_fac, "facility", "auth, authpriv");
    static_cast<Filter&>(*f_fac).set_invert(true);

    f_everything->add_object(f_lev, 0);
    f_everything->add_object(f_fac, 1);
  }

  std::shared_ptr<ObjectStatement> f_iptables = add_object_statement(config, "f_iptables");
  {
    std::shared_ptr<Object> f_match_in = add_object(config, "match", "filter");
    set_option(*f_match_in, "match", "IN=");
    set_option(*f_match_in, "value", "MESSAGE");
    static_cast<Filter&>(*f_match_in).set_next("and");

    std::shared_ptr<Object> f_match_out = add_object(config, "match", "filter");
    set_option(*f_match_out, "match", "OUT=");
    set_option(*f_match_out, "value", "MESSAGE");

    f_iptables->add_object(f_match_in, 0);
    f_iptables->add_object(f_match_out, 1);
  }

  for (const std::vector<std::string>& ids : logs)
  {
    add_object_statements_to_log_statement(config, ids);
  }

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

  return std::shared_ptr<Object>(object);
}

std::shared_ptr<ObjectStatement>& Test::add_object_statement(Config& config, const std::string& object_statement_id)
{
  ObjectStatement* new_object_statement = new ObjectStatement(object_statement_id);
  std::shared_ptr<ObjectStatement> object_statement = config.add_object_statement(new_object_statement);

  object_statements.push_back(std::move(object_statement));

  return object_statements.back();
}

void Test::add_object_statements_to_log_statement(Config& config, const std::vector<std::string>& object_statement_ids)
{
  const Options& options = static_cast<const Options&>(config.get_default_object("log", "options"));
  LogStatement* new_log_statement = new LogStatement(options);

  std::shared_ptr<LogStatement> log_statement = config.add_log_statement(new_log_statement);

  int i = 0;
  for (const std::string& id : object_statement_ids)
  {
    std::shared_ptr<ObjectStatement>& object_statement = find_object_statement(id);
    log_statement->add_object_statement(object_statement, i++);
  }

  log_statements.push_back(std::move(log_statement));
}

std::shared_ptr<ObjectStatement>& Test::find_object_statement(const std::string& id)
{
  for (std::shared_ptr<ObjectStatement>& object_statement : object_statements)
  {
    if (object_statement->get_id() == id)
    {
      return object_statement;
    }
  }
}

QTEST_MAIN(Test)
