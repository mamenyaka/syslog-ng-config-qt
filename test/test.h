#ifndef TEST_H
#define TEST_H

#include <QObject>

class Test : public QObject
{
  Q_OBJECT

private slots:
  void is_digit_only_test_data();
  void is_digit_only_test();
};

#endif  // TEST_H
