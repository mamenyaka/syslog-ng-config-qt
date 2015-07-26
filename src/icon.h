#ifndef ICON_H
#define ICON_H

#include <QWidget>
#include <QPixmap>

#include <memory>

class Driver;
class Log;

typedef std::unique_ptr< Log, std::function<void(const Log *)> > LogUPtr;

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

  LogUPtr log;

public:
  explicit LogIcon(LogUPtr& log,
                   QWidget* parent = 0);

  Log& get_log();

  void add_driver(DriverIcon& icon);
  void remove_driver(DriverIcon& icon);

protected:
  bool eventFilter(QObject *, QEvent* event);
};

#endif  // ICON_H
