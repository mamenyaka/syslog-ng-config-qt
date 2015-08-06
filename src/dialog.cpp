#include "dialog.h"
#include "ui_dialog.h"
#include "driver.h"

#include <QGroupBox>
#include <QAbstractButton>

Dialog::Dialog(Driver& driver, QWidget* parent) :
  QDialog(parent),
  driver(driver),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);

  ui->label->setText(QString::fromStdString(driver.get_description()));

  create_form();

  connect(ui->buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton* button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults)
    {
      driver.restore_default_values();
      set_form_values();
    }
  });
}

Dialog::~Dialog()
{
  delete ui;
}

int Dialog::exec()
{
  set_form_values();
  return QDialog::exec();
}

void Dialog::accept()
{
  if (set_driver_options())
  {
    driver.set_previous_values();
    QDialog::accept();
  }
}

void Dialog::reject()
{
  driver.restore_previous_values();
  QDialog::reject();
}

void Dialog::create_form()
{
  QFormLayout* formLayout = findChild<QFormLayout*>();

  for (const std::unique_ptr<Option>& option : driver.get_options())
  {
    const std::string name = (option->is_required() ? "* " : "") + option->get_name();
    QGroupBox* groupBox = new QGroupBox(QString::fromStdString(name));
    groupBox->setToolTip(QString::fromStdString(option->get_description()));

    QVBoxLayout* vboxLayout = new QVBoxLayout(groupBox);
    option->create_form(vboxLayout);

    formLayout->addRow(groupBox);
  }
}

void Dialog::set_form_values()
{
  QWidget* parent = findChild<QWidget*>("formWidget");
  QList<QGroupBox*> groupBoxes = parent->findChildren<QGroupBox*>(QString(), Qt::FindChildOption::FindDirectChildrenOnly);
  auto it = groupBoxes.begin();

  for (const std::unique_ptr<Option>& option : driver.get_options())
  {
    QGroupBox* groupBox = *it++;
    option->set_form_value(groupBox);
  }
}

bool Dialog::set_driver_options()
{
  QWidget* parent = findChild<QWidget*>("formWidget");
  QList<QGroupBox*> groupBoxes = parent->findChildren<QGroupBox*>(QString(), Qt::FindChildOption::FindDirectChildrenOnly);
  auto it = groupBoxes.begin();

  for (std::unique_ptr<Option>& option : driver.get_options())
  {
    QGroupBox* groupBox = *it++;

    const bool valid = option->set_option(groupBox);

    if (!valid)
    {
      ui->scrollArea->ensureWidgetVisible(groupBox);
      groupBox->setFocus();
      return false;
    }
  }

  return true;
}
