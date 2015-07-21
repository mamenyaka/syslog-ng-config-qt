#ifndef DRIVERS_H
#define DRIVERS_H

#include <QWidget>

enum class DriverType : int;
class Driver;
class DriverIcon;

class Drivers : public QWidget
{
  Q_OBJECT

public:
  explicit Drivers(QWidget* parent = 0);

  void setupDrivers(DriverType type, const std::vector<Driver>& default_drivers);

protected:
  void mousePressEvent(QMouseEvent *);

private:
  DriverIcon* select_nearest_driver() const;
};

#endif  // DRIVERS_H
