#include "icon.h"
#include "object.h"
#include "dialog.h"

#include <QLabel>
#include <QIcon>
#include <QPainter>
#include <QPalette>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

#define ICON_SIZE 80

Icon::Icon(QWidget* parent) :
  QWidget(parent)
{}

void Icon::mouseMoveEvent(QMouseEvent* event)
{
  QPoint pos = event->globalPos() - QPoint(width()/2, height()/2);
  QPoint new_pos = parentWidget()->mapFromGlobal(pos);

  move(std::max(0, new_pos.x()), std::max(0, new_pos.y()));

  parentWidget()->update();
  parentWidget()->updateGeometry();
}

void Icon::mousePressEvent(QMouseEvent* event)
{
  emit pressed(this);
}

void Icon::mouseReleaseEvent(QMouseEvent* event)
{
  emit released(this);
}


ObjectIcon::ObjectIcon(std::shared_ptr<Object>& object,
                       QWidget* parent) :
  Icon(parent),
  object(std::move(object))
{
  setFixedSize(ICON_SIZE, ICON_SIZE);
  setAutoFillBackground(true);

  QLabel* label = new QLabel(QString::fromStdString(this->object->get_name()), this);
  label->setFont(QFont("Sans", 8, QFont::DemiBold));
  label->setAlignment(Qt::AlignCenter);
  label->setWordWrap(true);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setMargin(5);
  mainLayout->addWidget(label);

  setupIcon();
}

std::shared_ptr<Object>& ObjectIcon::get_object()
{
  return object;
}

void ObjectIcon::mouseDoubleClickEvent(QMouseEvent *)
{
  if (object->get_id() > -1)
  {
    Dialog(*object, this).exec();
  }
}

void ObjectIcon::paintEvent(QPaintEvent *)
{
  if (object->get_id() > -1)
  {
    QLabel* label = findChild<QLabel*>();
    label->setText("\n" + QString::fromStdString(object->get_name()) + "\n" + QString::number(object->get_id()));
  }
}

void ObjectIcon::setupIcon()
{
  QPixmap pixmap(size());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
  painter.setPen(Qt::black);

  object->draw(&painter, width(), height());

  painter.end();

  QPalette palette;
  palette.setBrush(QPalette::Background, QBrush(pixmap));
  setPalette(palette);
}


LogpathIcon::LogpathIcon(std::shared_ptr<Logpath>& logpath,
                         QWidget* parent) :
  Icon(parent),
  logpath(std::move(logpath))
{
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

void LogpathIcon::add_object(ObjectIcon& icon)
{
  if (icon.get_object()->get_type() == "template")
  {
    return;
  }

  std::shared_ptr<Object>& object = icon.get_object();
  logpath->add_object(object);

  const int index = get_index(icon);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->insertWidget(index, &icon);

  icon.show();
  adjustSize();
}

void LogpathIcon::remove_object(ObjectIcon& icon)
{
  std::shared_ptr<Object>& object = icon.get_object();
  logpath->remove_object(object);

  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  frameLayout->removeWidget(&icon);

  frameLayout->activate();
  adjustSize();
}

void LogpathIcon::mouseDoubleClickEvent(QMouseEvent* event)
{
  Dialog(logpath->get_options(), this).exec();
}

int LogpathIcon::get_index(ObjectIcon& icon)
{
  QVBoxLayout* frameLayout = findChild<QVBoxLayout*>();
  QPoint pos = mapFromParent(icon.pos());

  for (int i = 0; i < frameLayout->count(); i++)
  {
    Icon* icon = static_cast<Icon*>(frameLayout->itemAt(i)->widget());

    if (icon->y() > pos.y())
    {
      return frameLayout->indexOf(icon);
    }
  }

  return frameLayout->count();
}


DeleteIcon::DeleteIcon(QWidget* parent) :
  QWidget(parent)
{
  QLabel* icon = new QLabel;
  icon->setPixmap(QIcon::fromTheme("edit-delete-symbolic").pixmap(60, 60));

  QLabel* text = new QLabel("delete");
  text->setFont(QFont("Sans", 8, QFont::DemiBold));
  text->setAlignment(Qt::AlignCenter);

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(0);
  mainLayout->addWidget(icon);
  mainLayout->addWidget(text);
}
