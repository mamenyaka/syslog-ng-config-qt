#include "dialog.h"
#include "ui_dialog.h"
#include "config.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

#include <limits>

Dialog::Dialog(Driver& driver, QWidget* parent) :
  QDialog(parent),
  driver(driver),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);

  ui->label->setText(QString::fromStdString(driver.get_description()));

  setupConnections();
  create_form();
  set_form_values();
}

Dialog::~Dialog()
{
  delete ui;
}

void Dialog::setupConnections()
{
  connect(ui->buttonBox, &QDialogButtonBox::accepted, [&]() {
    if (set_driver_options())
    {
      QDialog::accept();
    }
  });

  connect(ui->buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton* button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults)
    {
      driver.restore_defaults();
      set_form_values();
    }
  });
}

void Dialog::create_form()
{
  for (const Option& option : driver.get_options())
  {
    const std::string name = (option.is_required() ? "* " : "") + option.get_name();
    QGroupBox* groupBox = new QGroupBox(QString::fromStdString(name));
    groupBox->setToolTip(QString::fromStdString(option.get_description()));

    QVBoxLayout* vboxLayout = new QVBoxLayout(groupBox);

    switch (option.get_type())
    {
      case OptionType::string:
      {
        QLineEdit* lineEdit = new QLineEdit;
        vboxLayout->addWidget(lineEdit);
        break;
      }
      case OptionType::number:
      {
        QSpinBox* spinBox = new QSpinBox;
        const int min = option.get_default_value().empty() ? -1 : 0;
        spinBox->setRange(min, std::numeric_limits<int>::max());
        vboxLayout->addWidget(spinBox);
        break;
      }
      case OptionType::list:
      {
        QComboBox* comboBox = new QComboBox;
        for (const std::string& value : option.get_values())
        {
          comboBox->addItem(QString::fromStdString(value));
        }
        vboxLayout->addWidget(comboBox);
        break;
      }
      case OptionType::set:
      {
        for (const std::string& value : option.get_values())
        {
          QCheckBox* checkBox = new QCheckBox(QString::fromStdString(value));
          vboxLayout->addWidget(checkBox);
        }
        break;
      }
    }

    QFormLayout* formLayout = findChild<QFormLayout*>();
    formLayout->addRow(groupBox);
  }
}

void Dialog::set_form_values()
{
  QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
  auto groupBox_it = groupBoxes.begin();

  for (const Option& option : driver.get_options())
  {
    QGroupBox* groupBox = *groupBox_it++;
    const std::string& current_value = option.get_current_value();

    switch (option.get_type())
    {
      case OptionType::string:
      {
        QLineEdit* lineEdit = groupBox->findChild<QLineEdit*>();
        lineEdit->setText(QString::fromStdString(current_value));
        break;
      }
      case OptionType::number:
      {
        QSpinBox* spinBox = groupBox->findChild<QSpinBox*>();
        const int value = current_value.empty() ? spinBox->minimum() : std::stoi(current_value);
        spinBox->setValue(value);
        break;
      }
      case OptionType::list:
      {
        QComboBox* comboBox = groupBox->findChild<QComboBox*>();
        current_value.empty() ? comboBox->setCurrentIndex(-1) : comboBox->setCurrentText(QString::fromStdString(current_value));
        break;
      }
      case OptionType::set:
      {
        QList<QCheckBox*> checkBoxes = groupBox->findChildren<QCheckBox*>();
        for (QCheckBox* checkBox : checkBoxes)
        {
          std::string value = checkBox->text().toStdString();
          current_value.find(value) == std::string::npos ? checkBox->setChecked(false) : checkBox->setChecked(true);
        }
        break;
      }
    }
  }
}

bool Dialog::set_driver_options()
{
  QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
  auto groupBox_it = groupBoxes.begin();

  for (Option& option : driver.get_options())
  {
    QGroupBox* groupBox = *groupBox_it++;

    std::string current_value;
    switch (option.get_type())
    {
      case OptionType::string:
      {
        QLineEdit* lineEdit = groupBox->findChild<QLineEdit*>();
        current_value = lineEdit->text().toStdString();
        break;
      }
      case OptionType::number:
      {
        QSpinBox* spinBox = groupBox->findChild<QSpinBox*>();
        current_value = spinBox->value() == -1 ? "" : std::to_string(spinBox->value());
        break;
      }
      case OptionType::list:
      {
        QComboBox* comboBox = groupBox->findChild<QComboBox*>();
        current_value = comboBox->currentText().toStdString();
        break;
      }
      case OptionType::set:
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
        break;
      }
    }

    if (option.is_required() && current_value.empty())
    {
      ui->scrollArea->ensureWidgetVisible(groupBox);
      groupBox->setFocus();
      return false;
    }

    option.set_current_value(current_value);
  }

  return true;
}
