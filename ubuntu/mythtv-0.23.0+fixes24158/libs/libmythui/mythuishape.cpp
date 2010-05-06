
// Own header
#include "mythuishape.h"

// C++
#include <algorithm>
using namespace std;

// qt
#include <QDomDocument>
#include <QPainter>
#include <QSize>
#include <QColor>

// myth
#include "mythverbose.h"
#include "mythpainter.h"
#include "mythimage.h"
#include "mythmainwindow.h"

MythUIShape::MythUIShape(MythUIType *parent, const QString &name)
          : MythUIType(parent, name)
{
    m_image = NULL;
    m_type = "box";
    m_fillBrush = QBrush(Qt::NoBrush);
    m_linePen = QPen(Qt::NoPen);
    m_cornerRadius = 10;
}

MythUIShape::~MythUIShape()
{
    if (m_image)
    {
        m_image->DownRef();
        m_image = NULL;
    }
}

void MythUIShape::Reset()
{
    if (m_image)
    {
        m_image->DownRef();
        m_image = NULL;
    }
    
    MythUIType::Reset();
}

void MythUIShape::DrawSelf(MythPainter *p, int xoffset, int yoffset,
                          int alphaMod, QRect clipRect)
{
    QRect area = GetArea();
    area.translate(xoffset, yoffset);

    if (!m_image || m_image->isNull())
    {
            if (m_type == "box")
            DrawRect(area, m_fillBrush, m_linePen);
        else if (m_type == "roundbox")
            DrawRoundRect(area, m_cornerRadius, m_fillBrush, m_linePen);
    }

    if (m_image)
        p->DrawImage(area.x(), area.y(), m_image, alphaMod);
}

void MythUIShape::DrawRect(const QRect &area,const QBrush &fillBrush,
                           const QPen &linePen)
{
    if (m_image)
    {
        m_image->DownRef();
        m_image = NULL;
    }

    QImage image(QSize(area.width(), area.height()), QImage::Format_ARGB32);
    image.fill(0x00000000);
    QPainter painter(&image);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(linePen);
    painter.setBrush(fillBrush);
    
    int lineWidth = linePen.width();
    QRect r(lineWidth, lineWidth,
            area.width() - (lineWidth * 2), area.height() - (lineWidth * 2));
    painter.drawRect(r);

    painter.end();

    m_image = GetMythMainWindow()->GetCurrentPainter()->GetFormatImage();
    m_image->UpRef();
    m_image->Assign(image);
}

void MythUIShape::DrawRoundRect(const QRect &area, int radius,
                                const QBrush &fillBrush, const QPen &linePen)
{
    if (m_image)
    {
        m_image->DownRef();
        m_image = NULL;
    }

    QImage image(QSize(area.width(), area.height()), QImage::Format_ARGB32);
    image.fill(0x00000000);
    QPainter painter(&image);

    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(linePen);
    painter.setBrush(fillBrush);

    if ((area.width() / 2) < radius)
        radius = area.width() / 2;

    if ((area.height() / 2) < radius)
        radius = area.height() / 2;

    int lineWidth = linePen.width();
    QRect r(lineWidth, lineWidth,
            area.width() - (lineWidth * 2), area.height() - (lineWidth * 2));
    painter.drawRoundedRect(r, (qreal)radius, qreal(radius));

    painter.end();

    m_image = GetMythMainWindow()->GetCurrentPainter()->GetFormatImage();
    m_image->UpRef();
    m_image->Assign(image);
}

bool MythUIShape::ParseElement(
    const QString &filename, QDomElement &element, bool showWarnings)
{
    if (element.tagName() == "type")
    {
        QString type = getFirstText(element);
        if (type == "box" || type == "roundbox") // Validate input
            m_type = type;
    }
    else if (element.tagName() == "fill")
    {
        QString style = element.attribute("style", "solid");
        QString color = element.attribute("color", "");
        int alpha = element.attribute("alpha", "255").toInt();

        if (style == "solid" && !color.isEmpty())
        {
            m_fillBrush.setStyle(Qt::SolidPattern);
            QColor brushColor = QColor(color);
            brushColor.setAlpha(alpha);
            m_fillBrush.setColor(brushColor);
        }
        else if (style == "gradient")
        {
            for (QDomNode child = element.firstChild(); !child.isNull();
                child = child.nextSibling())
            {
                QDomElement childElem = child.toElement();
                if (childElem.tagName() == "gradient")
                {
                    QLinearGradient gradient = parseGradient(childElem);
                    m_fillBrush = QBrush(gradient);
                }
            }
        }
        else
            m_fillBrush.setStyle(Qt::NoBrush);
    }
    else if (element.tagName() == "line")
    {
        QString style = element.attribute("style", "solid");
        QString color = element.attribute("color", "");

        if (style == "solid" && !color.isEmpty())
        {
            int orig_width = element.attribute("width", "1").toInt();
            int width = (orig_width) ? max(NormX(orig_width),1) : 0;
            int alpha = element.attribute("alpha", "255").toInt();
            QColor lineColor = QColor(color);
            lineColor.setAlpha(alpha);
            m_linePen.setColor(lineColor);
            m_linePen.setWidth(width);
            m_linePen.setStyle(Qt::SolidLine);
        }
        else
           m_linePen.setStyle(Qt::NoPen);
    }
    else if (element.tagName() == "cornerradius")
    {
        m_cornerRadius = NormX(getFirstText(element).toInt());
    }
    else
    {
        return MythUIType::ParseElement(filename, element, showWarnings);
    }

    return true;
}

QLinearGradient MythUIShape::parseGradient(const QDomElement &element)
{
    QLinearGradient gradient;
    QString gradientStart = element.attribute("start", "");
    QString gradientEnd = element.attribute("end", "");
    int gradientAlpha = element.attribute("alpha", "100").toInt();
    QString direction = element.attribute("direction", "vertical");

    float x1, y1, x2, y2 = 0.0;
    if (direction == "vertical")
    {
        x1 = 0.5;
        x2 = 0.5;
        y1 = 0.0;
        y2 = 1.0;
    }
    else if (direction == "diagonal")
    {
        x1 = 0.0;
        x2 = 1.0;
        y1 = 0.0;
        y2 = 1.0;
    }
    else
    {
        x1 = 0.0;
        x2 = 1.0;
        y1 = 0.5;
        y2 = 0.5;
    }

    gradient.setCoordinateMode(QGradient::ObjectBoundingMode);
    gradient.setStart(x1, y1);
    gradient.setFinalStop(x2, y2);

    QGradientStops stops;
    
    if (!gradientStart.isEmpty())
    {
        QColor startColor = QColor(gradientStart);
        startColor.setAlpha(gradientAlpha);
        QGradientStop stop(0.0, startColor);
        stops.append(stop);
    }
    
    if (!gradientEnd.isEmpty())
    {
        QColor endColor = QColor(gradientEnd);
        endColor.setAlpha(gradientAlpha);
        QGradientStop stop(1.0, endColor);
        stops.append(stop);
    }

    for (QDomNode child = element.firstChild(); !child.isNull();
        child = child.nextSibling())
    {
        QDomElement childElem = child.toElement();
        if (childElem.tagName() == "stop")
        {
            float position = childElem.attribute("position", "0").toFloat();
            QString color = childElem.attribute("color", "");
            int alpha = childElem.attribute("alpha", "-1").toInt();
            if (alpha < 0)
                alpha = gradientAlpha;
            QColor stopColor = QColor(color);
            stopColor.setAlpha(alpha);
            QGradientStop stop((position / 100), stopColor);
            stops.append(stop);
        }
    }

    gradient.setStops(stops);

    return gradient;
}

void MythUIShape::CopyFrom(MythUIType *base)
{
    MythUIShape *shape = dynamic_cast<MythUIShape *>(base);
    if (!shape)
    {
        VERBOSE(VB_IMPORTANT, "ERROR, bad parsing");
        return;
    }

    m_image = shape->m_image;
    if (m_image)
        m_image->UpRef();

    m_type = shape->m_type;
    m_fillBrush = shape->m_fillBrush;
    m_linePen = shape->m_linePen;
    m_cornerRadius = shape->m_cornerRadius;

    MythUIType::CopyFrom(base);
}

void MythUIShape::CreateCopy(MythUIType *parent)
{
    MythUIShape *shape = new MythUIShape(parent, objectName());
    shape->CopyFrom(this);
}

