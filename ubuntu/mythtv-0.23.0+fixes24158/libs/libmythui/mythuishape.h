#ifndef MYTHUISHAPE_H_
#define MYTHUISHAPE_H_

// QT headers
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QLinearGradient>

// Mythui headers
#include "mythuitype.h"

class MythImage;

class MPUBLIC MythUIShape : public MythUIType
{
  public:
    MythUIShape(MythUIType *parent, const QString &name);
    ~MythUIShape();

    void Reset(void);

  protected:
    virtual void DrawSelf(MythPainter *p, int xoffset, int yoffset,
                          int alphaMod, QRect clipRect);

    virtual bool ParseElement(
        const QString &filename, QDomElement &element, bool showWarnings);
    virtual void CopyFrom(MythUIType *base);
    virtual void CreateCopy(MythUIType *parent);

    QLinearGradient parseGradient(const QDomElement &element);
    
    void DrawRect(const QRect &area, const QBrush &fillBrush,
                  const QPen &linePen);
    void DrawRoundRect(const QRect &area, int radius, const QBrush &fillBrush,
                       const QPen &linePen);
  private:
    MythImage     *m_image;
    QString        m_type;
    QBrush         m_fillBrush;
    QPen           m_linePen;
    int            m_cornerRadius;
};

#endif
