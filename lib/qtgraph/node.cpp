#include "edge.hpp"
#include "node.hpp"
#include "graphwidget.hpp"

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>


Node::Node(GraphWidget *graphWidget, qreal radius)
  : m_graphWidget(graphWidget)
  , m_radius(radius)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
    setZValue(10); // higher than the edge
}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

void Node::removeEdge(Edge* edge)
{
  edgeList.removeAll(edge);
}

QList<Edge *> Node::edges() const
{
    return edgeList;
}

Edge* Node::edgeTo(const Node* n) const
{
  for (int i = 0; i < edgeList.size(); ++i) {
    Edge* e = edgeList.at(i);
     if (e->destNode() == n)
       return e;
  }
  return 0;
}

QRectF Node::boundingRect() const
{
  const qreal adjust = 0;
  return QRectF( -m_radius   - adjust, -m_radius    - adjust,
                  m_radius*2 + adjust,  m_radius*2  + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-m_radius, -m_radius, m_radius*2, m_radius*2);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *)
{
    if (isSelected()) {
      painter->setBrush(Qt::red);
    } else if (option->state & QStyle::State_MouseOver) {
      painter->setBrush(Qt::green);
    } else {
      painter->setBrush(Qt::yellow);
    }

    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(QRectF( -m_radius, -m_radius, m_radius*2, m_radius*2));
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
      case ItemPositionChange: {
        foreach (Edge *edge, edgeList)
            edge->adjust();

        const QPointF old_pos = pos();
        const QPointF new_pos = value.toPointF();
        m_graphWidget->itemMoved(old_pos, new_pos);
        break;
      }
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    update();
    QGraphicsItem::mouseReleaseEvent(event);
}
