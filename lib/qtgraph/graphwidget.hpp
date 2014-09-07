#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QtGui/QGraphicsView>

class float2;
template<class T> class Graph;
class Node;
class Edge;

class GraphWidget : public QGraphicsView
{
  Q_OBJECT

public:
  GraphWidget(Graph<float2>* graph, QWidget *parent = 0);

  void itemMoved(const QPointF oldPos, const QPointF newPos);

  void updateFromGraph();

public slots:
  void zoomIn();
  void zoomOut();
  void quit();

protected:
  void keyPressEvent(QKeyEvent *event);
  void wheelEvent(QWheelEvent *event);
  void drawBackground(QPainter *painter, const QRectF &rect);

  void scaleView(qreal scaleFactor);

private:
  void insertNode(Node* selectedNode, QPointF pos);
  void removeNode(Node* selectedNode);
  void insertEdge(Node* selectedNode, Node* nodeUnderMouse);
  void removeEdge(Node* selectedNode, Node* nodeUnderMouse);

  Graph<float2>* m_graph;
  QList<Edge*> m_route;
};

#endif
