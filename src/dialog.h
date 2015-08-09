#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
  class Dialog;
}
class Object;

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
  void create_form();
  void set_form_values();
  bool set_object_options();

  // non copyable
  Dialog(const Dialog&) = delete;
  Dialog& operator=(const Dialog&) = delete;
};

#endif  // DIALOG_H
