/*
 * DColorRangeTableWidget.h
 *
 *  Created on: Jun 8, 2014
 *      Author: Tim
 */

#ifndef DCOLORRANGETABLEWIDGET_H_
#define DCOLORRANGETABLEWIDGET_H_

/*****************************************************************************
 ******************************  I N C L U D E  ******************************
 ****************************************************************************/

#include <QTableWidget>
#include <QContextMenuEvent>
#include <QItemDelegate>

#include "DBlobs.h"

/*****************************************************************************
 *
 ***  class DColorRangeTableDelegate
 *
 *****************************************************************************/

class DColorRangeTableDelegate : public QItemDelegate
   {
      Q_OBJECT

   public:
      DColorRangeTableDelegate(int nDispColorCol, QObject* pParent = nullptr) :
            QItemDelegate(pParent)
         {
         m_nDispColorCol = nDispColorCol;
         return;
         }

      ~DColorRangeTableDelegate()
         {
         return;
         }

   protected:
      int m_nDispColorCol;

   private:
      DColorRangeTableDelegate(const DColorRangeTableDelegate& src);
      DColorRangeTableDelegate& operator=(const DColorRangeTableDelegate& rhs);
   };
// end of class DColorRangeTableDelegate

/*****************************************************************************
 *
 ***  class DColorRangeTableModel
 *
 *****************************************************************************/

class DColorRangeTableModel : public QAbstractTableModel
   {
      Q_OBJECT

   public:
      enum EColumns
          {
          eMin1, eMax1, eCirc1, eMin2, eMax2, eMin3, eMax3, eDispColor,
          eNumColumns
          };

      DColorRangeTableModel(QObject* pParent = nullptr) :
            QAbstractTableModel(pParent), m_pBlobColors(nullptr)
         {
         return;
         }

      ~DColorRangeTableModel()
         {
         return;
         }

      void SetBlobColors(DBlobColor::Vector* pBlobColors)
         {
         m_pBlobColors = pBlobColors;
         return;
         }

      int rowCount(const QModelIndex & /* pParent */) const
         {
         return (m_pBlobColors->size() + m_nHeaderRows);
         }

      int columnCount(const QModelIndex & /* pParent */) const
          {
          return (eNumColumns);
          }

   protected:
      const int m_nHeaderRows = 2;

      DBlobColor::Vector* m_pBlobColors;

   private:
      DColorRangeTableModel(const DColorRangeTableModel& src);
      DColorRangeTableModel& operator=(const DColorRangeTableModel& rhs);
   };
// end of class DColorRangeTableModel

/*****************************************************************************
 *
 ***  class DColorRangeTableWidget
 *
 *****************************************************************************/

class DColorRangeTableWidget : public QTableWidget
   {
      Q_OBJECT

      typedef QTableWidget Base;

   public:
      DColorRangeTableWidget(QWidget * pParent = 0) :
            QTableWidget(2, 8, pParent)
         {
         Initialize();

         return;
         }

      ~DColorRangeTableWidget()
         {
         return;
         }

      virtual void Initialize();
      virtual void editItem(QTableWidgetItem* pItem);

      public slots:
      void InsertRow(int nRow);
      void AddRow();
      void DeleteRow();

   protected:
      enum
         {
         eDisplayColorItem = QTableWidgetItem::UserType
         };

      virtual void contextMenuEvent(QContextMenuEvent* pEvent);

   private:
      DColorRangeTableWidget(const DColorRangeTableWidget& src);
      DColorRangeTableWidget& operator=(const DColorRangeTableWidget& rhs);
   };
// end of class DColorRangeTableWidget

#endif /* DCOLORRANGETABLEWIDGET_H_ */
