/*
 * DColorWidget.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: Tim
 */

/*****************************************************************************
******************************  I N C L U D E  ******************************
****************************************************************************/

#include "DColorWidget.h"

#include <QPainter>
#include <QBrush>
#include <QColorDialog>

/*****************************************************************************
***  class DColorWidget
****************************************************************************/

/*****************************************************************************
*
***  DColorWidget::DColorWidget
*
****************************************************************************/

DColorWidget::DColorWidget(bool bAllowEdit, QWidget* pParent /* = nullptr */)
      : QWidget(pParent), m_bAllowEdit(bAllowEdit)
   {

   return;

   } // end of DColorWidget::DColorWidget

/*****************************************************************************
*
***  DColorWidget::mousePressEvent
*
****************************************************************************/

void DColorWidget::mousePressEvent(QMouseEvent* /* pEvent */)
   {
   if (m_bAllowEdit)
      {
      QColorDialog Dialog(m_Background, this);
      if (Dialog.exec() == QDialog::Accepted)
         {
         SetColor(Dialog.currentColor());
         } // end if
      } // end if

   return;

   } // end of method DColorWidget::mousePressEvent

/*****************************************************************************
*
***  DColorWidget::SetColor
*
****************************************************************************/

void DColorWidget::SetColor(const QColor& Color)
   {
   m_Background = Color;
   update();

   return;

   } // end of method DColorWidget::SetColor

/*****************************************************************************
*
***  DColorWidget::EnableEditing
*
****************************************************************************/

void DColorWidget::EnableEditing()
   {
   m_bAllowEdit = true;

   return;

   } // end of method DColorWidget::EnableEditing

/*****************************************************************************
*
***  DColorWidget::DisableEditing
*
****************************************************************************/

void DColorWidget::DisableEditing()
   {
   m_bAllowEdit = false;

   return;

   } // end of method DColorWidget::DisableEditing

/*****************************************************************************
*
***  DColorWidget::paintEvent
*
****************************************************************************/

void DColorWidget::paintEvent(QPaintEvent* /* pEvent */)
   {
   QPainter Painter(this);
   QBrush Brush(m_Background);
   Painter.setBrush(Brush);
   Painter.drawRect(rect());

   return;

   } // end of method DColorWidget::paintEvent
