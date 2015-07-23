#ifndef ICON_H
#define ICON_H

#include <QWidget>
#include <QPixmap>

#include <memory>
#include <functional>

class Driver;
class Log;
class QVBoxLayout;

class DriverIcon : public QWidget
{
  Q_OBJECT

  std::shared_ptr<Driver> driver;
  QPixmap pixmap;

public:
  explicit DriverIcon(std::shared_ptr<Driver>& driver,
                      QWidget* parent = 0);

  Driver& get_driver();
  std::shared_ptr<Driver>& get_driver_ptr();
  const QPixmap& get_pixmap() const;

protected:
  void paintEvent(QPaintEvent *);

private:
  void setupIcon();
};

class LogIcon : public QWidget
{
  Q_OBJECT

  std::unique_ptr< Log, std::function<void(const Log *)> > log;
  QVBoxLayout* frameLayout;

public:
  explicit LogIcon(std::unique_ptr< Log, std::function<void(const Log *)> >& log,
                   QWidget* parent = 0);

  void add_driver(DriverIcon& icon);
  void remove_driver(DriverIcon& icon);

protected:
  bool eventFilter(QObject *, QEvent* event);
};

#endif  // ICON_H
