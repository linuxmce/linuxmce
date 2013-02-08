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
#include <QUrl>
#include <QApplication>
/*!
 * \brief The Button class is here to provide a UI component for all skin designers.
 * \ingroup ui_buttons
 */

class QtButton : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(int height READ getHeight WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(int width READ getWidth WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged )
    Q_PROPERTY(QColor borderColor READ getBorderColor WRITE setBorderColor NOTIFY borderColorChanged)
    Q_PROPERTY(qreal borderWidth READ getBorderWidth WRITE setBorderWidth NOTIFY borderWidthChanged)
    Q_PROPERTY(bool isPressed READ getPressed WRITE setPressed NOTIFY pressedChanged)
    Q_PROPERTY(bool isHovering READ getHovering WRITE setHovering NOTIFY hoveringChanged)
    Q_PROPERTY(qreal radius READ getRadius WRITE setRadius NOTIFY radiusChanged )
    Q_PROPERTY(QUrl paintImage READ getPaintImage WRITE setPaintImage NOTIFY imageChanged)
    Q_DISABLE_COPY(QtButton)
    
public:
    QtButton(QDeclarativeItem *parent = 0);
    ~QtButton();


    /*! @name Standard Properties */
    //@{
    int height;
    int width;
    QColor color;
    QColor borderColor;
    qreal borderWidth;
    QList<QColor*>gradient;
    //@}

    /*! @name Extended Properties */
    //@{
    qreal radius;
    qreal topLeftRadius;
    qreal topRightRaduis;
    qreal bottomLeftRadius;
    qreal bottomRightRadius;
    //@}

    /*! @name Effect Properties */
    //@{
    bool dropShadowEnabled;
    int colorTransitionTime;
    QUrl paintImage;
    QImage m_PaintImage;
   // QImage paintIcon;
    //@}

    /*! @name State properties */
    //@{
    bool isPressed;
    bool isHovering;
    //@}

    void setPaintImage(QUrl i) {paintImage = i; emit imagePathChanged(); updateImage(paintImage); ;}
    QUrl getPaintImage(){return paintImage;}

    void setWidth(int t) {width = t; setImplicitWidth(t); emit widthChanged();scheduleUpdate();}
    qreal getWidth(){return width;}

    void setHeight(int h) {height = h; setImplicitHeight(h); emit heightChanged();scheduleUpdate();}
    qreal getHeight() {return height;}

    void setColor(QColor c){color = c; emit colorChanged();scheduleUpdate();}
    QColor getColor(){return color;}

    void setBorderColor(QColor c) {borderColor =c; emit borderColorChanged();scheduleUpdate();}
    QColor getBorderColor(){return borderColor;}

    void setBorderWidth(qreal w) {borderWidth = w; emit borderWidthChanged();scheduleUpdate();}
    qreal getBorderWidth() {return borderWidth;}

    bool getPressed(){return isPressed;}
    void setPressed(bool p){isPressed = p; emit pressedChanged();scheduleUpdate();}

    bool getHovering(){return isHovering;}
    void setHovering(bool h){isHovering =h; emit hoveringChanged();scheduleUpdate();}

    void setRadius(qreal r){radius = r; emit radiusChanged(); scheduleUpdate();}
    qreal getRadius(){return radius;}

    void setImage(QImage img){m_PaintImage = img; emit imageChanged(); scheduleUpdate();}

    void updateImage(QUrl img){
        QImage image;
        if(image.load(img.toLocalFile())){
            setImage(image);
            qDebug() << QApplication::applicationDirPath();
        }
    }

public slots:
    void scheduleUpdate(){update(QRect(0,0, width,height));}

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
    void radiusChanged();
    void iconChanged();
    void imageChanged();
    void imagePathChanged();



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

