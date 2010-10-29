#ifndef __INTERACTIVEQLABEL_H__
#define __INTERACTIVEQLABEL_H__

#include <QLabel>

class InteractiveQLabel : public QLabel
{
Q_OBJECT
public:
	InteractiveQLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
	InteractiveQLabel(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
	virtual ~InteractiveQLabel();
signals:
	void clicked(int X, int Y);
	void keypressed(int key);
protected:
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void keyPressEvent(QKeyEvent *ev);
};

#endif // __INTERACTIVEQLABEL_H__
