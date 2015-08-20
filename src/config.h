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

/*
 * Holds the default Objects created from the yaml files
 * and the syslog-ng configuration elements.
 */
class Config
{
  // All the Objects used in the configuration are copied from these
  std::vector< std::unique_ptr<const Object> > default_objects;

  std::unique_ptr<GlobalOptions> global_options;
  std::list< std::unique_ptr<ObjectStatement> > object_statements;
  std::list< std::unique_ptr<LogStatement> > log_statements;

public:
  /*
   * @dir_name: directory holding the yaml files.
   */
  Config(const std::string& dir_name = "objects");
  ~Config();

  const std::vector< std::unique_ptr<const Object> >& get_default_objects() const;

  /*
   * @name, @type: uniquely indentifies an Object.
   * @return: returns a default Object to be copied and stored in an ObjectIcon.
   */
  const Object& get_default_object(const std::string& name, const std::string& type) const;

  Options& get_global_options();
  const std::list< std::unique_ptr<ObjectStatement> >& get_object_statements() const;
  const std::list< std::unique_ptr<LogStatement> >& get_log_statements() const;

  /*
   * The same ObjectStatement can be added to multiple LogStatements.
   * @return: Usually the returned shared_ptr is held by an ObjectStatementIcon.
   */
  std::shared_ptr<ObjectStatement> add_object_statement(ObjectStatement* new_object_statement);

  /*
   * @return: Usually the returned shared_ptr is held by an LogStatementIcon.
   *
   * Should be unique_ptr, but shared_ptr is less code:
   * std::unique_ptr< Log, std::function<void(const LogStatement *)> >
   */
  std::shared_ptr<LogStatement> add_log_statement(LogStatement* new_log_statement);

  /*
   * Read the @file_name yaml file and create a default Object from it.
   * Created default Objects are moved to the @default_objects vector.
   */
  void parse_yaml(const std::string& file_name);

  /*
   * @return: returns a syslog-ng configuration file.
   * The output should go in a file, and used with syslog-ng.
   */
  const std::string to_string() const;

private:
  /*
   * Method for erasing an ObjectStatement from it's container.
   * Used as shared_ptr deleter.
   */
  void delete_object_statement(const ObjectStatement* old_object_statement);

  /*
   * Method for erasing a LogStatement from it's container.
   * Used as shared_ptr deleter.
   */
  void delete_log_statement(const LogStatement* old_log_statement);

  // Set to true in destructor, disables the delete methods
  bool deleted = false;
};

#endif  // CONFIG_H
