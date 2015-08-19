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

#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <vector>
#include <memory>

class QVBoxLayout;
class QGroupBox;
class Options;

enum class OptionType { STRING, NUMBER, LIST, SET, TLS, VALUEPAIRS };

class Option
{
protected:
  std::string name;
  std::string description;
  bool required = false;

public:
  Option(const std::string& name,
         const std::string& description);
  virtual ~Option() {}

  virtual Option* clone() const = 0;

  const std::string& get_name() const;
  const std::string& get_description() const;
  virtual const std::string get_current_value() const = 0;

  bool is_required() const;
  void set_required(bool required);
  virtual bool has_changed() const = 0;

  virtual void set_default(const std::string& default_value) = 0;
  virtual void set_current(const std::string& current_value) = 0;
  virtual void set_previous() = 0;

  virtual void restore_default() = 0;
  virtual void restore_previous() = 0;

  virtual void create_form(QVBoxLayout* vboxLayout) const = 0;
  virtual void set_form_value(QGroupBox* groupBox) const = 0;
  virtual bool set_option(QGroupBox* groupBox) = 0;

  virtual const std::string to_string() const;
};

template<typename Value, class Derived>
class SimpleOption : public Option
{
protected:
  Value default_value;
  Value current_value;
  Value previous_value;

public:
  SimpleOption(const std::string& name,
               const std::string& description);

  Option* clone() const;

  virtual bool has_changed() const;

  virtual void set_previous();

  virtual void restore_default();
  virtual void restore_previous();
};

class StringOption : public SimpleOption<std::string, StringOption>
{
public:
  StringOption(const std::string& name,
               const std::string& description);

  const std::string get_current_value() const;

  void set_default(const std::string& default_value);
  void set_current(const std::string& current_value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);
};

class NumberOption : public SimpleOption<int, NumberOption>
{
public:
  NumberOption(const std::string& name,
               const std::string& description);

  const std::string get_current_value() const;

  void set_default(const std::string& default_value);
  void set_current(const std::string& current_value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);
};


class SelectOption
{
protected:
  std::vector<std::string> values;

public:
  SelectOption();

  void add_value(const std::string& value);
};

class ListOption : public SimpleOption<int, ListOption>, public SelectOption
{
public:
  ListOption(const std::string& name,
             const std::string& description);

  const std::string get_current_value() const;

  void set_default(const std::string& default_value);
  void set_current(const std::string& current_value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);

private:
  int find_index(const std::string& value) const;
};

class SetOption : public SimpleOption<std::string, SetOption>, public SelectOption
{
public:
  SetOption(const std::string& name,
            const std::string& description);

  const std::string get_current_value() const;

  void set_default(const std::string& default_value);
  void set_current(const std::string& current_value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);
};


template<class Derived>
class ExternOption : public Option
{
protected:
  std::unique_ptr<Options> options;

public:
  ExternOption(const std::string& name,
               const std::string& description);
  ExternOption(const ExternOption& other);

  Option* clone() const;

  void set_options(const Options& options)
  {
    this->options = std::make_unique<Options>(options);
  }

  const std::string get_current_value() const;

  bool has_changed() const;

  void set_default(const std::string &) {}
  void set_current(const std::string &) {}
  void set_previous();

  void restore_default();
  void restore_previous();

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox *) const {}
  bool set_option(QGroupBox *) { return true; }

protected:
  virtual const std::string get_type() const = 0;
};

class TLSOption : public ExternOption<TLSOption>
{
public:
  TLSOption(const std::string& name,
            const std::string& description);

private:
  const std::string get_type() const;
};

class ValuePairsOption : public ExternOption<ValuePairsOption>
{
public:
  ValuePairsOption(const std::string& name,
                   const std::string& description);

private:
  const std::string get_type() const;
};

#endif  // OPTION_H
