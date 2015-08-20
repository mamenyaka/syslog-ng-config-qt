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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
  class Dialog;
}
class Object;

/*
 * Custom QDialog class for setting Object options.
 */
class Dialog : public QDialog
{
  Q_OBJECT

  Ui::Dialog* ui;

  Object& object;

public:
  explicit Dialog(Object& object,
                  QWidget* parent = 0);
  ~Dialog();

public slots:
  int exec();
  void accept();
  void reject();

private:
  /*
   * Fills the dialog with line edits, spinboxes, comboboxes, checkboxes.
   * Called only once in constructor.
   */
  void create_form();

  /*
   * Display the current value of the Object's options.
   * Called at dialog exec and when RestoreDefaults is pressed.
   */
  void set_form_values();

  /*
   * @return: true if all required options are set, false otherwise.
   * Called when OK is pressed.
   */
  bool set_object_options();

  // non copyable
  Dialog(const Dialog&) = delete;
  Dialog& operator=(const Dialog&) = delete;
};

#endif  // DIALOG_H
