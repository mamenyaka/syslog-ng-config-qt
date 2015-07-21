#include "drivers.h"
#include "icon.h"
#include "config.h"

#include <QLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

Drivers::Drivers(QWidget* parent) :
  QWidget(parent)
{}

void Drivers::setupDrivers(DriverType type, const std::vector<Driver>& default_drivers)
{
  for (const Driver& driver : default_drivers)
  {
    if (driver.get_type() == type)
    {
      DriverIcon* icon = new DriverIcon(const_cast<Driver&>(driver));
      layout()->addWidget(icon);
    }
  }
}

void Drivers::mousePressEvent(QMouseEvent *)
{
  DriverIcon* icon = select_nearest_driver();

  if (!icon)
  {
    return;
  }

  Driver& driver = icon->get_driver();

  QByteArray itemData;
  QDataStream dataStream(&itemData, QIODevice::WriteOnly);
  dataStream << QString::fromStdString(driver.get_name()) << (int) driver.get_type();

  QMimeData* mimeData = new QMimeData;
  mimeData->setData("drivericon", itemData);

  const QPixmap& pixmap = icon->get_pixmap();

  QDrag *drag = new QDrag(window());
  drag->setMimeData(mimeData);
  drag->setPixmap(pixmap);
  drag->setHotSpot(pixmap.rect().center());

  drag->exec();
}

DriverIcon* Drivers::select_nearest_driver() const
{
  for (DriverIcon* icon : findChildren<DriverIcon*>())
  {
    if (icon->underMouse())
    {
      return icon;
    }
  }

  return nullptr;
}
