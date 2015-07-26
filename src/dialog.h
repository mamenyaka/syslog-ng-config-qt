#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Driver;
namespace Ui {
  class Dialog;
}

class Dialog : public QDialog
{
  Q_OBJECT

  Driver& driver;

  Ui::Dialog* ui;

public:
  explicit Dialog(Driver& driver,
                  QWidget* parent = 0);
  ~Dialog();

private:
  void setupConnections();

  void create_form();
  void set_form_values();
  bool set_driver_options();
};

#endif  // DIALOG_H
