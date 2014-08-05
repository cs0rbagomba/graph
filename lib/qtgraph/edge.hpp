#ifndef EDGE_H
#define EDGE_H

#include <QtGui/QGraphicsItem>

class Node;

class Edge : public QGraphicsItem
{
public:

    enum ArrowStyle {
      NoArrow,
      ArrowToDestination,
      ArrowToSource,
      ArrowToBothSides
    };

    Edge(Node *source, Node *destination, ArrowStyle arrowStyle = NoArrow );

    Node *sourceNode() const;
    Node *destNode() const;

    void adjust();

    enum { Type = UserType + 2 };
    int type() const { return Type; }

protected:
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    Node *source, *dest;
    ArrowStyle m_arrowStyle;

    QPointF sourcePoint;
    QPointF destPoint;
    qreal arrowSize;
};

#endif
