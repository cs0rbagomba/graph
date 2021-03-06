#include "graphwidget.hpp"

#include "edge.hpp"
#include "node.hpp"

#include <graph/graph.hpp>
#include <graph/graph_algorithms.hpp>

#include "floats.hpp"
#include "marching_squares.hpp"

#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QGraphicsScene>
#include <QtGui/QPainter>

#include <QtGui/QApplication>

#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>

#include <functional>
#include <cassert>

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

  class distanceOf2float2s : public std::function<float(float2, float2)>
  {
  public:
    float operator()(float2 a, float2 b)
    {
      return dist(a, b);
    }
  };
} // namespace std

namespace {

float2 inline float2FromQPointF(const QPointF& p)
{
  return float2(p.x(), p.y());
}

QPointF inline QPointFFromfloat2(const float2& f)
{
  return QPointF(f.x, f.y);
}

QList<Edge*> calculateShortestRoute(const QGraphicsScene* scene,
                                       const Graph<float2>* graph,
                                       const Node* source,
                                       const Node* destination)
{

  const float2 s = float2FromQPointF(source->pos());
  const float2 d = float2FromQPointF(destination->pos());
  const std::vector<float2> shortestPath = dijkstra_shortest_path_to<float2, float>(*graph, s, d, std::distanceOf2float2s());

  QList<Edge*> route;
  const int lenghtOfRoute = shortestPath.size();
  if (lenghtOfRoute < 2)
    return route;

  for (int i = 0; i < lenghtOfRoute; ++i) {
    if (i+1 == lenghtOfRoute)
      break;

    QGraphicsItem* source_item = scene->itemAt(QPointFFromfloat2(shortestPath[i]));
    QGraphicsItem* destination_item = scene->itemAt(QPointFFromfloat2(shortestPath[i+1]));
    Node* source_node = dynamic_cast<Node*>(source_item);
    assert(source_node);
    Node* destination_node = dynamic_cast<Node*>(destination_item);
    assert(destination_node);
    Edge* e = source_node->edgeTo(destination_node);
    assert(e);
    route.append(e);
  }
  return route;
}

} // anonym namespace


GraphWidget::GraphWidget(Graph<float2>* graph, const QString& png_file, QWidget *p)
    : QGraphicsView(p)
    , m_graph(graph)
    , m_background(0)
    , m_lines()
    , m_lines_are_visible(true)
{
  MarchingSquares ms;
  ms.ReadImage(png_file.toStdString());
  m_lines = ms.RunMarchingSquares();

  m_background = new QPixmap(png_file);
  if (m_background->isNull())
    throw std::invalid_argument("Could not load image from png file.");

  const int h = m_background->height();
  const int w = m_background->width();

  QGraphicsScene *s = new QGraphicsScene(this);
  s->setItemIndexMethod(QGraphicsScene::NoIndex);
  s->setSceneRect(0, 0, w, h);
  setScene(s);

  setCacheMode(CacheBackground);
  setViewportUpdateMode(BoundingRectViewportUpdate);
  setRenderHint(QPainter::Antialiasing);
  setTransformationAnchor(AnchorUnderMouse);
  setMinimumSize(w, h);
  setWindowTitle(tr("Graph"));
}

GraphWidget::~GraphWidget()
{
  if (m_background != 0)
    delete m_background;
}

void GraphWidget::itemMoved(const QPointF oldPos, const QPointF newPos)
{
  float2 old_v = float2FromQPointF(oldPos);
  float2 new_v = float2FromQPointF(newPos);

  // if old_pos not found, returns silently
  m_graph->modifyVertex(old_v, new_v);
}

void GraphWidget::updateFromGraph()
{
  for (Graph<float2>::iterator cit = m_graph->begin(); cit != m_graph->end(); ++cit) {
    Node *node = new Node(this);
    scene()->addItem(node);
    node->setPos(QPointFFromfloat2(*cit));
  }

  for (Graph<float2>::iterator cit = m_graph->begin(); cit != m_graph->end(); ++cit) {
    for (const auto cit2 : m_graph->neighboursOf(*cit)) {
      Node* node1 = dynamic_cast<Node*>(scene()->itemAt(QPointFFromfloat2(*cit)));
      Node* node2 = dynamic_cast<Node*>(scene()->itemAt(QPointFFromfloat2(cit2)));
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

    case Qt::Key_Delete:
    case Qt::Key_Insert: {
      QList <QGraphicsItem* > selectedItems = scene()->selectedItems();
      if (selectedItems.isEmpty())
        break;

      QGraphicsItem* selectedItem = selectedItems.first();
      Node* selectedNode = dynamic_cast<Node*>(selectedItem);

      const QPoint global_p = QCursor::pos();
      const QPoint widget_p = mapFromGlobal(global_p);
      const QPointF scene_p = mapToScene(widget_p);

      QGraphicsItem* item_under_mouse = scene()->itemAt(scene_p);
      Node* nodeUnderMouse = dynamic_cast<Node*>(item_under_mouse);

      if (nodeUnderMouse != 0 && nodeUnderMouse != selectedNode) {
        if (e->key() == Qt::Key_Delete)
          removeEdge(selectedNode, nodeUnderMouse);
        else
          insertEdge(selectedNode, nodeUnderMouse);
      } else {
        if (e->key() == Qt::Key_Delete)
          removeNode(selectedNode);
        else
          insertNode(selectedNode, scene_p);
      }
      break;
    }
    case Qt::Key_Space: {
      modifyRoute();
      break;
    }
    case Qt::Key_L: {
      showLines();
      break;
    }
    default:
        QGraphicsView::keyPressEvent(e);
    }
}

void GraphWidget::modifyRoute()
{
  for (QList<Edge*>::iterator it = m_route.begin(); it != m_route.end(); ++it)
    (*it)->setIsRoute(false);

  QList <QGraphicsItem* > selectedItems = scene()->selectedItems();
  if (selectedItems.isEmpty())
    return;

  QGraphicsItem* selectedItem = selectedItems.first();
  Node* selectedNode = dynamic_cast<Node*>(selectedItem);

  const QPoint global_p = QCursor::pos();
  const QPoint widget_p = mapFromGlobal(global_p);
  const QPointF scene_p = mapToScene(widget_p);

  QGraphicsItem* item_under_mouse = scene()->itemAt(scene_p);
  Node* nodeUnderMouse = dynamic_cast<Node*>(item_under_mouse);

  if (nodeUnderMouse != 0 && nodeUnderMouse != selectedNode) {
    m_route = calculateShortestRoute(scene(), m_graph, selectedNode, nodeUnderMouse);
    for (QList<Edge*>::iterator it = m_route.begin(); it != m_route.end(); ++it)
      (*it)->setIsRoute(true);
  }
}

void GraphWidget::showLines()
{
  m_lines_are_visible = !m_lines_are_visible;
  if (m_lines_are_visible) {
    resetGraph();
    for (std::size_t i = 0; i < m_lines.size(); ++i) {
       const std::pair<float2, float2> e = m_lines[i];
       const float2 s = e.first;
       Node* s_n = 0;
       if (!contains(*m_graph, s)) {
        s_n = new Node(this);
        scene()->addItem(s_n);
        s_n->setPos(s.x, s.y);
       } else {
        QGraphicsItem* gi = scene()->itemAt(QPointFFromfloat2(s));
        s_n = dynamic_cast<Node*>(gi);
       }
       const float2 d = e.second;
       Node* d_n = 0;
       if (!contains(*m_graph, d)) {
        d_n = new Node(this);
        scene()->addItem(d_n);
        d_n->setPos(d.x, d.y);
       } else {
        QGraphicsItem* gi = scene()->itemAt(QPointFFromfloat2(d));
        d_n = dynamic_cast<Node*>(gi);
       }
       insertEdge(s_n, d_n);
    }
  }
}

void GraphWidget::resetGraph()
{
  for (QList<Edge*>::iterator it = m_route.begin(); it != m_route.end(); ++it)
    (*it)->setIsRoute(false);

  m_route.clear();
  m_graph->clear();
  QList <QGraphicsItem* > all_items = scene()->items();
  for (int i = 0; i < all_items.size(); ++i) {
    scene()->removeItem(all_items.at(i));
    delete all_items.at(i);
  }
}


void GraphWidget::removeEdge(Node* selectedNode, Node* nodeUnderMouse)
{
  const float2 source_pos = float2FromQPointF(selectedNode->pos());
  const float2 destination_pos = float2FromQPointF(nodeUnderMouse->pos());
  if (!connected(*m_graph, source_pos, destination_pos))
    return;

  m_graph->removeEdge(source_pos, destination_pos);
  QList<Edge *> edges_of_selected = selectedNode->edges();
  for (Edge* edge : edges_of_selected) {
    if (edge->sourceNode() == nodeUnderMouse || edge->destNode() == nodeUnderMouse) {
      selectedNode->removeEdge(edge);
      nodeUnderMouse->removeEdge(edge);
      scene()->removeItem(edge);
      delete edge;
      return;
    }
  }
}

void GraphWidget::removeNode(Node* selectedNode)
{
  m_graph->removeVertex(float2FromQPointF(selectedNode->pos()));
  QList<Edge *> edges_of_selected = selectedNode->edges();
  for (Edge* edge : edges_of_selected) {
    edge->sourceNode()->removeEdge(edge);
    edge->destNode()->removeEdge(edge);
    scene()->removeItem(edge);
    delete edge;
  }
  scene()->removeItem(selectedNode);
}

void GraphWidget::insertEdge(Node* selectedNode, Node* nodeUnderMouse)
{
  const float2 source_pos = float2FromQPointF(selectedNode->pos());
  const float2 destination_pos = float2FromQPointF(nodeUnderMouse->pos());
  if (connected(*m_graph, source_pos, destination_pos))
    return;

  scene()->addItem(new Edge(selectedNode, nodeUnderMouse));
  m_graph->addEdge(source_pos, destination_pos);
}


void GraphWidget::insertNode(Node* selectedNode, QPointF scene_p)
{
  Node *node = new Node(this);
  scene()->addItem(node);
  node->setPos(scene_p.x(), scene_p.y());
  scene()->addItem(new Edge(selectedNode, node));

  const float2 source_pos = float2FromQPointF(selectedNode->pos());
  const float2 destination_pos = float2FromQPointF(scene_p);
  m_graph->addEdge(source_pos, destination_pos);

  selectedNode->setSelected(false);
  node->setSelected(true);
}

void GraphWidget::wheelEvent(QWheelEvent *e)
{
    scaleView(pow((double)2, -e->delta() / 240.0));
}

void GraphWidget::drawBackground(QPainter* painter, const QRectF& /*r*/)
{
  const int h = m_background->height();
  const int w = m_background->width();
  painter->drawPixmap(QRectF(0, 0, w, h), *m_background, QRectF(0, 0, w, h));
}

void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}

void GraphWidget::mouseMoveEvent(QMouseEvent* e)
{
  const QPoint global_p = QCursor::pos();
  const QPoint widget_p = mapFromGlobal(global_p);
  const QPointF scene_p = mapToScene(widget_p);

  const QGraphicsItem* i = scene()->itemAt(scene_p);
  const Node* n = dynamic_cast<const Node*>(i);

  QString msg(QString("(x,y) coordinates: (%1,%2)").arg(e->x()).arg(e->y()));
  if (n != 0)
    msg += QString("(node) coordinates: (%1,%2)").arg(n->pos().x()).arg(n->pos().y());

  QObject* p = parentWidget();
  QMainWindow* w = dynamic_cast<QMainWindow*>(p);
  w->statusBar()->showMessage(msg);

  QGraphicsView::mouseMoveEvent(e);
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
