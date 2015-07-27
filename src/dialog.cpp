#include "dialog.h"
#include "ui_dialog.h"
#include "config.h"

#include <QGroupBox>
#include <QAbstractButton>

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
      driver.set_previous_values();
      QDialog::accept();
    }
  });

  connect(ui->buttonBox, &QDialogButtonBox::rejected, [&]() {
    driver.restore_previous_values();
    QDialog::reject();
  });

  connect(ui->buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton* button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults)
    {
      driver.restore_default_values();
      set_form_values();
    }
  });
}

void Dialog::create_form()
{
  QFormLayout* formLayout = findChild<QFormLayout*>();

  for (const auto& option : driver.get_options())
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
  QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
  auto groupBox_it = groupBoxes.begin();

  for (const auto& option : driver.get_options())
  {
    QGroupBox* groupBox = *groupBox_it++;
    option->set_form_value(groupBox);
  }
}

bool Dialog::set_driver_options()
{
  QList<QGroupBox*> groupBoxes = findChildren<QGroupBox*>();
  auto groupBox_it = groupBoxes.begin();

  for (auto& option : driver.get_options())
  {
    QGroupBox* groupBox = *groupBox_it++;

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
