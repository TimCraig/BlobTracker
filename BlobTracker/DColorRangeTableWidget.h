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

#include "DColorWidget.h"
#include "DBlobs.h"

#include <QTableWidget>
#include <QDialog>
#include <QContextMenuEvent>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QCheckBox>

#include "DBlobTracker.h"

/*****************************************************************************
*
***  class DColorRangeTableWidget
*
*****************************************************************************/

class DColorRangeTableWidget : public QTableWidget
   {
   Q_OBJECT

   using Base = QTableWidget;

   public:
      enum ERows { eBackgroundRow = 2, eBlobRowStart = 3 };
      enum EColumns { eMin1, eMax1, eCirc1, eMin2, eMax2, eMin3, eMax3,
            eDispColor };

      explicit DColorRangeTableWidget(QWidget* pParent = 0) :
            QTableWidget(2, 8, pParent)
         {
         Initialize();

         return;
         }

      DColorRangeTableWidget(const DColorRangeTableWidget& src) = delete;

      ~DColorRangeTableWidget() = default;

      DColorRangeTableWidget& operator=(const DColorRangeTableWidget& rhs) = delete;

      virtual void Initialize();
      virtual void SetColorScheme(DBlobTracker::EColorScheme eColorScheme,
            QString strColorScheme, QStringList ColorComponentNames);

      virtual void LoadColorTable(const DBlobColor::Vector& Colors);

   public slots:
      void InsertRow(int nRow);
      void InsertRow(int nRow, const DBlobColor& BlobColor);
      void AddRow();
      void DeleteRow();
      void DeleteRow(int nRow);
      void EditRow();

   protected:
      enum { eDisplayColorItem = QTableWidgetItem::UserType };

      DBlobTracker::EColorScheme m_eColorScheme;
      QString m_strColorScheme;
      QStringList m_ColorComponentNames;

      virtual void contextMenuEvent(QContextMenuEvent* pEvent);

   private:

   }; // end of class DColorRangeTableWidget

/*****************************************************************************
*
***  class DColorRangeTableEditor
*
*****************************************************************************/

class DColorRangeTableEditor : public QWidget
   {
      Q_OBJECT

   public:
      explicit DColorRangeTableEditor(QWidget* pParent = nullptr);

      DColorRangeTableEditor(const DColorRangeTableEditor& src) = delete;

      ~DColorRangeTableEditor() = default;

      DColorRangeTableEditor& operator=(const DColorRangeTableEditor& rhs) = delete;

      DColorRangeTableWidget* GetColorTable()
         {
         return (m_pTable);
         }

      virtual void SetColorScheme(DBlobTracker::EColorScheme eColorScheme,
            QString strColorScheme, QStringList ColorComponentNames);

   protected:
      DBlobTracker::EColorScheme m_eColorScheme;

      DColorRangeTableWidget* m_pTable;

      QPushButton* m_pButtonAdd;
      QPushButton* m_pButtonDelete;
      QPushButton* m_pButtonEdit;

   private:

   }; // end of class DColorRangeTableEditor

/*****************************************************************************
*
***  class DColorRangeRowEditDialog
*
*****************************************************************************/

class DColorRangeRowEditDialog : public QDialog
   {
      Q_OBJECT

   public:
      DColorRangeRowEditDialog(DBlobTracker::EColorScheme eColorScheme,
            const QString& strColorScheme,
            const QStringList& ColorComponentNames,
            QWidget* pParent = nullptr);

      DColorRangeRowEditDialog(const DColorRangeRowEditDialog& src) = delete;

      ~DColorRangeRowEditDialog() = default;

      DColorRangeRowEditDialog& operator=(const DColorRangeRowEditDialog& rhs) = delete;

      QColor GetDisplayColor() const
         {
         return (m_pDisplayColor->GetColor());
         }

      void SetDisplayColor(QColor Color)
         {
         m_pDisplayColor->SetColor(Color);

         return;
         }

      QString GetMin(int nNum) const
         {
         return (m_pMin[nNum]->text());
         }

      QString GetMax(int nNum) const
         {
         return (m_pMax[nNum]->text());
         }

      void SetMin(int nNum, QString strValue)
         {
         m_pMin[nNum]->setText(strValue);

         return;
         }

      void SetMax(int nNum, QString strValue)
         {
         m_pMax[nNum]->setText(strValue);

         return;
         }

      bool IsCircular() const
         {
         return (m_pCircular1->isChecked());
         }

      void SetCircular(bool bChecked)
         {
         m_pCircular1->setChecked(bChecked);

         return;
         }

   protected:
      QLineEdit* m_pMin[3];
      QLineEdit* m_pMax[3];
      QCheckBox* m_pCircular1;
      DColorWidget* m_pDisplayColor;

   private:

   }; // end of class DColorRangeRowEditDialog

#endif /* DCOLORRANGETABLEWIDGET_H_ */
