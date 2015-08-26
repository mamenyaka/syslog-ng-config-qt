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

#include "sources.h"
#include "config.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QtTest/QTest>

void Test::is_config_valid_test_data()
{
  QFile file("sources.conf");
  file.open(QIODevice::ReadOnly | QIODevice::Text);

  QTextStream in(&file);
  QString conf = in.readAll();

  file.close();

  QTest::addColumn<QString>("conf");
  QTest::newRow("sources config") << conf;
}

void Test::is_config_valid_test()
{
  Config config("../../objects");

  std::shared_ptr<ObjectStatement> s_local = add_object_statement(config, "s_local");
  {
    std::shared_ptr<Object> internal = add_object(config, "internal", "source");

    s_local->add_object(internal, 0);
  }

  std::shared_ptr<ObjectStatement> s_file = add_object_statement(config, "s_file");
  {
    std::shared_ptr<Object> file = add_object(config, "file", "source");
    set_option(*file, "file", "/var/log/apache/access.log");
    set_option(*file, "flags", "no-parse");

    s_file->add_object(file, 0);
  }

  std::shared_ptr<ObjectStatement> s_network = add_object_statement(config, "s_network");
  {
    std::shared_ptr<Object> network = add_object(config, "network", "source");
    set_option(*network, "ip", "192.168.1.1");
    set_option(*network, "transport", "udp");

    s_network->add_object(network, 0);
  }

  std::shared_ptr<ObjectStatement> s_network6 = add_object_statement(config, "s_network6");
  {
    std::shared_ptr<Object> network = add_object(config, "network", "source");
    set_option(*network, "ip", "::1");
    set_option(*network, "port", "2222");
    set_option(*network, "ip-protocol", "6");

    s_network6->add_object(network, 0);
  }

  std::shared_ptr<ObjectStatement> apps = add_object_statement(config, "apps");
  {
    std::shared_ptr<Object> nodejs = add_object(config, "nodejs", "source");
    set_option(*nodejs, "localip", "192.168.1.1");
    set_option(*nodejs, "port", "9999");

    apps->add_object(nodejs, 0);
  }

  std::shared_ptr<ObjectStatement> s_pipe = add_object_statement(config, "s_pipe");
  {
    std::shared_ptr<Object> pipe = add_object(config, "pipe", "source");
    set_option(*pipe, "pipe", "/dev/pipe");
    set_option(*pipe, "pad-size", "2048");

    s_pipe->add_object(pipe, 0);
  }

  std::shared_ptr<ObjectStatement> s_pacct = add_object_statement(config, "s_pacct");
  {
    std::shared_ptr<Object> pacct = add_object(config, "pacct", "source");

    s_pacct->add_object(pacct, 0);
  }

  std::shared_ptr<ObjectStatement> s_program = add_object_statement(config, "s_program");
  {
    std::shared_ptr<Object> program = add_object(config, "program", "source");
    set_option(*program, "program", "/etc/init.d/mydaemon");

    s_program->add_object(program, 0);
  }

  std::shared_ptr<ObjectStatement> s_syslog = add_object_statement(config, "s_syslog");
  {
    std::shared_ptr<Object> syslog = add_object(config, "syslog", "source");
    set_option(*syslog, "ip", "10.100.20.40");
    set_option(*syslog, "transport", "tls");

    Options& options = const_cast<Options&>(static_cast<const Options&>(config.get_default_object("tls", "options")));
    set_option(options, "ca-dir", "/opt/syslog-ng/etc/syslog-ng/keys/ca.d/");
    set_option(options, "key-file", "/opt/syslog-ng/etc/syslog-ng/keys/server_privatekey.pem");
    set_option(options, "cert-file", "/opt/syslog-ng/etc/syslog-ng/keys/server_certificate.pem");

    auto it = std::find_if(syslog->get_options().begin(), syslog->get_options().end(),
                           [](std::unique_ptr<Option>& option)->bool {
                             return option->get_name() == "tls";
                           });
    ExternOption* tls = dynamic_cast<ExternOption*>(it->get());
    tls->set_options(options);

    s_syslog->add_object(syslog, 0);
  }

  std::shared_ptr<ObjectStatement> s_journald = add_object_statement(config, "s_journald");
  {
    std::shared_ptr<Object> systemdjournal = add_object(config, "systemd-journal", "source");
    set_option(*systemdjournal, "prefix", ".SDATA.journald.");

    s_journald->add_object(systemdjournal, 0);
  }

  std::shared_ptr<ObjectStatement> s_systemdd = add_object_statement(config, "s_systemdd");
  {
    std::shared_ptr<Object> systemdsyslog = add_object(config, "systemd-syslog", "source");

    s_systemdd->add_object(systemdsyslog, 0);
  }

  std::shared_ptr<ObjectStatement> s_dgram = add_object_statement(config, "s_dgram");
  {
    std::shared_ptr<Object> unixdgram = add_object(config, "unix-dgram", "source");
    set_option(*unixdgram, "unix-dgram", "/var/run/log");

    s_dgram->add_object(unixdgram, 0);
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

QTEST_MAIN(Test)

