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

#include "dialog.h"
#include "ui_dialog.h"
#include "object.h"

#include <QGroupBox>
#include <QAbstractButton>

Dialog::Dialog(Object& object, QWidget* parent) :
  QDialog(parent),
  ui(new Ui::Dialog),
  object(object)
{
  ui->setupUi(this);

  ui->label->setText(QString::fromStdString(object.get_description()));

  setWindowTitle("Configure " + QString::fromStdString(object.get_type()));

  create_form();

  connect(ui->buttonBox, &QDialogButtonBox::clicked, [&](QAbstractButton* button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults)
    {
      for (std::unique_ptr<Option>& option : object.get_options())
      {
        option->restore_default();
      }

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
  if (set_object_options())  // dialog remains open if there are empty required options
  {
    for (std::unique_ptr<Option>& option : object.get_options())
    {
      option->set_previous();
    }

    QDialog::accept();
  }
}

void Dialog::reject()
{
  for (std::unique_ptr<Option>& option : object.get_options())
  {
    option->restore_previous();
  }

  QDialog::reject();
}

void Dialog::create_form()
{
  QFormLayout* formLayout = findChild<QFormLayout*>();

  for (const std::unique_ptr<Option>& option : object.get_options())
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
  QList<QGroupBox*> groupBoxes = parent->findChildren<QGroupBox*>(QString(), Qt::FindDirectChildrenOnly);
  auto it = groupBoxes.begin();

  for (const std::unique_ptr<Option>& option : object.get_options())
  {
    QGroupBox* groupBox = *it++;
    option->set_form_value(groupBox);
  }
}

bool Dialog::set_object_options()
{
  QWidget* parent = findChild<QWidget*>("formWidget");
  QList<QGroupBox*> groupBoxes = parent->findChildren<QGroupBox*>(QString(), Qt::FindDirectChildrenOnly);
  auto it = groupBoxes.begin();

  for (std::unique_ptr<Option>& option : object.get_options())
  {
    QGroupBox* groupBox = *it++;
    bool valid = option->set_option(groupBox);

    if (!valid)
    {
      ui->scrollArea->ensureWidgetVisible(groupBox);
      groupBox->setFocus();
      return false;
    }
  }

  return true;
}
