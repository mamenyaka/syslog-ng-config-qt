#ifndef ICON_H
#define ICON_H

#include <QWidget>
#include <QPixmap>

class Driver;
class Log;
class QVBoxLayout;

class DriverIcon : public QWidget
{
  Q_OBJECT

  Driver& driver;
  QPixmap pixmap;

public:
  explicit DriverIcon(Driver& driver,
                      QWidget* parent = 0);

  Driver& get_driver();
  const QPixmap& get_pixmap() const;

private:
  void setupIcon();
};

class LogIcon : public QWidget
{
  Q_OBJECT

  Log& log;
  QVBoxLayout* frameLayout;

public:
  explicit LogIcon(Log& log,
                   QWidget* parent = 0);

  Log& get_log();

  void add_driver(DriverIcon& icon);
  void remove_driver(DriverIcon& icon);

protected:
  bool eventFilter(QObject *, QEvent* event);
};

#endif  // ICON_H
