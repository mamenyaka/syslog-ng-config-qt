#include "dialog.h"
#include "config.h"

#include <QLabel>
#include <QScrollArea>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QCheckBox>

#include <limits>

Dialog::Dialog(Driver& driver, QWidget* parent) :
  QDialog(parent),
  driver(driver)
{
  setWindowTitle(QString::fromStdString("Configure " + driver.print_id()));

  QLabel* label = new QLabel;
  label->setText(QString::fromStdString(driver.get_description()));
  label->setWordWrap(true);

  QWidget* widget = new QWidget;
  QFormLayout* formLayout = new QFormLayout(widget);

  QScrollArea* scrollArea = new QScrollArea;
  scrollArea->setWidget(widget);
  scrollArea->setWidgetResizable(true);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
  connect(buttonBox, &QDialogButtonBox::rejected, [&]() {
    QDialog::reject();
  });
  connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
    set_driver_options();
    QDialog::accept();
  });
  connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::pressed, [&]() {
    driver.restore_defaults();
    set_form_values();
  });

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(label);
  mainLayout->addWidget(scrollArea);
  mainLayout->addWidget(buttonBox);

  create_form();
  set_form_values();
}

Dialog::~Dialog()
{}

void Dialog::create_form()
{
  for (const Option& option : driver.get_default_driver().get_options())
  {
    QGroupBox* groupBox = new QGroupBox(QString::fromStdString(option.get_name()));
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
        spinBox->setRange(-1, std::numeric_limits<int>::max());

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

  for (Option& option : driver.get_options())
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
        current_value.empty() ? spinBox->setValue(-1) : spinBox->setValue(std::stoi(current_value));
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

        auto checkBox_it = checkBoxes.begin();
        for (const std::string& value : option.get_values())
        {
          QCheckBox* checkBox = *checkBox_it++;
          current_value.find(value) == std::string::npos ? checkBox->setChecked(false) : checkBox->setChecked(true);
        }
        break;
      }
    }
  }
}

void Dialog::set_driver_options()
{
  QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
  auto groupBox_it = groupBoxes.begin();

  for (Option& option : driver.get_options())
  {
    QGroupBox* groupBox = *groupBox_it++;
    switch (option.get_type())
    {
      case OptionType::string:
      {
        QLineEdit* lineEdit = groupBox->findChild<QLineEdit*>();
        option.set_current_value(lineEdit->text().toStdString());
        break;
      }
      case OptionType::number:
      {
        QSpinBox* spinBox = groupBox->findChild<QSpinBox*>();
        option.set_current_value(std::to_string(spinBox->value()));
        break;
      }
      case OptionType::list:
      {
        QComboBox* comboBox = groupBox->findChild<QComboBox*>();
        option.set_current_value(comboBox->currentText().toStdString());
        break;
      }
      case OptionType::set:
      {
        QList<QCheckBox*> checkBoxes = groupBox->findChildren<QCheckBox*>();

        std::string current_value;
        auto checkBox_it = checkBoxes.begin();
        for (const std::string& value : option.get_values())
        {
          QCheckBox* checkBox = *checkBox_it++;
          if (checkBox->isChecked())
          {
            current_value += current_value.empty() ? "" : ", ";
            current_value += checkBox->text().toStdString();
          }
        }

        option.set_current_value(current_value);
        break;
      }
    }
  }
}
