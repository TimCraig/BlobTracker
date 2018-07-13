/*
 * DColorRangeTableWidget.cpp
 *
 *  Created on: Jun 8, 2014
 *      Author: Tim
 */

/*****************************************************************************
******************************  I N C L U D E  ******************************
****************************************************************************/

#include "DColorRangeTableWidget.h"

#include <QHeaderView>
#include <QColorDialog>

/*****************************************************************************
***  class DColorRangeTableWidget
****************************************************************************/

/*****************************************************************************
*
***  DColorRangeTableWidget::Initialize
*
****************************************************************************/

void DColorRangeTableWidget::Initialize()
   {
   // Hide the headers since they don't provide spanning
   horizontalHeader()->hide();
   verticalHeader()->hide();

   // Main header
   setSpan(0, DColorRangeTableModel::eMin1, 1, 3);
   setSpan(0, DColorRangeTableModel::eMin2, 1, 2);
   setSpan(0, DColorRangeTableModel::eMin3, 1, 2);

   QTableWidgetItem* pItem = new QTableWidgetItem;
   QFont Font(pItem->font());
   Font.setBold(true);
   pItem->setTextAlignment(Qt::AlignCenter);
   pItem->setForeground(QBrush(QColor(0, 0, 0)));
   pItem->setBackground(QBrush(QColor(255, 255, 0)));
   pItem->setFont(Font);
   pItem->setFlags(Qt::NoItemFlags);
   setItem(0, DColorRangeTableModel::eMin1, pItem);

   pItem = new QTableWidgetItem(*pItem);
   setItem(0, DColorRangeTableModel::eMin2, pItem);

   pItem = new QTableWidgetItem(*pItem);
   setItem(0, DColorRangeTableModel::eMin3, pItem);

   pItem = new QTableWidgetItem(*pItem);
   pItem->setData(Qt::DisplayRole, "Display Color");
   setItem(0, DColorRangeTableModel::eDispColor, pItem);

   for (int c = 0 ; c < columnCount() ; c++)
      {
      setColumnWidth(c, 90);
      } // end for

   // Secondary header
   QTableWidgetItem* pItemCirc = new QTableWidgetItem(*pItem);
   pItemCirc->setBackground(QBrush(QColor(0, 255, 255)));
   pItemCirc->setData(Qt::DisplayRole, "Circular");

   QTableWidgetItem* pItemMin = new QTableWidgetItem(*pItemCirc);
   pItemMin->setBackground(QBrush(QColor(0, 255, 0)));
   pItemMin->setData(Qt::DisplayRole, "Min");

   QTableWidgetItem* pItemMax = new QTableWidgetItem(*pItemCirc);
   pItemMax->setBackground(QBrush(QColor(255, 0, 0)));
   pItemMax->setData(Qt::DisplayRole, "Max");

   setItem(1, DColorRangeTableModel::eMin1, pItemMin);
   setItem(1, DColorRangeTableModel::eMax1, pItemMax);
   setItem(1, DColorRangeTableModel::eCirc1, pItemCirc);

   setItem(1, DColorRangeTableModel::eMin2, new QTableWidgetItem(*pItemMin));
   setItem(1, DColorRangeTableModel::eMax2, new QTableWidgetItem(*pItemMax));

   setItem(1, DColorRangeTableModel::eMin3, new QTableWidgetItem(*pItemMin));
   setItem(1, DColorRangeTableModel::eMax3, new QTableWidgetItem(*pItemMax));

   AddRow();

   return;

   } // end of method DColorRangeTableWidget::Initialize

/*****************************************************************************
*
***  DColorRangeTableWidget::InsertRow
*
****************************************************************************/

void DColorRangeTableWidget::InsertRow(int nRow)
   {
   Base::insertRow(nRow);

   // Populate the row with Table Widget Items
   QTableWidgetItem* pMin = new QTableWidgetItem;
   pMin->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

   QTableWidgetItem* pMax = new QTableWidgetItem(*pMin);

   // Set the initial range to all inclusive
   pMin->setData(Qt::DisplayRole, "0");
   pMax->setData(Qt::DisplayRole, "255");

   setItem(nRow, DColorRangeTableModel::eMin1, pMin);
   setItem(nRow, DColorRangeTableModel::eMax1, pMax);

   pMin = new QTableWidgetItem(*pMin);
   pMax = new QTableWidgetItem(*pMax);
   setItem(nRow, DColorRangeTableModel::eMin2, pMin);
   setItem(nRow, DColorRangeTableModel::eMax2, pMax);

   pMin = new QTableWidgetItem(*pMin);
   pMax = new QTableWidgetItem(*pMax);
   setItem(nRow, DColorRangeTableModel::eMin3, pMin);
   setItem(nRow, DColorRangeTableModel::eMax3, pMax);

   QTableWidgetItem* pCheck = new QTableWidgetItem;
   pCheck->setTextAlignment(Qt::AlignCenter);
   pCheck->setData(Qt::DisplayRole, "");
   pCheck->setFlags(
         Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   pCheck->setCheckState(Qt::Unchecked);
   setItem(nRow, DColorRangeTableModel::eCirc1, pCheck);

   QTableWidgetItem* pColor = new QTableWidgetItem(eDisplayColorItem);
   pColor->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   pColor->setBackground(QBrush(QColor(0, 0, 0)));
   setItem(nRow, DColorRangeTableModel::eDispColor, pColor);

   return;

   } // end of method DColorRangeTableWidget::InsertRow

/*****************************************************************************
*
***  DColorRangeTableWidget::AddRow
*
****************************************************************************/

void DColorRangeTableWidget::AddRow()
   {
   InsertRow(rowCount());

   return;

   } // end of method DColorRangeTableWidget::AddRow

/*****************************************************************************
*
***  DColorRangeTableWidget::DeleteRow
*
****************************************************************************/

void DColorRangeTableWidget::DeleteRow()
   {


   return;

   } // end of method DColorRangeTableWidget::DeleteRow

/*****************************************************************************
*
***  DColorRangeTableWidget::editItem
*
****************************************************************************/

void DColorRangeTableWidget::editItem(QTableWidgetItem* pItem)
   {
   if (pItem->type() == eDisplayColorItem)
      {
      QColor BlobColor = QColorDialog::getColor(pItem->background().color(), this,
            "Select Blog Color");

      if (BlobColor.isValid())
         {
         pItem->background().setColor(BlobColor);
         update();
         } // end if
      } // end if
   else
      {
      Base::editItem(pItem);
      } // end else

   return;

   } // end of method DColorRangeTableWidget::editItem

/*****************************************************************************
*
***  DColorRangeTableWidget::contextMenuEvent
*
****************************************************************************/

void DColorRangeTableWidget::contextMenuEvent(QContextMenuEvent* pEvent)
   {


   return;

   } // end of method DColorRangeTableWidget::contextMenuEvent
