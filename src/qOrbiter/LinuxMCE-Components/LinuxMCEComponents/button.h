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
    Q_PROPERTY(qreal height READ getHeight WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(qreal width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_DISABLE_COPY(Button)
    
public:
    Button(QDeclarativeItem *parent = 0);
    ~Button();


    /*! @name Standard Properties */
    //@{
    qreal height;
    qreal width;
    QColor fillColor;
    QList<QColor*>gradient;
    //@}

    /*! @name Effect Properties */
    //@{
    bool dropShadowEnabled;
    int colorTransitionTime;
    //@}


    void setWidth(qreal t) {width = t; emit widthChanged();}
    qreal getWidth(){return width;}

    void setHeight(qreal h) {height = h; emit heightChanged();}
    qreal getHeight() {return height;}

signals:
    void heightChanged();
    void widthChanged();

protected:
   void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0);
};

QML_DECLARE_TYPE(Button)

#endif // BUTTON_H

