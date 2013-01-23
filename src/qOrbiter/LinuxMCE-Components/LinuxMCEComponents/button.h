#ifndef BUTTON_H
#define BUTTON_H

#include <QDeclarativeItem>
#ifdef GLENABLED
#include <QtOpenGL/QGLWidget>
#else
#include <QWidget>
#endif
#include <QPainter>

/*!
 * \brief The Button class is here to provide a UI component for all skin designers.
 * \ingroup ui_buttons
 */

class Button : public QDeclarativeItem
{
    Q_OBJECT
    Q_DISABLE_COPY(Button)
    
public:
    Button(QDeclarativeItem *parent = 0);
    ~Button();
};

QML_DECLARE_TYPE(Button)

#endif // BUTTON_H

