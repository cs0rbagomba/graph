#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>

#include <QtCore/QTime>

#include <graph/graph.hpp>
#include <graph/graph_xml.hpp>

#include "floats.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

#include "graphwidget.hpp"

namespace std {
  template <>
  struct hash<float2>
  {
    std::size_t operator()(const float2& f2) const {
      std::size_t h1 = std::hash<float>()(f2.x);
      std::size_t h2 = std::hash<float>()(f2.y);
      return h1 ^ (h2 << 1);
    }
  };

  template <typename T>
  inline std::string to_string_with_precision(const T a_value, const int n = 6)
  {
      std::ostringstream out;
      out << std::fixed << std::setprecision(n) << a_value;
      return out.str();
  }

  inline std::ostream& operator<< (std::ostream& os, const float2& f2)
  {
    os << std::to_string_with_precision(f2);
    return os;
  }
}

float2 float2creator(const std::string& line)
{
  std::stringstream ss(line);
  float f1, f2;
  ss >> f1 >> f2;
  return float2(f1, f2);
}

inline std::string float2serializer(const float2& f2)
{
  return std::to_string_with_precision(f2.x, 3) + "  " +  std::to_string_with_precision(f2.y, 3);
}


int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));


  const std::string xml_file = "graph_example.xml";
  Graph<float2> g = readGraphFromXML<float2>(xml_file, float2creator);

  GraphWidget *widget = new GraphWidget(&g);
  widget->updateFromGraph();

  QMainWindow mainWindow;
  mainWindow.setCentralWidget(widget);


  mainWindow.menuBar()->addAction("Zoom In", widget, SLOT(zoomIn()));
  mainWindow.menuBar()->addAction("Zoom Out", widget, SLOT(zoomOut()));
  mainWindow.menuBar()->addAction("Quit", widget, SLOT(quit()));

  mainWindow.show();
  const int app_retval = app.exec();

  writeGraphToXML<float2>(g, xml_file, float2serializer);
  return app_retval;
}
