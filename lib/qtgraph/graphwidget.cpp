#include "graphwidget.hpp"

#include "edge.hpp"
#include "node.hpp"

#include <graph/graph.hpp>
#include "floats.hpp"

#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QGraphicsScene>

#include <QtGui/QApplication>

namespace std {
  template <>
  struct hash<float2>
  {
    std::size_t operator()(const float2& f2) const
    {
      std::size_t h1 = std::hash<float>()(f2.x);
      std::size_t h2 = std::hash<float>()(f2.y);
      return h1 ^ (h2 << 1);
    }
  };
}

// for the map
bool operator< (const float2& v1, const float2& v2)
{
  return length(v1) < length(v2);
}

float2 inline float2FromQPointF(const QPointF& p)
{
  return float2(p.x(), p.y());
}


GraphWidget::GraphWidget(Graph<float2>* graph, QWidget *p)
    : QGraphicsView(p)
    , m_graph(graph)
{
    QGraphicsScene *s = new QGraphicsScene(this);
    s->setItemIndexMethod(QGraphicsScene::NoIndex);
    s->setSceneRect(-200, -200, 400, 400);
    setScene(s);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Elastic Nodes"));
}

void GraphWidget::itemMoved(const QPointF oldPos, const QPointF newPos)
{
  float2 old_v = float2FromQPointF(oldPos);
  float2 new_v = float2FromQPointF(newPos);
  m_graph->modifyVertex(old_v, new_v);
}

void GraphWidget::updateFromGraph()
{
  // scene()->itemAt returns the topmost only which can be an Edge
  QMap<float2, Node*> node_map;
  for (Graph<float2>::iterator cit = m_graph->begin(); cit != m_graph->end(); ++cit) {
    Node *node = new Node(this);
    scene()->addItem(node);
    node->setPos(cit->x, cit->y);
    node_map.insert(*cit, node);
  }

  for (Graph<float2>::iterator cit = m_graph->begin(); cit != m_graph->end(); ++cit) {
    for (const auto cit2 : m_graph->neighboursOf(*cit)) {
      Node* node1 = node_map.find(*cit).value();
      Node* node2 = node_map.find(cit2).value();
      scene()->addItem(new Edge(node1, node2));
    }
  }
}

void GraphWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Insert: {
      QList <QGraphicsItem* > selectedItems = scene()->selectedItems();
      if (selectedItems.isEmpty())
        break;

      QGraphicsItem* selectedItem = selectedItems.first();
      Node* selectedNode = dynamic_cast<Node*>(selectedItem);

      const QPoint global_p = QCursor::pos();
      const QPoint widget_p = mapFromGlobal(global_p);
      const QPointF scene_p = mapToScene(widget_p);

      Node *node = new Node(this);
      scene()->addItem(node);
      node->setPos(scene_p.x(), scene_p.y());
      scene()->addItem(new Edge(selectedNode, node));

      const float2 source_pos = float2FromQPointF(selectedItem->pos());
      const float2 destination_pos = float2FromQPointF(scene_p);
      m_graph->addEdge(source_pos, destination_pos);
    }
    default:
        QGraphicsView::keyPressEvent(e);
    }
}

void GraphWidget::wheelEvent(QWheelEvent *e)
{
    scaleView(pow((double)2, -e->delta() / 240.0));
}

void GraphWidget::drawBackground(QPainter *painter, const QRectF &r)
{
    // Shadow
    QRectF scene_rect = this->sceneRect();
    QRectF rightShadow(scene_rect.right(), scene_rect.top() + 5, 5, scene_rect.height());
    QRectF bottomShadow(scene_rect.left() + 5, scene_rect.bottom(), scene_rect.width(), 5);
    if (rightShadow.intersects(r) || rightShadow.contains(r))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(r) || bottomShadow.contains(r))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(scene_rect.topLeft(), scene_rect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(r.intersect(scene_rect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(scene_rect);
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}

void GraphWidget::quit()
{
  QApplication::quit();
}
