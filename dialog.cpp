#include "dialog.h"
#include "driver.h"

#include <QLabel>
#include <QScrollArea>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>

Dialog::Dialog(Driver& driver, QWidget* parent) :
  QDialog(parent),
  driver(driver)
{
  setWindowTitle(QString::fromStdString("Configure " + driver.get_name()));

  QLabel* label = new QLabel;
  label->setText(QString::fromStdString(driver.get_description()));
  label->setWordWrap(true);

  QWidget* widget = create_form();

  QScrollArea* scrollArea = new QScrollArea;
  scrollArea->setWidget(widget);
  scrollArea->setWidgetResizable(true);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setSizeConstraint(QLayout::SetFixedSize);
  mainLayout->addWidget(label);
  mainLayout->addWidget(scrollArea);
  mainLayout->addWidget(buttonBox);
}

Dialog::~Dialog()
{}

void Dialog::accept()
{
  update_values();

  QDialog::accept();
}

QWidget* Dialog::create_form()
{
  QWidget* widget = new QWidget;
  QFormLayout* formLayout = new QFormLayout(widget);

  for (const Option& option : driver.get_options())
  {
    OptionType type = option.get_type();
    const std::string& name = option.get_name();
    const std::string& description = option.get_description();
    const std::string& current_value = option.get_current_value();

    QLabel* label = new QLabel(QString::fromStdString(name));
    label->setToolTip(QString::fromStdString(description));

    switch (type)
    {
      case OptionType::string:
      {
        QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(current_value));

        formLayout->addRow(label, lineEdit);
        break;
      }
      case OptionType::number:
      {
        QSpinBox* spinBox = new QSpinBox;
        spinBox->setRange(0, std::numeric_limits<int>::max());

        if (!current_value.empty())
        {
          spinBox->setValue(std::stoi(current_value));
        }

        formLayout->addRow(label, spinBox);
        break;
      }
      case OptionType::list:
      {
        QComboBox* comboBox = new QComboBox;

        for (const std::string& value : option.get_values())
        {
          comboBox->addItem(QString::fromStdString(value));
        }

        current_value.empty() ? comboBox->setCurrentIndex(-1) : comboBox->setCurrentText(QString::fromStdString(current_value));

        formLayout->addRow(label, comboBox);
        break;
      }
      case OptionType::set:
      {
        for (const std::string& value : option.get_values())
        {
          QCheckBox* checkBox = new QCheckBox(QString::fromStdString(value));

          if (current_value.find(value) != std::string::npos)
          {
            checkBox->setChecked(true);
          }

          formLayout->addRow(label, checkBox);
          label = 0;
        }

        break;
      }
    }
  }

  return widget;
}

void Dialog::update_values()
{
  QFormLayout* formLayout = this->findChild<QFormLayout*>();

  int row = 0;
  for (Option& option : driver.get_options())
  {
    OptionType type = option.get_type();
    switch (type)
    {
      case OptionType::string:
      {
        QLineEdit* lineEdit = static_cast<QLineEdit*>(formLayout->itemAt(row++, QFormLayout::FieldRole)->widget());

        option.set_current_value(lineEdit->text().toStdString());
        break;
      }
      case OptionType::number:
      {
        QSpinBox* spinBox = static_cast<QSpinBox*>(formLayout->itemAt(row++, QFormLayout::FieldRole)->widget());

        option.set_current_value(spinBox->text().toStdString());
        break;
      }
      case OptionType::list:
      {
        QComboBox* comboBox = static_cast<QComboBox*>(formLayout->itemAt(row++, QFormLayout::FieldRole)->widget());

        option.set_current_value(comboBox->currentText().toStdString());
        break;
      }
      case OptionType::set:
      {
        std::string current_value;
        const int n = option.get_values().size();
        for (int i = 0; i < n; i++)
        {
          QCheckBox* checkBox = static_cast<QCheckBox*>(formLayout->itemAt(row++, QFormLayout::FieldRole)->widget());

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
