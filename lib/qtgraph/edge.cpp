#include "edge.hpp"
#include "node.hpp"

#include <QtGui/QPainter>

#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

Edge::Edge(Node* s, Node* d, Edge::ArrowStyle arrowStyle  )
    : m_arrowStyle(arrowStyle)
    , arrowSize(10)
    , m_isRoute(false)
{
    setAcceptedMouseButtons(0);
    source = s;
    dest = d;
    source->addEdge(this);
    dest->addEdge(this);
    adjust();
}

Node *Edge::sourceNode() const
{
    return source;
}

Node *Edge::destNode() const
{
    return dest;
}

void Edge::adjust()
{
    if (!source || !dest)
        return;

    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();

    prepareGeometryChange();

    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        sourcePoint = destPoint = line.p1();
    }
}

QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    return QRectF(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                      destPoint.y() - sourcePoint.y()))
        .normalized()
        .adjusted(-extra, -extra, extra, extra);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!source || !dest)
        return;

    const QLineF line(sourcePoint, destPoint);
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Draw the line itself
    if (m_isRoute)
      painter->setPen(QPen(Qt::red, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    else
      painter->setPen(QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    painter->drawLine(line);

    // Draw the arrows
    if (m_arrowStyle == NoArrow)
      return;

    double angle = ::acos(line.dx() / line.length());
    if (line.dy() >= 0)
        angle = TwoPi - angle;

    painter->setBrush(Qt::black);

    if (m_arrowStyle == ArrowToSource || m_arrowStyle == ArrowToBothSides) {
      const QPointF sourceArrowP1 = sourcePoint + QPointF(sin(angle + Pi / 3) * arrowSize, cos(angle + Pi / 3) * arrowSize);
      const QPointF sourceArrowP2 = sourcePoint + QPointF(sin(angle + Pi - Pi / 3) * arrowSize, cos(angle + Pi - Pi / 3) * arrowSize);
      painter->drawPolygon(QPolygonF() << line.p1() << sourceArrowP1 << sourceArrowP2);
    }

    if (m_arrowStyle == ArrowToDestination || m_arrowStyle == ArrowToBothSides) {
      const QPointF destArrowP1 = destPoint + QPointF(sin(angle - Pi / 3) * arrowSize, cos(angle - Pi / 3) * arrowSize);
      const QPointF destArrowP2 = destPoint + QPointF(sin(angle - Pi + Pi / 3) * arrowSize, cos(angle - Pi + Pi / 3) * arrowSize);
      painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
    }
}