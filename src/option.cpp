#include "option.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

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


template<typename Value, typename Derived>
OptionBase<Value, Derived>::OptionBase(const std::string& name,
                              const std::string& description) :
  Option(name, description)
{}

template<typename Value, typename Derived>
Option* OptionBase<Value, Derived>::clone() const
{
  return new Derived(static_cast<const Derived&>(*this));
}

template<typename Value, typename Derived>
bool OptionBase<Value, Derived>::has_changed() const
{
  return current_value != default_value;
}

template<typename Value, typename Derived>
void OptionBase<Value, Derived>::set_default(const Value& default_value)
{
  current_value = this->default_value = default_value;
}

template<typename Value, typename Derived>
void OptionBase<Value, Derived>::restore_default()
{
  current_value = default_value;
}

template<typename Value, typename Derived>
void OptionBase<Value, Derived>::set_previous()
{
  previous_value = current_value;
}

template<typename Value, typename Derived>
void OptionBase<Value, Derived>::restore_previous()
{
  current_value = previous_value;
}


StringOption::StringOption(const std::string& name,
                           const std::string& description) :
  OptionBase(name, description)
{}

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

  return !is_required() || !current_value.empty();
}

const std::string StringOption::to_string() const
{
  return get_name() + "(\"" + current_value + "\")";
}


NumberOption::NumberOption(const std::string& name,
                           const std::string& description) :
  OptionBase(name, description)
{
  current_value = default_value = -1;
}

void NumberOption::set_default(const std::string& default_value)
{
  current_value = this->default_value = std::stoi(default_value);
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

  return !is_required() || current_value != -1;
}

const std::string NumberOption::to_string() const
{
  return get_name() + "(" + std::to_string(current_value) + ")";
}


ListOption::ListOption(const std::string& name,
                       const std::string& description) :
  OptionBase(name, description)
{
  current_value = default_value = -1;
}

void ListOption::add_value(const std::string& value)
{
  values.push_back(std::move(value));
}

void ListOption::set_default(const std::string& default_value)
{
  auto it = std::find_if(values.cbegin(), values.cend(),
                         [&default_value](const std::string& value)->bool {
                           return value == default_value;
                         });

  if (it != values.cend())
  {
    current_value = this->default_value = std::distance(values.cbegin(), it);
  }
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

  return !is_required() || current_value != -1;
}

const std::string ListOption::to_string() const
{
  return get_name() + "(" + values.at(current_value) + ")";
}


SetOption::SetOption(const std::string& name,
                     const std::string& description) :
  OptionBase(name, description)
{}

void SetOption::add_value(const std::string& value)
{
  values.push_back(std::move(value));
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
  QList<QCheckBox*> checkBoxes = groupBox->findChildren<QCheckBox*>();
  for (QCheckBox* checkBox : checkBoxes)
  {
    if (checkBox->isChecked())
    {
      std::string value = checkBox->text().toStdString();
      current_value += (current_value.empty() ? "" : ", ") + value;
    }
  }

  return !is_required() || !current_value.empty();
}

const std::string SetOption::to_string() const
{
  return get_name() + "(\"" + current_value + "\")";
}
