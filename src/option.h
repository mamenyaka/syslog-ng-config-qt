#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <vector>

class QVBoxLayout;
class QGroupBox;

class Option
{
  std::string name;
  std::string description;
  bool required = false;

public:
  Option(const std::string& name,
         const std::string& description);

  const std::string& get_name() const;
  const std::string& get_description() const;

  bool is_required() const;
  void set_required(bool required);

  virtual bool has_changed() const = 0;

  virtual void set_default(const std::string& default_value) = 0;
  virtual void restore_default() = 0;

  virtual void set_previous() = 0;
  virtual void restore_previous() = 0;

  virtual Option* clone() const = 0;

  virtual void create_form(QVBoxLayout* vboxLayout) const = 0;
  virtual void set_form_value(QGroupBox* groupBox) const = 0;
  virtual bool set_option(QGroupBox* groupBox) = 0;

  virtual const std::string to_string() const = 0;
};

template<typename Value>
class OptionBase : public Option
{
public:
  OptionBase(const std::string& name,
             const std::string& description);

  virtual bool has_changed() const;

  virtual void set_default(const Value& default_value);
  virtual void restore_default();

  virtual void set_previous();
  virtual void restore_previous();

protected:
  Value default_value;
  Value current_value;
  Value previous_value;
};

class StringOption : public OptionBase<std::string>
{
public:
  StringOption(const std::string& name, const std::string& description);
  StringOption* clone() const;

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);

  const std::string to_string() const;
};

class NumberOption : public OptionBase<int>
{
public:
  NumberOption(const std::string& name, const std::string& description);
  NumberOption* clone() const;

  void set_default(const std::string& default_value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);

  const std::string to_string() const;
};

class ListOption : public OptionBase<int>
{
  std::vector<std::string> values;

public:
  ListOption(const std::string& name, const std::string& description);
  ListOption* clone() const;

  void add_value(const std::string& value);
  void set_default(const std::string& default_value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);

  const std::string to_string() const;
};

class SetOption : public OptionBase<std::string>
{
  std::vector<std::string> values;

public:
  SetOption(const std::string& name, const std::string& description);
  SetOption* clone() const;

  void add_value(const std::string& value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);

  const std::string to_string() const;
};

#endif  // OPTION_H
