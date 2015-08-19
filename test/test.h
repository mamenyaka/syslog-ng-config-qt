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

#ifndef TEST_H
#define TEST_H

#include <QObject>

#include <memory>

class Object;
class ObjectStatement;
class LogStatement;
class Config;

class Test : public QObject
{
  Q_OBJECT

  std::vector< std::shared_ptr<ObjectStatement> > object_statements;
  std::vector< std::shared_ptr<LogStatement> > log_statements;

private slots:
  void is_config_valid_test_data();
  void is_config_valid_test();

private:
  void set_option(Object& object, const std::string& option_name, const std::string& option_value);

  std::shared_ptr<Object> add_object(Config& config, const std::string& object_name, const std::string& object_type);
  std::shared_ptr<ObjectStatement>& add_object_statement(Config& config, const std::string& object_statement_name);

  void add_object_statements_to_log_statement(Config& config, const std::vector<std::string>& object_statement_names);

  std::shared_ptr<ObjectStatement>& find_object_statement(const std::string& name);
};

#endif  // TEST_H
