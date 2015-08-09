#ifndef OPTION_H
#define OPTION_H

#include <string>
#include <vector>
#include <memory>

class QVBoxLayout;
class QGroupBox;
class Options;

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

  bool is_required() const;
  void set_required(bool required);

  virtual const std::string get_current_value() const = 0;

  virtual bool has_changed() const = 0;

  virtual void set_default(const std::string& default_value) = 0;
  virtual void set_current(const std::string& default_value) = 0;
  virtual void set_previous() = 0;

  virtual void restore_default() = 0;
  virtual void restore_previous() = 0;

  virtual void create_form(QVBoxLayout* vboxLayout) const = 0;
  virtual void set_form_value(QGroupBox* groupBox) const = 0;
  virtual bool set_option(QGroupBox* groupBox) = 0;

  virtual const std::string to_string() const;
};

template<typename Value, class Derived>
class OptionBase : public Option
{
protected:
  Value default_value;
  Value current_value;
  Value previous_value;

public:
  OptionBase(const std::string& name,
             const std::string& description);

  Option* clone() const;

  virtual bool has_changed() const;

  virtual void set_previous();

  virtual void restore_default();
  virtual void restore_previous();
};

class StringOption : public OptionBase<std::string, StringOption>
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

class NumberOption : public OptionBase<int, NumberOption>
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

class ListOption : public OptionBase<int, ListOption>
{
  std::vector<std::string> values;

public:
  ListOption(const std::string& name,
             const std::string& description);

  const std::string get_current_value() const;

  void set_default(const std::string& default_value);
  void set_current(const std::string& current_value);

  void add_value(const std::string& value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);

private:
  int find_index(const std::string& value) const;
};

class SetOption : public OptionBase<std::string, SetOption>
{
  std::vector<std::string> values;

public:
  SetOption(const std::string& name,
            const std::string& description);

  const std::string get_current_value() const;

  void set_default(const std::string& default_value);
  void set_current(const std::string& current_value);

  void add_value(const std::string& value);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox* groupBox) const;
  bool set_option(QGroupBox* groupBox);
};

class ExternOption : public OptionBase<bool, ExternOption>
{
  std::unique_ptr<Options> options;

public:
  ExternOption(const std::string& name,
               const std::string& description);
  ExternOption(const ExternOption& other);

  const std::string get_current_value() const;

  bool has_changed() const;

  void set_default(const std::string &) {}
  void set_current(const std::string &) {}
  void set_previous();

  void restore_default();
  void restore_previous();

  void set_options(const Options& options);

  void create_form(QVBoxLayout* vboxLayout) const;
  void set_form_value(QGroupBox *) const {}
  bool set_option(QGroupBox *) { return true; }
};

#endif  // OPTION_H
