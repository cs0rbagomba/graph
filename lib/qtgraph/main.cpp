#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>

#include <QtCore/QTime>


#include "graphwidget.hpp"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

  GraphWidget *widget = new GraphWidget;

  QMainWindow mainWindow;
  mainWindow.setCentralWidget(widget);


  mainWindow.menuBar()->addAction("Shuffle", widget, SLOT(shuffle()));
  mainWindow.menuBar()->addAction("Zoom In", widget, SLOT(zoomIn()));
  mainWindow.menuBar()->addAction("Zoom Out", widget, SLOT(zoomOut()));

  mainWindow.show();
  return app.exec();
}
