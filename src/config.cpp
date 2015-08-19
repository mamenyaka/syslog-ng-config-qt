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

#include "config.h"

#include <QDirIterator>

#include <yaml-cpp/yaml.h>
#include <map>

template <class T, class U>
struct Typelist
{
  typedef T Head;
  typedef U Tail;
};

class NullType {};

typedef Typelist<Source,
  Typelist<Destination,
  Typelist<Filter,
  Typelist<Template,
  Typelist<Rewrite,
  Typelist<Parser,
  Typelist<Options, NullType> > > > > > > ObjectTypes;

typedef Typelist<StringOption,
  Typelist<NumberOption,
  Typelist<ListOption,
  Typelist<SetOption,
  Typelist<TLSOption,
  Typelist<ValuePairsOption, NullType> > > > > > OptionTypes;

template<class TList, unsigned int index>
struct TypeAt;

template <class Head, class Tail>
struct TypeAt<Typelist<Head, Tail>, 0>
{
  typedef Head Result;
};

template<class Head, class Tail, unsigned int index>
struct TypeAt<Typelist<Head, Tail>, index>
{
  typedef typename TypeAt<Tail, index - 1>::Result Result;
};

template<ObjectType type>
Object* create_object(const std::string& name, const std::string& description)
{
  return new typename TypeAt<ObjectTypes, (int) type>::Result(name, description);
}

template<OptionType type>
Option* create_option(const std::string& name, const std::string& description)
{
  return new typename TypeAt<OptionTypes, (int) type>::Result(name, description);
}


Config::Config(const std::string& dir_name)
{
  default_objects.reserve(60);

  QDirIterator it(QString::fromStdString(dir_name));
  while (it.hasNext())
  {
    it.next();
    if (!it.fileInfo().isFile())
    {
      continue;
    }

    parse_yaml(it.filePath().toStdString());
  }

  std::sort(default_objects.begin(), default_objects.end(),
            [](std::unique_ptr<const Object>& a, std::unique_ptr<const Object>& b)->bool {
              return a->get_name() < b->get_name();
            });

  const Options& options_global = static_cast<const Options&>(get_default_object("global", "options"));
  const Options& options_tls = static_cast<const Options&>(get_default_object("tls", "options"));
  const Options& options_value_pairs = static_cast<const Options&>(get_default_object("value-pairs", "options"));

  global_options = std::make_unique<GlobalOptions>(options_global);

  for (std::unique_ptr<const Object>& object : default_objects)
  {
    for (const std::unique_ptr<Option>& option : object->get_options())
    {
      TLSOption* tls_option = dynamic_cast<TLSOption*>(option.get());
      if (tls_option)
      {
        tls_option->set_options(options_tls);
        continue;
      }

      ValuePairsOption* value_pairs_option = dynamic_cast<ValuePairsOption*>(option.get());
      if (value_pairs_option)
      {
        value_pairs_option->set_options(options_value_pairs);
        continue;
      }
    }
  }
}

Config::~Config()
{
  deleted = true;
}

const std::vector< std::unique_ptr<const Object> >& Config::get_default_objects() const
{
  return default_objects;
}

const Object& Config::get_default_object(const std::string& name, const std::string& type) const
{
  auto it = std::find_if(default_objects.cbegin(), default_objects.cend(),
                         [&name, &type](const std::unique_ptr<const Object>& object)->bool {
                           return object->get_name() == name && object->get_type() == type;
                         });
  return **it;
}

Options& Config::get_global_options()
{
  return global_options->get_options();
}

const std::list< std::unique_ptr<ObjectStatement> >& Config::get_object_statements() const
{
  return object_statements;
}

const std::list< std::unique_ptr<LogStatement> >& Config::get_log_statements() const
{
  return log_statements;
}

std::shared_ptr<ObjectStatement> Config::add_object_statement(ObjectStatement* new_object_statement)
{
  object_statements.emplace_back(new_object_statement);

  std::unique_ptr<ObjectStatement>& object_statement = object_statements.back();

  return std::shared_ptr<ObjectStatement>(object_statement.get(),
                                          [&](const ObjectStatement* object_statement) {
                                            delete_object_statement(object_statement);
                                          });
}

std::shared_ptr<LogStatement> Config::add_log_statement(LogStatement* new_log_statement)
{
  log_statements.emplace_back(new_log_statement);

  std::unique_ptr<LogStatement>& log_statement = log_statements.back();

  return std::shared_ptr<LogStatement>(log_statement.get(),
                                       [&](const LogStatement* log_statement) {
                                         delete_log_statement(log_statement);
                                       });
}

void Config::delete_object_statement(const ObjectStatement* old_object_statement)
{
  if (deleted)
  {
    return;
  }

  object_statements.remove_if([old_object_statement](const std::unique_ptr<ObjectStatement>& object_statement)->bool {
    return object_statement.get() == old_object_statement;
  });
}

void Config::delete_log_statement(const LogStatement* old_log_statement)
{
  if (deleted)
  {
    return;
  }

  log_statements.remove_if([old_log_statement](const std::unique_ptr<LogStatement>& log_statement)->bool {
    return log_statement.get() == old_log_statement;
  });
}

void Config::parse_yaml(const std::string& file_name)
{
  const YAML::Node yaml_object = YAML::LoadFile(file_name);

  const std::string name = yaml_object["name"].as<std::string>();
  const std::string type = yaml_object["type"].as<std::string>();
  const std::string description = yaml_object["description"].as<std::string>();

  std::map<std::string, void*> create_object_map {
    {"source", create_object<ObjectType::SOURCE>(name, description)},
    {"destination", create_object<ObjectType::DESTINATION>(name, description)},
    {"filter", create_object<ObjectType::FILTER>(name, description)},
    {"template", create_object<ObjectType::TEMPLATE>(name, description)},
    {"rewrite", create_object<ObjectType::REWRITE>(name, description)},
    {"parser", create_object<ObjectType::PARSER>(name, description)},
    {"options", create_object<ObjectType::OPTIONS>(name, description)}
  };

  Object* object = static_cast<Object*>(create_object_map[type]);

  const YAML::Node& options = yaml_object["options"];
  for (YAML::const_iterator option_it = options.begin(); option_it != options.end(); ++option_it)
  {
    YAML::const_iterator tmp = option_it->begin();
    const YAML::Node& yaml_option = tmp->second;

    const std::string name = tmp->first.as<std::string>();
    const std::string type = yaml_option["type"].as<std::string>();
    const std::string description = yaml_option["description"].as<std::string>();

    std::map<std::string, void*> create_option_map {
      {"string", create_option<OptionType::STRING>(name, description)},
      {"number", create_option<OptionType::NUMBER>(name, description)},
      {"list", create_option<OptionType::LIST>(name, description)},
      {"set", create_option<OptionType::SET>(name, description)},
      {"tls", create_option<OptionType::TLS>(name, description)},
      {"value-pairs", create_option<OptionType::VALUEPAIRS>(name, description)}
    };

    Option* option = static_cast<Option*>(create_option_map[type]);

    const YAML::Node& values = yaml_option["values"];
    for (YAML::const_iterator value_it = values.begin(); value_it != values.end(); ++value_it)
    {
      const std::string value = value_it->as<std::string>();
      dynamic_cast<SelectOption*>(option)->add_value(value);
    }

    if (yaml_option["default"])
    {
      option->set_default(yaml_option["default"].as<std::string>());
    }

    if (yaml_option["required"])
    {
      option->set_required(yaml_option["required"].as<bool>());
    }

    object->add_option(option);
  }

  default_objects.emplace_back(object);
}

const std::string Config::to_string() const
{
  std::string config;

  config += "@version: 3.7\n";
  config += "@include \"scl.conf\"\n\n";

  config += global_options->to_string();

  for (const std::unique_ptr<ObjectStatement>& object_statement : object_statements)
  {
    config += object_statement->to_string();
  }

  for (const std::unique_ptr<LogStatement>& log_statement : log_statements)
  {
    config += log_statement->to_string();
  }

  return config;
}
