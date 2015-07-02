#include "test.h"
#include "../config.h"

#include <QString>
#include <QtTest/QTest>

void Test::is_digit_only_test_data()
{
  QTest::addColumn<QString>("string");
  QTest::addColumn<bool>("result");

  QTest::newRow("digits only") << "12345" << true;
  QTest::newRow("mixed") << "He11o" << false;
  QTest::newRow("no digits") << "Hello" << false;
}

void Test::is_digit_only_test()
{
  QFETCH(QString, string);
  QFETCH(bool, result);

  QCOMPARE(Option::is_digit_only(string.toStdString()), result);
}

QTEST_MAIN(Test)
