#ifndef NODE_H
#define NODE_H

#include <QtGui/QGraphicsItem>
#include <QtCore/QList>

class Edge;
class GraphWidget;
class QGraphicsSceneMouseEvent;

class Node : public QGraphicsItem
{
public:
  Node(GraphWidget *graphWidget);

  void addEdge(Edge *edge);
  void removeEdge(Edge* edge);
  QList<Edge *> edges() const;
  Edge* edgeTo(const Node* n) const;

  enum { Type = UserType + 1 };
  int type() const override { return Type; }

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
  QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
  QList<Edge *> edgeList;
  GraphWidget *m_graphWidget;
};

#endif
