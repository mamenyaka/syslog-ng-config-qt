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

public slots:
  void accept();

private:
  QWidget* create_form();
  void update_values();
};

#endif  // DIALOG_H
