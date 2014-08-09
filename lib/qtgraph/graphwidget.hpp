#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QtGui/QGraphicsView>

class float2;
template<class T> class Graph;
class Node;

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
  Graph<float2>* m_graph;
};

#endif
