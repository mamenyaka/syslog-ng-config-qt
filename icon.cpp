#include "icon.h"
#include "config.h"

#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>

#define ICON_SIZE 80

DriverIcon::DriverIcon(std::shared_ptr<Driver>& driver,
                       QWidget* parent) :
  QWidget(parent),
  driver(std::move(driver))
{
  setFixedSize(ICON_SIZE, ICON_SIZE);
  setAutoFillBackground(true);

  QLabel* label = new QLabel(QString::fromStdString(this->driver->get_name()), this);
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(0);
  mainLayout->addWidget(label);

  setupIcon();

  QPalette palette;
  palette.setBrush(QPalette::Window, QBrush(pixmap));
  setPalette(palette);
}

Driver& DriverIcon::get_driver()
{
  return *driver;
}

std::shared_ptr<Driver>& DriverIcon::get_driver_ptr()
{
  return driver;
}

const QPixmap& DriverIcon::get_pixmap() const
{
  return pixmap;
}

void DriverIcon::paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  if (driver->get_id() > -1)
  {
    painter.setRenderHints(QPainter::TextAntialiasing);
    painter.setPen(QPen(Qt::black));
    painter.drawText(0, height() - 20, width(), 20, Qt::AlignHCenter, QString::number(driver->get_id()));
  }

  painter.end();
}

void DriverIcon::setupIcon()
{
  pixmap = QPixmap(size());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(QPen(Qt::black));

  QRect rect(1, 1, width() - 2, height() - 2);

  switch (driver->get_type())
  {
    case DriverType::source:
    {
      painter.setBrush(QColor(255, 128, 128, 192));
      painter.drawEllipse(rect);
      break;
    }
    case DriverType::destination:
    {
      painter.setBrush(QColor(128, 128, 225, 192));
      painter.drawRect(rect);
      break;
    }
  }

  painter.end();
}


LogIcon::LogIcon(std::unique_ptr< Log, std::function<void(const Log *)> >& log,
                 QWidget* parent) :
  QWidget(parent),
  log(std::move(log))
{
  installEventFilter(this);

  QLabel* label = new QLabel("L\nO\nG");
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QFrame* frame = new QFrame;
  frame->setLineWidth(2);
  frame->setFrameStyle(QFrame::Box | QFrame::Plain);
  frame->setMinimumSize(ICON_SIZE, ICON_SIZE);

  frameLayout = new QVBoxLayout(frame);

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(label);
  mainLayout->addWidget(frame);
}

void LogIcon::add_driver(DriverIcon& icon)
{
  std::shared_ptr<const Driver> driver = icon.get_driver_ptr();
  log->add_driver(driver);
  frameLayout->addWidget(&icon);

  icon.show();
  adjustSize();
}

void LogIcon::remove_driver(DriverIcon& icon)
{
  std::shared_ptr<Driver>& driver = icon.get_driver_ptr();
  log->remove_driver(driver);
  frameLayout->removeWidget(&icon);

  frameLayout->activate();
  adjustSize();
}

bool LogIcon::eventFilter(QObject *, QEvent* event)
{
  event->ignore();
  return true;
}
