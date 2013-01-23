#ifndef BUTTON_H
#define BUTTON_H

#include <QDeclarativeItem>
#ifdef GLENABLED
#include <QtOpenGL/QGLWidget>
#else
#include <QWidget>
#endif
#include <QPainter>
#include <QTouchEvent>

/*!
 * \brief The Button class is here to provide a UI component for all skin designers.
 * \ingroup ui_buttons
 */

class QtButton : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(qreal height READ getHeight WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(qreal width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged )
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(qreal borderWidth READ getBorderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_DISABLE_COPY(QtButton)
    
public:
    QtButton(QDeclarativeItem *parent = 0);
    ~QtButton();


    /*! @name Standard Properties */
    //@{
    qreal height;
    qreal width;
    QColor color;
    QColor borderColor;
    qreal borderWidth;
    QList<QColor*>gradient;
    //@}

    /*! @name Effect Properties */
    //@{
    bool dropShadowEnabled;
    int colorTransitionTime;
    //@}

    /*! @name State properties */
    //@{
    bool isPressed;
    bool isHovering;
    //@}

    void setWidth(qreal t) {width = t; setImplicitWidth(t); emit widthChanged();}
    qreal getWidth(){return width;}

    void setHeight(qreal h) {height = h; setImplicitHeight(h); emit heightChanged();}
    qreal getHeight() {return height;}

    void setColor(QColor c){color = c; emit colorChanged();}
    QColor getColor(){return color;}

    void setBorderColor(QColor c) {borderColor =c; emit borderColorChanged();}
    QColor getBorderColor(){return borderColor;}

    void setBorderWidth(qreal w) {borderWidth = w; emit borderWidthChanged();}
    qreal getBorderWidth() {return borderWidth;}

    bool getPressed(){return isPressed;}
    void setPressed(bool p){isPressed = p; emit pressedChanged();}

    bool getHovering(){return isHovering;}
    bool setHovering(bool h){isHovering =h; emit hoveringChanged();}
signals:
    void heightChanged();
    void widthChanged();
    void colorChanged();
    void borderColorChanged();
    void borderWidthChanged();
    void clicked();
    void pressed();
    void released();
    void hoveringChanged();
    void pressedChanged();



protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget=0);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

};

QML_DECLARE_TYPE(QtButton)

#endif // BUTTON_H

