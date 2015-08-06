#include "icon.h"
#include "driver.h"

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
  label->setFont(QFont("Sans", 8, QFont::DemiBold));
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(5);
  mainLayout->addWidget(label);

  setupIcon();

  QPalette palette;
  palette.setBrush(QPalette::Window, QBrush(pixmap));
  setPalette(palette);
}

std::shared_ptr<Driver>& DriverIcon::get_driver()
{
  return driver;
}

const QPixmap& DriverIcon::get_pixmap() const
{
  return pixmap;
}

void DriverIcon::paintEvent(QPaintEvent *)
{
  if (driver->get_id() > -1)
  {
    QLabel* label = findChild<QLabel*>();
    label->setText("\n" + QString::fromStdString(driver->get_name()) + "\n" + QString::number(driver->get_id()));
  }
}

void DriverIcon::setupIcon()
{
  pixmap = QPixmap(size());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::black);

  driver->draw(&painter, width(), height());

  painter.end();
}


LogIcon::LogIcon(std::shared_ptr<Log>& log,
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

  QVBoxLayout* frameLayout = new QVBoxLayout(frame);

  QHBoxLayout* mainLayout = new QHBoxLayout(this);
  mainLayout->addWidget(label);
  mainLayout->addWidget(frame);
}

std::shared_ptr<Log>& LogIcon::get_log()
{
  return log;
}

void LogIcon::add_driver(DriverIcon& icon)
{
  if (icon.get_driver()->get_type() == "template")
  {
    return;
  }

  std::shared_ptr<Driver>& driver = icon.get_driver();
  log->add_driver(driver);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->addWidget(&icon);

  icon.show();
  adjustSize();
}

void LogIcon::remove_driver(DriverIcon& icon)
{
  std::shared_ptr<Driver>& driver = icon.get_driver();
  log->remove_driver(driver);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->removeWidget(&icon);

  frameLayout->activate();
  adjustSize();
}

bool LogIcon::eventFilter(QObject *, QEvent* event)
{
  event->ignore();
  return true;
}
