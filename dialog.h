#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

class Driver;

class Dialog : public QDialog
{
  Q_OBJECT

  Driver& driver;

public:
  explicit Dialog(Driver& driver, QWidget* parent = 0);
  ~Dialog();

private:
  void create_form();
  void set_form_values();
  bool set_driver_options();
};

#endif  // DIALOG_H
