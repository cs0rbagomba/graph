#include "edge.hpp"
#include "node.hpp"
#include "graphwidget.hpp"

#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QStyleOption>


Node::Node(GraphWidget *graphWidget)
    : m_graphWidget(graphWidget)
{
    setFlag(ItemIsMovable);
    setFlag(ItemSendsGeometryChanges);
    setFlag(ItemIsSelectable);
    setAcceptHoverEvents(true);
    setCacheMode(DeviceCoordinateCache);
    setZValue(1); // higher than the edge
}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
}

void Node::removeEdge(Node* node)
{
//   QMutableListIterator<Edge*> i(edgeList);
//   while (i.hasNext())
//     if (i.next()->sourceNode() == node || i.next()->destNode() == node)
//       i.remove();

  edgeList.erase(std::remove_if(edgeList.begin(), edgeList.end(),
                                [node](Edge* e) { return e->sourceNode() == node || e->destNode() == node; }),
                 edgeList.end());
}

QList<Edge *> Node::edges() const
{
    return edgeList;
}

QRectF Node::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust,
                  23 + adjust, 23 + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *)
{
    QRadialGradient gradient(-3, -3, 10);
    if (isSelected()) {
      gradient.setCenter(3, 3);
      gradient.setFocalPoint(3, 3);
      gradient.setColorAt(0, Qt::red);
      gradient.setColorAt(1, Qt::darkRed);
    } else if (option->state & QStyle::State_MouseOver) {
      gradient.setColorAt(0, Qt::green);
      gradient.setColorAt(1, Qt::darkGreen);
    } else {
      gradient.setColorAt(0, Qt::yellow);
      gradient.setColorAt(1, Qt::darkYellow);
    }

    painter->setBrush(gradient);
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);
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
