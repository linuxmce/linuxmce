
// Own header
#include "mythuishape.h"

// qt
#include <QDomDocument>
#include <QPainter>
#include <QBrush>
#include <QSize>

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
    m_fillColor = QColor();
    m_lineColor = QColor();
    m_drawFill = false;
    m_drawLine = false;
    m_lineWidth = 1;
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
    QRect area = m_Area;
    area.translate(xoffset, yoffset);

    if (!m_image || m_image->isNull())
    {
        if (m_type == "box")
            DrawRect(area, m_drawFill, m_fillColor,
                     m_drawLine, m_lineWidth, m_lineColor);
        else if (m_type == "roundbox")
            DrawRoundRect(area, m_cornerRadius, m_drawFill, m_fillColor,
                          m_drawLine, m_lineWidth, m_lineColor);
    }

    if (m_image)
        p->DrawImage(area.x(), area.y(), m_image, alphaMod);
}

void MythUIShape::DrawRect(const QRect &area,
                           bool drawFill, const QColor &fillColor,
                           bool drawLine, int lineWidth, const QColor &lineColor)
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

    if (drawLine)
        painter.setPen(QPen(lineColor, lineWidth));
    else
        painter.setPen(QPen(Qt::NoPen));

    if (drawFill)
        painter.setBrush(QBrush(fillColor));
    else
        painter.setBrush(QBrush(Qt::NoBrush));

    QRect r(lineWidth, lineWidth, area.width() - (lineWidth * 2), area.height() - (lineWidth * 2));
    painter.drawRect(r);

    painter.end();

    m_image = GetMythMainWindow()->GetCurrentPainter()->GetFormatImage();
    m_image->UpRef();
    m_image->Assign(image);
}

void MythUIShape::DrawRoundRect(const QRect &area, int radius,
                                bool drawFill, const QColor &fillColor,
                                bool drawLine, int lineWidth, const QColor &lineColor)
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

    if (drawLine)
        painter.setPen(QPen(lineColor, lineWidth));
    else
        painter.setPen(QPen(Qt::NoPen));

    if (drawFill)
        painter.setBrush(QBrush(fillColor));
    else
        painter.setBrush(QBrush(Qt::NoBrush));

    if ((area.width() / 2) < radius)
        radius = area.width() / 2;

    if ((area.height() / 2) < radius)
        radius = area.height() / 2;

    QRect r(lineWidth, lineWidth, area.width() - (lineWidth * 2), area.height() - (lineWidth * 2));
    painter.drawRoundedRect(r, (qreal)radius, qreal(radius));

    painter.end();

    m_image = GetMythMainWindow()->GetCurrentPainter()->GetFormatImage();
    m_image->UpRef();
    m_image->Assign(image);
}

bool MythUIShape::ParseElement(QDomElement &element)
{
    if (element.tagName() == "type")
    {
        QString type = getFirstText(element);
        if (type == "box" || type == "roundbox") // Validate input
            m_type = type;
    }
    else if (element.tagName() == "fill")
    {
        QString color = element.attribute("color", "");
        int alpha = element.attribute("alpha", "255").toInt();

        if (!color.isEmpty())
        {
            m_fillColor = QColor(color);
            m_fillColor.setAlpha(alpha);
            m_drawFill = true;
        }
        else
        {
           m_fillColor = QColor();
           m_drawFill = false;
        }
    }
    else if (element.tagName() == "line")
    {
        QString color = element.attribute("color", "");
        int alpha = element.attribute("alpha", "255").toInt();

        if (!color.isEmpty())
        {
            m_lineColor = QColor(color);
            m_lineColor.setAlpha(alpha);
            m_drawLine = true;
        }
        else
        {
           m_lineColor = QColor();
           m_drawLine = false;
        }

        m_lineWidth = element.attribute("width", "1").toInt();
    }
    else if (element.tagName() == "cornerradius")
    {
        m_cornerRadius = getFirstText(element).toInt();
    }
    else
        return MythUIType::ParseElement(element);

    return true;
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
    m_fillColor = shape->m_fillColor;
    m_lineColor = shape->m_lineColor;
    m_lineWidth = shape->m_lineWidth;
    m_drawFill = shape->m_drawFill;
    m_drawLine = shape->m_drawLine;
    m_cornerRadius = shape->m_cornerRadius;

    MythUIType::CopyFrom(base);
}

void MythUIShape::CreateCopy(MythUIType *parent)
{
    MythUIShape *shape = new MythUIShape(parent, objectName());
    shape->CopyFrom(this);
}

