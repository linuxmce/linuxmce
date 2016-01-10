#include "InteractiveQLabel.h"

#include <QMouseEvent>

InteractiveQLabel::InteractiveQLabel(QWidget * parent, Qt::WindowFlags f)
	: QLabel(parent, f)
{
	setFocusPolicy(Qt::StrongFocus);
}

InteractiveQLabel::InteractiveQLabel(const QString & text, QWidget * parent, Qt::WindowFlags f)
	: QLabel(text, parent, f)
{
}

InteractiveQLabel::~InteractiveQLabel()
{
}

void InteractiveQLabel::mousePressEvent(QMouseEvent *ev)
{
	emit clicked(ev->x(), ev->y());
}

void InteractiveQLabel::keyPressEvent(QKeyEvent *ev)
{
	emit keypressed(ev->key());
}
