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

#ifndef CONFIG_H
#define CONFIG_H

#include "object.h"

class Config
{
  std::vector< std::unique_ptr<const Object> > default_objects;

  std::unique_ptr<GlobalOptions> global_options;
  std::list< std::unique_ptr<ObjectStatement> > object_statements;
  std::list< std::unique_ptr<LogStatement> > log_statements;

public:
  Config(const std::string& dir_name = "objects");
  ~Config();

  const std::vector< std::unique_ptr<const Object> >& get_default_objects() const;
  const Object& get_default_object(const std::string& name, const std::string& type) const;

  Options& get_global_options();
  const std::list< std::unique_ptr<ObjectStatement> >& get_object_statements() const;
  const std::list< std::unique_ptr<LogStatement> >& get_log_statements() const;

  std::shared_ptr<ObjectStatement> add_object_statement(ObjectStatement* new_object_statement);
  std::shared_ptr<LogStatement> add_log_statement(LogStatement* new_log_statement);

  void parse_yaml(const std::string& file_name);

  const std::string to_string() const;

private:
  void delete_object_statement(const ObjectStatement* old_object_statement);
  void delete_log_statement(const LogStatement* old_log_statement);

  bool deleted = false;
};

#endif  // CONFIG_H
