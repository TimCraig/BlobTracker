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
#include "DQOpenCV.h"

#include <QHeaderView>
#include <QColorDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QIntValidator>

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
   setSpan(0, eMin1, 1, 3);
   setSpan(0, eMin2, 1, 2);
   setSpan(0, eMin3, 1, 2);

   QTableWidgetItem* pItem = new QTableWidgetItem;
   QFont Font(pItem->font());
   Font.setBold(true);
   pItem->setTextAlignment(Qt::AlignCenter);
   pItem->setForeground(QBrush(QColor(0, 0, 0)));
   pItem->setBackground(QBrush(QColor(255, 255, 0)));
   pItem->setFont(Font);
   pItem->setFlags(Qt::NoItemFlags);
   setItem(0, eMin1, pItem);

   pItem = new QTableWidgetItem(*pItem);
   setItem(0, eMin2, pItem);

   pItem = new QTableWidgetItem(*pItem);
   setItem(0, eMin3, pItem);

   pItem = new QTableWidgetItem(*pItem);
   pItem->setData(Qt::DisplayRole, "Display Color");
   setItem(0, eDispColor, pItem);

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

   setItem(1, eMin1, pItemMin);
   setItem(1, eMax1, pItemMax);
   setItem(1, eCirc1, pItemCirc);

   setItem(1, eMin2, new QTableWidgetItem(*pItemMin));
   setItem(1, eMax2, new QTableWidgetItem(*pItemMax));

   setItem(1, eMin3, new QTableWidgetItem(*pItemMin));
   setItem(1, eMax3, new QTableWidgetItem(*pItemMax));

   AddRow();

   return;

   } // end of method DColorRangeTableWidget::Initialize

/*****************************************************************************
*
***  DColorRangeTableWidget::SetColorScheme
*
****************************************************************************/

void DColorRangeTableWidget::SetColorScheme(
      DBlobTracker::EColorScheme eColorScheme,
      QString strColorScheme, QStringList ColorComponentNames)
   {
   m_eColorScheme = eColorScheme;
   m_strColorScheme = strColorScheme;
   m_ColorComponentNames = ColorComponentNames;

   item(0, 0)->setData(Qt::DisplayRole, ColorComponentNames[0]);
   item(0, 3)->setData(Qt::DisplayRole, ColorComponentNames[1]);
   item(0, 5)->setData(Qt::DisplayRole, ColorComponentNames[2]);

   return;

   } // end of method DColorRangeTableWidget::SetColorScheme

/******************************************************************************
*
***  DColorRangeTableWidget::LoadColorTable
*
******************************************************************************/

void DColorRangeTableWidget::LoadColorTable(const DBlobColor::Vector& Colors)
   {
   // Resize the grid, if necessary
   int nColors = static_cast<int>(Colors.size());
   int nCurColors = rowCount() - 2;
   int nExcess = nCurColors - nColors;
   if (nExcess > 0)
      {
      // Remove the excess rows
      for (int i = 0 ; i < nExcess ; i++)
         {
         DeleteRow(rowCount() - 1);
         } // end for
      } // end if

   // Overwrite the existing rows
   int nRow = DColorRangeTableWidget::eBackgroundRow;
   for (int nColor = 0 ; nColor < nCurColors ; nColor++)
      {
      QString strValue;
      const DBlob::DBlobColorRange& Range(Colors[nColor].GetColorRange());
      strValue.setNum(static_cast<int>(Range.GetColor1().GetLower()));
      item(nRow, DColorRangeTableWidget::eMin1)->setText(strValue);

      strValue.setNum(static_cast<int>(Range.GetColor1().GetUpper()));
      item(nRow, DColorRangeTableWidget::eMax1)->setText(strValue);

      item(nRow, DColorRangeTableWidget::eCirc1)->setCheckState(
               Range.GetColor1().IsCircular() ? Qt::Checked : Qt::Unchecked);

      strValue.setNum(static_cast<int>(Range.GetColor2().GetLower()));
      item(nRow, DColorRangeTableWidget::eMin2)->setText(strValue);

      strValue.setNum(static_cast<int>(Range.GetColor2().GetUpper()));
      item(nRow, DColorRangeTableWidget::eMax2)->setText(strValue);

      strValue.setNum(static_cast<int>(Range.GetColor3().GetLower()));
      item(nRow, DColorRangeTableWidget::eMin3)->setText(strValue);

      strValue.setNum(static_cast<int>(Range.GetColor3().GetUpper()));
      item(nRow, DColorRangeTableWidget::eMax3)->setText(strValue);

      QColor BackgroundColor = Colors[nColor].GetDisplayColor();
      QTableWidgetItem* pColor = item(nRow, DColorRangeTableWidget::eDispColor);
      QBrush Brush(BackgroundColor);
      pColor->setBackground(Brush);

      nRow++;
      } // end for

   // Add any new rows
   for (int nColor = nCurColors ; nColor < nColors ; nColor++)
      {
      InsertRow(nRow++, Colors[nColor]);
      } // end for

   return;

   } // end of method DColorRangeTableWidget::LoadColorTable

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

   setItem(nRow, eMin1, pMin);
   setItem(nRow, eMax1, pMax);

   pMin = new QTableWidgetItem(*pMin);
   pMax = new QTableWidgetItem(*pMax);
   setItem(nRow, eMin2, pMin);
   setItem(nRow, eMax2, pMax);

   pMin = new QTableWidgetItem(*pMin);
   pMax = new QTableWidgetItem(*pMax);
   setItem(nRow, eMin3, pMin);
   setItem(nRow, eMax3, pMax);

   QTableWidgetItem* pCheck = new QTableWidgetItem;
   pCheck->setTextAlignment(Qt::AlignCenter);
   pCheck->setData(Qt::DisplayRole, "");
   pCheck->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   pCheck->setCheckState(Qt::Unchecked);
   setItem(nRow, eCirc1, pCheck);

   QTableWidgetItem* pColor = new QTableWidgetItem(eDisplayColorItem);
   pColor->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   QBrush Brush(QColor(0, 0, 0));
   pColor->setBackground(Brush);
   setItem(nRow, eDispColor, pColor);

   return;

   } // end of method DColorRangeTableWidget::InsertRow

/*****************************************************************************
 *
 ***  DColorRangeTableWidget::InsertRow
 *
 ****************************************************************************/

void DColorRangeTableWidget::InsertRow(int nRow, const DBlobColor& BlobColor)
   {
   Base::insertRow(nRow);

   // Populate the row with Table Widget Items
   QTableWidgetItem* pMin = new QTableWidgetItem;
   pMin->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

   QTableWidgetItem* pMax = new QTableWidgetItem(*pMin);

   QString strNum;
   const DBlob::DBlobColorRange& ColorRange = BlobColor.GetColorRange();

   // Set to the passed BlobColor
   pMin->setData(Qt::DisplayRole, strNum.setNum(ColorRange.GetColor1().GetLower()));
   pMax->setData(Qt::DisplayRole, strNum.setNum(ColorRange.GetColor1().GetUpper()));

   setItem(nRow, eMin1, pMin);
   setItem(nRow, eMax1, pMax);

   QTableWidgetItem* pCheck = new QTableWidgetItem;
   pCheck->setTextAlignment(Qt::AlignCenter);
   pCheck->setData(Qt::DisplayRole, "");
   pCheck->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
   pCheck->setCheckState(ColorRange.GetColor1().IsCircular() ? Qt::Checked : Qt::Unchecked);
   setItem(nRow, eCirc1, pCheck);

   pMin = new QTableWidgetItem(*pMin);
   pMax = new QTableWidgetItem(*pMax);

   pMin->setData(Qt::DisplayRole, strNum.setNum(ColorRange.GetColor2().GetLower()));
   pMax->setData(Qt::DisplayRole, strNum.setNum(ColorRange.GetColor2().GetUpper()));

   setItem(nRow, eMin2, pMin);
   setItem(nRow, eMax2, pMax);

   pMin = new QTableWidgetItem(*pMin);
   pMax = new QTableWidgetItem(*pMax);

   pMin->setData(Qt::DisplayRole, strNum.setNum(ColorRange.GetColor3().GetLower()));
   pMax->setData(Qt::DisplayRole, strNum.setNum(ColorRange.GetColor3().GetUpper()));

   setItem(nRow, eMin3, pMin);
   setItem(nRow, eMax3, pMax);

   QTableWidgetItem* pColor = new QTableWidgetItem(eDisplayColorItem);
   pColor->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
   QBrush Brush(BlobColor.GetDisplayColor());
   setItem(nRow, eDispColor, pColor);
   pColor->setBackground(Brush);

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
   setCurrentCell(rowCount() - 1, 0, QItemSelectionModel::SelectCurrent);

   return;

   } // end of method DColorRangeTableWidget::AddRow

/*****************************************************************************
 *
 ***  DColorRangeTableWidget::DeleteRow
 *
 ****************************************************************************/

void DColorRangeTableWidget::DeleteRow()
   {
   DeleteRow(currentRow());

   return;

   } // end of method DColorRangeTableWidget::DeleteRow

/*****************************************************************************
 *
 ***  DColorRangeTableWidget::DeleteRow
 *
 ****************************************************************************/

void DColorRangeTableWidget::DeleteRow(int nRow)
   {
   if (nRow > 2)
      {
      removeRow(nRow);
      } // end if

   return;

   } // end of method DColorRangeTableWidget::DeleteRow

/*****************************************************************************
 *
 ***  DColorRangeTableWidget::EditRow
 *
 ****************************************************************************/

void DColorRangeTableWidget::EditRow()
   {
   int nCurrentRow = currentRow();
   if (nCurrentRow == 2)
      {
      // Row for background color, just bring up the color dialog
      QTableWidgetItem* pColor = item(nCurrentRow, eDispColor);

      QColorDialog Dialog(pColor->backgroundColor(), this);
      if (Dialog.exec() == QDialog::Accepted)
         {
         pColor->setBackgroundColor(Dialog.currentColor());

         update();
         } // end if
      } // end if
   else if (nCurrentRow >= 3)
      {
      DColorRangeRowEditDialog Dialog(m_eColorScheme, m_strColorScheme,
            m_ColorComponentNames, this);
      QTableWidgetItem* pColor = item(nCurrentRow, eDispColor);
      Dialog.SetDisplayColor(pColor->backgroundColor());

      QTableWidgetItem* pMin[3];
      QTableWidgetItem* pMax[3];

      pMin[0] = item(nCurrentRow, eMin1);
      pMax[0] = item(nCurrentRow, eMax1);
      pMin[1] = item(nCurrentRow, eMin2);
      pMax[1] = item(nCurrentRow, eMax2);
      pMin[2] = item(nCurrentRow, eMin3);
      pMax[2] = item(nCurrentRow, eMax3);

      QTableWidgetItem* pCircular = item(nCurrentRow, eCirc1);
      Dialog.SetCircular(pCircular->checkState() == Qt::Checked);

      for (int i = 0 ; i < 3 ; i++)
         {
         Dialog.SetMin(i, pMin[i]->text());
         Dialog.SetMax(i, pMax[i]->text());
         } // end for

      if (Dialog.exec() == QDialog::Accepted)
         {
         for (int i = 0 ; i < 3 ; i++)
            {
            pMin[i]->setText(Dialog.GetMin(i));
            pMax[i]->setText(Dialog.GetMax(i));
            } // end for

         pCircular->setCheckState(Dialog.IsCircular() ? Qt::Checked : Qt::Unchecked);
         QBrush Brush(Dialog.GetDisplayColor());
         pColor->setBackground(Brush);
         } // end if
      } // end else if

   return;

   } // end of method DColorRangeTableWidget::EditRow

/*****************************************************************************
 *
 ***  DColorRangeTableWidget::contextMenuEvent
 *
 ****************************************************************************/

void DColorRangeTableWidget::contextMenuEvent(QContextMenuEvent* /* pEvent */)
   {

   return;

   } // end of method DColorRangeTableWidget::contextMenuEvent

/*****************************************************************************
 ***  class DColorRangeTableEditor
 ****************************************************************************/

/*****************************************************************************
 *
 ***  DColorRangeTableEditor::DColorRangeTableEditor
 *
 ****************************************************************************/

DColorRangeTableEditor::DColorRangeTableEditor(QWidget* pParent /* = nullptr */) :
      QWidget(pParent)
   {
   m_pTable = new DColorRangeTableWidget(this);
   m_pButtonAdd = new QPushButton(tr("Add"), this);
   m_pButtonDelete = new QPushButton(tr("Delete"), this);
   m_pButtonEdit = new QPushButton(tr("Edit"), this);

   QGridLayout* pGrid = new QGridLayout;
   pGrid->addWidget(m_pTable, 0, 0, 1, 3);
   pGrid->addWidget(m_pButtonAdd, 1, 0);
   pGrid->addWidget(m_pButtonDelete, 1, 1);
   pGrid->addWidget(m_pButtonEdit, 1, 2);

   setLayout(pGrid);

   connect(m_pButtonAdd, SIGNAL(pressed()), m_pTable, SLOT(AddRow()));
   connect(m_pButtonDelete, SIGNAL(pressed()), m_pTable, SLOT(DeleteRow()));
   connect(m_pButtonEdit, SIGNAL(pressed()), m_pTable, SLOT(EditRow()));

   return;

   } // end of DColorRangeTableEditor::DColorRangeTableEditor

/*****************************************************************************
*
***  DColorRangeTableEditor::SetColorScheme
*
****************************************************************************/

void DColorRangeTableEditor::SetColorScheme(
      DBlobTracker::EColorScheme eColorScheme,
      QString strColorScheme, QStringList ColorComponentNames)
   {
   m_eColorScheme = eColorScheme;
   m_pTable->SetColorScheme(eColorScheme, strColorScheme,
         ColorComponentNames);

   return;

   } // end of method DColorRangeTableEditor::SetColorScheme

/*****************************************************************************
 ***  class DColorRangeRowEditDialog
 ****************************************************************************/

/*****************************************************************************
 *
 ***  DColorRangeRowEditDialog::DColorRangeRowEditDialog
 *
 * Provides a dialog box that allows editing a whole row in that format.  So
 * far that's how the display color has to be set because I can't find a good
 * way to handle that with a context popup directly from the cell.
 *
 ****************************************************************************/

DColorRangeRowEditDialog::DColorRangeRowEditDialog(
      DBlobTracker::EColorScheme eColorScheme,
      const QString& strColorScheme,
      const QStringList& ColorComponentNames,
      QWidget* pParent /* = nullptr */) :
      QDialog(pParent)
   {
   QString strTitle;
   strTitle = tr("Edit Blob Color Data (") + strColorScheme + ")";
   setWindowTitle(strTitle);

   QPushButton* pButtonOK = new QPushButton(tr("OK"));
   QPushButton* pButtonCancel = new QPushButton(tr("Cancel"));

   QHBoxLayout* pButtonLayout = new QHBoxLayout;
   pButtonLayout->addWidget(pButtonOK);
   pButtonLayout->addWidget(pButtonCancel);

   QVBoxLayout* pMainLayout = new QVBoxLayout;

   QGridLayout* pEditLayout = new QGridLayout;

   pMainLayout->addLayout(pEditLayout);
   pMainLayout->addLayout(pButtonLayout);
   setLayout(pMainLayout);

   m_pCircular1 = new QCheckBox(tr(""));

   int nRow = 0;
   for (int i = 0 ; i < 3 ; i++)
      {
      QLabel* pLabel = new QLabel(ColorComponentNames[i]);
      pLabel->setAlignment(Qt::AlignCenter);
      pEditLayout->addWidget(pLabel, nRow++, 0, 1, 2);

      pLabel = new QLabel(tr("Min"));
      m_pMin[i] = new QLineEdit(this);
      m_pMin[i]->setMaxLength(3);
      QIntValidator* pValidator = new QIntValidator(0, 255, this);
      m_pMin[i]->setValidator(pValidator);
      pLabel->setBuddy(m_pMin[i]);
      pEditLayout->addWidget(pLabel, nRow, 0);
      pEditLayout->addWidget(m_pMin[i], nRow++, 1);

      pLabel = new QLabel(tr("Max"));
      m_pMax[i] = new QLineEdit(this);
      m_pMax[i]->setMaxLength(3);
      pValidator = new QIntValidator(0, 255, this);
      m_pMax[i]->setValidator(pValidator);
      pLabel->setBuddy(m_pMax[i]);
      pEditLayout->addWidget(pLabel, nRow, 0);
      pEditLayout->addWidget(m_pMax[i], nRow++, 1);

      if ((i == 0) && ((eColorScheme == DBlobTracker::eHSV) ||
            (eColorScheme == DBlobTracker::eHSL)))
         {
         pLabel = new QLabel(tr("Circular"));
         pEditLayout->addWidget(pLabel, nRow, 0);
         pEditLayout->addWidget(m_pCircular1, nRow++, 1);
         } // end if
      } // end for

   QLabel* pLabel = new QLabel(tr("Display Color"));
   m_pDisplayColor = new DColorWidget(true);
   m_pDisplayColor->SetColor(QColor(255, 0, 0));
   pEditLayout->addWidget(pLabel, nRow, 0);
   pEditLayout->addWidget(m_pDisplayColor, nRow++, 1);

   connect(pButtonOK, SIGNAL(pressed()), this, SLOT(accept()));
   connect(pButtonCancel, SIGNAL(pressed()), this, SLOT(reject()));

   setFixedSize(QSize(300, 400));

   return;

   } // end of DColorRangeRowEditDialog::DColorRangeRowEditDialog
