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

#include "option.h"
#include "object.h"
#include "dialog.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

#include <limits>

Option::Option(const std::string& name,
               const std::string& description) :
  name(name),
  description(description)
{}

const std::string& Option::get_name() const
{
  return name;
}

const std::string& Option::get_description() const
{
  return description;
}

bool Option::is_required() const
{
  return required;
}

void Option::set_required(bool required)
{
  this->required = required;
}

const std::string Option::to_string() const
{
  return name + "(" + get_current_value() + ")";
}


template<typename Value, class Derived>
SimpleOption<Value, Derived>::SimpleOption(const std::string& name,
                                           const std::string& description) :
  Option(name, description)
{}

template<typename Value, class Derived>
Option* SimpleOption<Value, Derived>::clone() const
{
  return new Derived(static_cast<const Derived&>(*this));
}

template<typename Value, class Derived>
bool SimpleOption<Value, Derived>::has_changed() const
{
  return required || current_value != default_value;
}

template<typename Value, class Derived>
void SimpleOption<Value, Derived>::set_previous()
{
  previous_value = current_value;
}

template<typename Value, class Derived>
void SimpleOption<Value, Derived>::restore_default()
{
  current_value = default_value;
}

template<typename Value, class Derived>
void SimpleOption<Value, Derived>::restore_previous()
{
  current_value = previous_value;
}


StringOption::StringOption(const std::string& name,
                           const std::string& description) :
  SimpleOption(name, description)
{}

const std::string StringOption::get_current_value() const
{
  // should be NumberOption, but spinbox wouldn't be suitable
  if (name == "perm" || name == "dir-perm")
  {
    return current_value;
  }

  return "\"" + current_value + "\"";
}

void StringOption::set_default(const std::string& default_value)
{
  this->default_value = default_value;
  set_current(default_value);
}

void StringOption::set_current(const std::string& current_value)
{
  this->current_value = current_value;
  set_previous();
}

void StringOption::create_form(QVBoxLayout* vboxLayout) const
{
  QLineEdit* lineEdit = new QLineEdit;
  vboxLayout->addWidget(lineEdit);
}

void StringOption::set_form_value(QGroupBox* groupBox) const
{
  QLineEdit* lineEdit = groupBox->findChild<QLineEdit*>();
  lineEdit->setText(QString::fromStdString(current_value));
}

bool StringOption::set_option(QGroupBox* groupBox)
{
  QLineEdit* lineEdit = groupBox->findChild<QLineEdit*>();
  current_value = lineEdit->text().toStdString();

  return !(is_required() && current_value.empty());
}


NumberOption::NumberOption(const std::string& name,
                           const std::string& description) :
  SimpleOption(name, description)
{
  previous_value = current_value = default_value = -1;
}

const std::string NumberOption::get_current_value() const
{
  return std::to_string(current_value);
}

void NumberOption::set_default(const std::string& default_value)
{
  this->default_value = std::stoi(default_value);
  set_current(default_value);
}

void NumberOption::set_current(const std::string& current_value)
{
  this->current_value = std::stoi(current_value);
  set_previous();
}

void NumberOption::create_form(QVBoxLayout* vboxLayout) const
{
  QSpinBox* spinBox = new QSpinBox;
  spinBox->setRange(-1, std::numeric_limits<int>::max());
  spinBox->setSpecialValueText(" ");
  vboxLayout->addWidget(spinBox);
}

void NumberOption::set_form_value(QGroupBox* groupBox) const
{
  QSpinBox* spinBox = groupBox->findChild<QSpinBox*>();
  spinBox->setValue(current_value);
}

bool NumberOption::set_option(QGroupBox* groupBox)
{
  QSpinBox* spinBox = groupBox->findChild<QSpinBox*>();
  current_value = spinBox->value();

  return !(is_required() && current_value == -1);
}


SelectOption::SelectOption()
{
  values.reserve(10);
}

void SelectOption::add_value(const std::string& value)
{
  values.push_back(std::move(value));
}


ListOption::ListOption(const std::string& name,
                       const std::string& description) :
  SimpleOption(name, description)
{
  previous_value = current_value = default_value = -1;
}

const std::string ListOption::get_current_value() const
{
  return values.at(current_value);
}

void ListOption::set_default(const std::string& default_value)
{
  this->default_value = find_value(default_value);
  set_current(default_value);
}

void ListOption::set_current(const std::string& current_value)
{
  this->current_value = find_value(current_value);
  set_previous();
}

void ListOption::create_form(QVBoxLayout* vboxLayout) const
{
  QComboBox* comboBox = new QComboBox;
  for (const std::string& value : values)
  {
    comboBox->addItem(QString::fromStdString(value));
  }
  vboxLayout->addWidget(comboBox);
}

void ListOption::set_form_value(QGroupBox* groupBox) const
{
  QComboBox* comboBox = groupBox->findChild<QComboBox*>();
  comboBox->setCurrentIndex(current_value);
}

bool ListOption::set_option(QGroupBox* groupBox)
{
  QComboBox* comboBox = groupBox->findChild<QComboBox*>();
  current_value = comboBox->currentIndex();

  return !(is_required() && current_value == -1);
}

int ListOption::find_value(const std::string& value) const
{
  auto it = std::find_if(values.cbegin(), values.cend(),
                         [&value](const std::string& v)->bool {
                           return v == value;
                         });

  if (it != values.cend())
  {
    return std::distance(values.cbegin(), it);
  }
  else
  {
    return -1;
  }
}


SetOption::SetOption(const std::string& name,
                     const std::string& description) :
  SimpleOption(name, description)
{}

const std::string SetOption::get_current_value() const
{
  return current_value;
}

void SetOption::set_default(const std::string& default_value)
{
  this->default_value = default_value;
  set_current(default_value);
}

void SetOption::set_current(const std::string& current_value)
{
  this->current_value = current_value;
  set_previous();
}

void SetOption::create_form(QVBoxLayout* vboxLayout) const
{
  for (const std::string& value : values)
  {
    QCheckBox* checkBox = new QCheckBox(QString::fromStdString(value));
    vboxLayout->addWidget(checkBox);
  }
}

void SetOption::set_form_value(QGroupBox* groupBox) const
{
  QList<QCheckBox*> checkBoxes = groupBox->findChildren<QCheckBox*>();
  for (QCheckBox* checkBox : checkBoxes)
  {
    std::string value = checkBox->text().toStdString();
    current_value.find(value) == std::string::npos ? checkBox->setChecked(false) : checkBox->setChecked(true);
  }
}

bool SetOption::set_option(QGroupBox* groupBox)
{
  current_value.clear();

  // quirks
  std::string sep = (name == "scope" ? " " : ", ");

  QList<QCheckBox*> checkBoxes = groupBox->findChildren<QCheckBox*>();
  for (QCheckBox* checkBox : checkBoxes)
  {
    if (checkBox->isChecked())
    {
      std::string value = checkBox->text().toStdString();
      current_value += (current_value.empty() ? "" : sep) + value;
    }
  }

  return !(is_required() && current_value.empty());
}


template<class Derived>
ExternOption<Derived>::ExternOption(const std::string& name,
                                    const std::string& description) :
  Option(name, description)
{}

template<class Derived>
ExternOption<Derived>::ExternOption(const ExternOption& other) :
  Option(other)
{
  if (other.options.get())
  {
    options = std::make_unique<Options>(*other.options);
  }
}

template<class Derived>
Option* ExternOption<Derived>::clone() const
{
  return new Derived(static_cast<const Derived&>(*this));
}

template<class Derived>
const std::string ExternOption<Derived>::get_current_value() const
{
  std::string config;

  for (const std::unique_ptr<Option>& option : options->get_options())
  {
    if (!option->has_changed())
    {
      continue;
    }

    config += "\n            " + option->to_string();
  }

  config += "\n        ";

  return config;
}

template<class Derived>
bool ExternOption<Derived>::has_changed() const
{
  for (const std::unique_ptr<Option>& option : options->get_options())
  {
    if (option->has_changed())
    {
      return true;
    }
  }

  return false;
}

template<class Derived>
void ExternOption<Derived>::set_previous()
{
  for (std::unique_ptr<Option>& option : options->get_options())
  {
    option->set_previous();
  }
}

template<class Derived>
void ExternOption<Derived>::restore_default()
{
  for (std::unique_ptr<Option>& option : options->get_options())
  {
    option->restore_default();
  }
}

template<class Derived>
void ExternOption<Derived>::restore_previous()
{
  for (std::unique_ptr<Option>& option : options->get_options())
  {
    option->restore_previous();
  }
}

template<class Derived>
void ExternOption<Derived>::create_form(QVBoxLayout* vboxLayout) const
{
  QPushButton* button = new QPushButton(QString::fromStdString("set " + get_type() + " options"));
  vboxLayout->addWidget(button);

  Dialog* dialog = new Dialog(*options, vboxLayout->parentWidget());
  QObject::connect(button, &QPushButton::clicked, dialog, &Dialog::exec);
}


TLSOption::TLSOption(const std::string& name,
                     const std::string& description) :
  ExternOption(name, description)
{}

const std::string TLSOption::get_type() const
{
  return "TLS";
}


ValuePairsOption::ValuePairsOption(const std::string& name,
                                   const std::string& description) :
  ExternOption(name, description)
{}

const std::string ValuePairsOption::get_type() const
{
  return "value-pairs";
}
