/*
 * ColorPlaneApp.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: Tim
 */

/*****************************************************************************
******************************  I N C L U D E  *******************************
*****************************************************************************/

#include <QApplication>
#include "DBlobTrackerMainWindow.h"

int main (int argc, char *argv[])
   {
   QApplication App(argc, argv);

   Q_INIT_RESOURCE(Druai);

   DBlobTrackerMainWindow MainWindow;
   MainWindow.Initialize();
   MainWindow.show();

   return (App.exec());

   } // end main


