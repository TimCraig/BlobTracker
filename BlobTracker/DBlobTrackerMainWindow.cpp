/*
 * DBlobTrackerMainWindow.cpp
 *
 *  Created on: May 25, 2014
 *      Author: Tim
 */

/*****************************************************************************
 ******************************  I N C L U D E  ******************************
 ****************************************************************************/

#include "DBlobTrackerMainWindow.h"
#include "DQCVImageUtils.h"

#include <QMessageBox>
#include <QStatusBar>
#include <QFileDialog>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <boost/config.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

/*****************************************************************************
 ***  class DBlobParams
 ****************************************************************************/

/******************************************************************************
*
***  DBlobParams::Read
*
* Read the blob parameters from a Boost Serialization XML Archive.
*
******************************************************************************/

bool DBlobParams::Read(const std::string& strPath)
   {
   std::ifstream ifs(strPath);
   bool bRet = ifs.good();
   if (bRet)
      {
      boost::archive::xml_iarchive ia(ifs);
      ia & boost::serialization::make_nvp("Blob_Params", *this);
      } // end if

   ifs.close();

   return (bRet);

   } // end of method DBlobParams::Read

/******************************************************************************
*
***  DBlobParams::Write
*
* Write the blob parameters to a Boost Serialization XML Archive.
*
******************************************************************************/

bool DBlobParams::Write(const std::string& strPath) const
   {
   std::ofstream ofs(strPath);
   bool bRet = ofs.good();
   if (bRet)
      {
      boost::archive::xml_oarchive oa(ofs);
      oa & boost::serialization::make_nvp("Blob_Params", *this);
      } // end if

   ofs.close();

   return (bRet);

   } // end of method DBlobParams::Write

/*****************************************************************************
 ***  class DBlobTrackerMainWindow
 ****************************************************************************/

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::DBlobTrackerMainWindow
 *
 ****************************************************************************/

DBlobTrackerMainWindow::DBlobTrackerMainWindow(QWidget* pParent /* = nullptr */) :
      Base(pParent),
      m_pBlobFinder(nullptr),
      m_pColorTableEditor(nullptr),
      m_pBlobCount(nullptr),
      m_pProcessTime(nullptr),
      m_pActionSaveBlobFile(nullptr),
      m_pActionLoadBlobFile(nullptr)
   {
   setGeometry(200, 200, 750, 800);

   m_Params.SetColorSet(DBlobTracker::eHSV);

   m_bDisplayCategories = false;
   m_bDisplayLargest = false;
   m_bShowBoundingBox = false;
   m_bShowCrossHairs = false;
   m_nBlobDisplay = DBlob::eBlob;

   m_strAppName = "DruaiBlobTracker";

   return;

   } // end of DBlobTrackerMainWindow::DBlobTrackerMainWindow

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::Initialize
 *
 ****************************************************************************/

void DBlobTrackerMainWindow::Initialize(
      Qt::Orientation eOrientation /* = Qt::Vertcial */)
   {
   Base::Initialize(eOrientation);

   SetupColorStrings();

   AddBlobMenu();

   SetupStatusBar();

   // Make the captured image valid until one is actually captured
   m_CapturedImage = DCVImage(m_ImageSize.width(), m_ImageSize.height(), CV_8UC3);
   m_CapturedImage.Clear();

   SetColorScheme(m_Params.GetColorSet());

   m_pCameraHandler->StartCamera();

   return;

   } // end of method DBlobTrackerMainWindow::Initialize

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::SetupCentralWidget
 *
 ****************************************************************************/

void DBlobTrackerMainWindow::SetupCentralWidget()
   {
   Base::SetupCentralWidget();

   CreateHistogramWidgets();

   CreateColorTable();

   // Setup to reprocess image if ROI changes
   connect(m_pInputImageWidget, SIGNAL(ROIChanged(const QRect&)), this, SLOT(UpdateHistograms()));

   return;

   } // end of method DBlobTrackerMainWindow::SetupCentralWidget

/******************************************************************************
*
***  DBlobTrackerMainWindow::SetupStatusBar()
*
******************************************************************************/

void DBlobTrackerMainWindow::SetupStatusBar()
   {
   QStatusBar* pSB = statusBar();

   QLabel* pTime = new QLabel(tr("Process Time:"));
   pSB->addPermanentWidget(pTime);
   m_pProcessTime = new QLabel(tr("9999"));
   m_pProcessTime->setMinimumSize(m_pProcessTime->sizeHint());
   m_pProcessTime->setAlignment(Qt::AlignRight);
   pSB->addPermanentWidget(m_pProcessTime);
   QLabel* pMS = new QLabel(tr("ms"));
   pSB->addPermanentWidget(pMS);

   QLabel* pBlobs = new QLabel(tr("Blob Count:"));
   pSB->addPermanentWidget(pBlobs);
   m_pBlobCount = new QLabel(tr("999999999999999999"));
   m_pBlobCount->setMinimumSize(m_pBlobCount->sizeHint());
   m_pBlobCount->setAlignment(Qt::AlignLeft);
   pSB->addPermanentWidget(m_pBlobCount);

   m_pBlobCount->setText("");
   m_pProcessTime->setText("");

   return;

   } // end of method DBlobTrackerMainWindow::SetupStatusBar

/*****************************************************************************
*
***  DBlobTrackerMainWindow::SetupColorStrings
*
****************************************************************************/

void DBlobTrackerMainWindow::SetupColorStrings()
   {
   m_ColorSchemeNames << tr("RGB") << tr("HSV") << tr("HSL");

   QStringList RGB;
   RGB << tr("Red") << tr("Green") << tr("Blue");
   QStringList HSV;
   HSV << tr("Hue") << tr("Saturation") << tr("Value");
   QStringList HSL;
   HSL << tr("Hue") << tr("Saturation") << tr("Lightness");

   m_ColorComponentNames.push_back(RGB);
   m_ColorComponentNames.push_back(HSV);
   m_ColorComponentNames.push_back(HSL);

   return;

   } // end of method DBlobTrackerMainWindow::SetupColorStrings

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::AddBlobMenu
 *
 ****************************************************************************/

QMenu* DBlobTrackerMainWindow::AddBlobMenu()
   {
   QMenu* pBlobMenu = GetMenuBar()->addMenu(tr("&Blob"));

   // Action Group for the color schemes
   QActionGroup* pColorGroup = new QActionGroup(this);
   pColorGroup->setExclusive(true);

   QAction* pColorAction = new QAction(tr("RGB"), pColorGroup);
   pColorAction->setCheckable(true);
   pColorAction->setData(QVariant(DBlobTracker::eRGB));

   pBlobMenu->addAction(pColorAction);
   pColorAction->setChecked(pColorAction->data().toInt() == m_Params.GetColorSet());

   pColorAction = new QAction(tr("HSV"), pColorGroup);
   pColorAction->setCheckable(true);
   pColorAction->setData(QVariant(DBlobTracker::eHSV));

   pBlobMenu->addAction(pColorAction);
   pColorAction->setChecked(pColorAction->data().toInt() == m_Params.GetColorSet());

   pColorAction = new QAction(tr("HSL"), pColorGroup);
   pColorAction->setCheckable(true);
   pColorAction->setData(QVariant(DBlobTracker::eHSL));

   pBlobMenu->addAction(pColorAction);
   pColorAction->setChecked(pColorAction->data().toInt() == m_Params.GetColorSet());

   connect(pColorGroup, SIGNAL(triggered(QAction*)),
         SLOT(ChangeColorScheme(QAction*)));

   pBlobMenu->addSeparator();

   // Menu items to "decorate" the blobs for easier viewing

   QAction* pActionBoundingBox = new QAction(tr("Bounding Boxes"), this);
   pActionBoundingBox->setStatusTip(tr("Show the Blob Bounding Boxes"));
   pActionBoundingBox->setCheckable(true);
   pBlobMenu->addAction(pActionBoundingBox);
   connect(pActionBoundingBox, SIGNAL(toggled(bool)),
         SLOT(ShowBoundingBoxAction(bool)));

   QAction* pActionCrossHairs = new QAction(tr("Cross Hairs"), this);
   pActionCrossHairs->setStatusTip(tr("Show Cross Hairs on Blob Centroid"));
   pActionCrossHairs->setCheckable(true);
   pBlobMenu->addAction(pActionCrossHairs);
   connect(pActionCrossHairs, SIGNAL(toggled(bool)), SLOT(ShowCrossHairsAction(bool)));

   QAction* pActionLargestBlob = new QAction(tr("Largest Blob"), this);
   pActionLargestBlob->setStatusTip(tr("Show Only the Largest Blob"));
   pActionLargestBlob->setCheckable(true);
   pBlobMenu->addAction(pActionLargestBlob);
   connect(pActionLargestBlob, SIGNAL(toggled(bool)), SLOT(DisplayLargestAction(bool)));

   // Add save and load blob file to the File menu
   m_pActionSaveBlobFile = new QAction(tr("Save Blob File..."), this);
   m_pActionSaveBlobFile->setEnabled(false);
   m_pFileMenu->insertAction(m_pActionExit, m_pActionSaveBlobFile);
   connect(m_pActionSaveBlobFile, SIGNAL(triggered()), SLOT(OnSaveBlobFile()));

   m_pActionLoadBlobFile = new QAction(tr("Load Blob File..."), this);
   m_pActionLoadBlobFile->setEnabled(false);
   m_pFileMenu->insertAction(m_pActionExit, m_pActionLoadBlobFile);
   connect(m_pActionLoadBlobFile, SIGNAL(triggered()), SLOT(OnLoadBlobFile()));

   m_pFileMenu->insertSeparator(m_pActionExit);


   return (pBlobMenu);

   } // end of method DBlobTrackerMainWindow::AddBlobMenu

/*****************************************************************************
*
***  DBlobTrackerMainWindow::UpdateFileActions
*
* Some file actions are inappropriate when the camera is running.  Enable or
* disable based on the camera state.
*
****************************************************************************/

void DBlobTrackerMainWindow::UpdateFileActions()
   {
   Base::UpdateFileActions();

   bool bEnable = !m_pCameraHandler->IsRunning();

   m_pActionSaveBlobFile->setEnabled(bEnable);
   m_pActionLoadBlobFile->setEnabled(bEnable);

   return;

   } // end of method DBlobTrackerMainWindow::UpdateFileActions

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::CreateHistogramWidgets
 *
 ****************************************************************************/

void DBlobTrackerMainWindow::CreateHistogramWidgets()
   {
   QVBoxLayout* pHistogramBox = new QVBoxLayout();

   for (int i = 0 ; i < 3 ; i++)
      {
      QHBoxLayout* pHBox = new QHBoxLayout();

      // Create a label for each histogram
      QLabel* pLabel = new QLabel(this);
      pLabel->setStyleSheet("QLabel{font: bold 20px}");
      m_HistogramLabels.push_back(pLabel);
      pHBox->addWidget(pLabel);

      // Create the Histogram Widgets and insert into the grid
      DQHistogramWidget* pHistogramWidget = new DQHistogramWidget(
            qRgb(0, 0, 0));
      m_HistogramWidgets.push_back(pHistogramWidget);
      pHBox->addWidget(pHistogramWidget);

      pHistogramBox->addLayout(pHBox);
      } // end for

   m_pMainGrid->addLayout(pHistogramBox, 0, 1, 2, 1);

   return;

   } // end of method DBlobTrackerMainWindow::CreateHistogramWidgets

/*****************************************************************************
*
***  DBlobTrackerMainWindow::CreateColorTable
*
****************************************************************************/

void DBlobTrackerMainWindow::CreateColorTable()
   {
   m_pColorTableEditor = new DColorRangeTableEditor(this);
   m_pMainGrid->addWidget(m_pColorTableEditor, 2, 0, 1, 2);

   // If we automatically loaded a saved parameter file, display it
   if (m_Params.GetColors().size() > 0)
      {
      LoadColorTable();
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::CreateColorTable

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::ChangeColorScheme
 *
 *  Handle the menu selections for changing the color scheme in the 3
 *  alternative color plane widgets.
 *
 ****************************************************************************/

void DBlobTrackerMainWindow::ChangeColorScheme(QAction* pAction)
   {
   int nColorScheme = pAction->data().toInt();
   if (nColorScheme != m_Params.GetColorSet())
      {
      SetColorScheme(nColorScheme);

      // User changed the color scheme so reprocess the image
      if (!m_pCameraHandler->IsRunning())
         {
         // If camera stopped, update the output display
         ProcessImage(m_CapturedImage);
         } // end if
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::ChangeColorScheme

/******************************************************************************
*
***  DBlobTrackerMainWindow::DisplayLargestAction
*
* Handle menu item for displaying the largest blob.
*
******************************************************************************/

void DBlobTrackerMainWindow::DisplayLargestAction(bool bChecked)
   {
   m_bDisplayLargest = bChecked;

   if (!m_pCameraHandler->IsRunning())
      {
      // If camera stopped, update the output display
      DisplayOutput();
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::DisplayLargestAction

/******************************************************************************
*
***  DBlobTrackerMainWindow::ShowBoundingBoxAction
*
* Handle the menu item for showing blob bounding boxes.
*
******************************************************************************/

void DBlobTrackerMainWindow::ShowBoundingBoxAction(bool bChecked)
   {
   m_bShowBoundingBox = bChecked;

   if (m_bShowBoundingBox)
      {
      m_nBlobDisplay |= DBlob::eBoundingRect;
      } // end if
   else
      {
      m_nBlobDisplay &= ~DBlob::eBoundingRect;
      } // end else

   if (!m_pCameraHandler->IsRunning())
      {
      // If camera stopped, update the output display
      DisplayOutput();
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::ShowBoundingBoxAction

/******************************************************************************
*
***  DBlobTrackerMainWindow::ShowCrossHairsAction
*
* Handle menu item for displaying cross hairs at the blob centroid.
*
******************************************************************************/

void DBlobTrackerMainWindow::ShowCrossHairsAction(bool bChecked)
   {
   m_bShowCrossHairs = bChecked;

   if (m_bShowCrossHairs)
      {
      m_nBlobDisplay |= DBlob::eCrossHairs;
      } // end if
   else
      {
      m_nBlobDisplay &= ~DBlob::eCrossHairs;
      } // end else

   if (!m_pCameraHandler->IsRunning())
      {
      // If camera stopped, update the output display
      DisplayOutput();
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::ShowCrossHairsAction

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::SetColorScheme
 *
 ****************************************************************************/

void DBlobTrackerMainWindow::SetColorScheme(int nColorScheme)
   {
   m_Params.SetColorSet(nColorScheme);

   if (nColorScheme == DBlobTracker::eRGB)
      {
      delete m_pBlobFinder;
      m_pBlobFinder = new DColorBlobFinder(&m_Params.GetColors());

      m_HistogramWidgets[0]->SetLineColor(qRgb(255, 0, 0));
      m_HistogramWidgets[1]->SetLineColor(qRgb(0, 255, 0));
      m_HistogramWidgets[2]->SetLineColor(qRgb(0, 0, 255));
      } // end if
   else if (nColorScheme == DBlobTracker::eHSV)
      {
      delete m_pBlobFinder;
      m_pBlobFinder = new DColorBlobFinderHSV(&m_Params.GetColors());

      m_HistogramWidgets[0]->SetLineColor(qRgb(255, 0, 255));
      m_HistogramWidgets[1]->SetLineColor(qRgb(0, 255, 255));
      m_HistogramWidgets[2]->SetLineColor(qRgb(0, 0, 0));
      } // end else if
   else if (nColorScheme == DBlobTracker::eHSL)
      {
      delete m_pBlobFinder;
      m_pBlobFinder = new DColorBlobFinderHSL(&m_Params.GetColors());

      m_HistogramWidgets[0]->SetLineColor(qRgb(255, 0, 255));
      m_HistogramWidgets[1]->SetLineColor(qRgb(0, 255, 255));
      m_HistogramWidgets[2]->SetLineColor(qRgb(0, 0, 0));
      } // end else if

   QStringList Names = m_ColorComponentNames[nColorScheme];
   m_HistogramLabels[0]->setText(Names[0]);
   m_HistogramLabels[1]->setText(Names[1]);
   m_HistogramLabels[2]->setText(Names[2]);

   m_pColorTableEditor->SetColorScheme(
         static_cast<DBlobTracker::EColorScheme>(m_Params.GetColorSet()),
         m_ColorSchemeNames[nColorScheme],
         m_ColorComponentNames[nColorScheme]);

   return;

   } // end of method DBlobTrackerMainWindow::SetColorScheme

/******************************************************************************
*
***  DBlobTrackerMainWindow::CameraStarted
*
******************************************************************************/

void DBlobTrackerMainWindow::CameraStarted()
   {
   // Get what's currently in the color table
   if (ExtractColorTable())
      {
      // Prohibit editing the color table while running
      m_pColorTableEditor->setEnabled(false);

      Base::CameraStarted();
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::CameraStarted

/******************************************************************************
*
***  DBlobTrackerMainWindow::CameraStopped
*
******************************************************************************/

void DBlobTrackerMainWindow::CameraStopped()
   {
   Base::CameraStopped();
\
   // Let them edit away
   m_pColorTableEditor->setEnabled(true);

   return;

   } // end of method DBlobTrackerMainWindow::CameraStopped

/******************************************************************************
*
***  DBlobTrackerMainWindow::ExtractColorTable
*
* Extract the data from the Color Table Editor Widget and store them in the
* Blob parameters object for processing.
*
******************************************************************************/

bool DBlobTrackerMainWindow::ExtractColorTable()
   {
   bool bRet = true;

   // Just recreate the table, it's easier
   m_Params.GetColors().clear();

   DColorRangeTableWidget* pTable = m_pColorTableEditor->GetColorTable();

   // Not currently using
   QString strBlobName;

   for (int r = DColorRangeTableWidget::eBackgroundRow ; bRet && (r < pTable->rowCount()) ; r++)
      {
      int nValues[6];
      int nVal = 0;
      bool bCircular = false;
//      strBlobName = m_pColorGrid->GetRowLabelValue(r);

      for (int c = DColorRangeTableWidget::eMin1 ;
            bRet && (c <= DColorRangeTableWidget::eMax3) ; c++)
         {
         QTableWidgetItem* pItem = pTable->item(r, c);
         if (pItem != nullptr)
            {
            if (c == DColorRangeTableWidget::eCirc1)
               {
               bCircular = (pItem->checkState() == Qt::Checked);
               } // end if
            else
               {
               nValues[nVal++] = pItem->text().toInt(&bRet);
               } // end else
            } // end if
         else
            {
            bRet = false;
            } // end else
         } // end for

      QTableWidgetItem* pColor = pTable->item(r, DColorRangeTableWidget::eDispColor);
      QColor DisplayColor;
      if (pColor != nullptr)
         {
         DisplayColor = pColor->backgroundColor();
         } // end if

      if (bRet)
         {
         if (bCircular && (nValues[0] < nValues[1]))
            {
            std::swap(nValues[0], nValues[1]);
            } // end if

         DBlob::DBlobColorRange::RangeTest Test1(nValues[0], nValues[1],
               bCircular);
         DBlob::DBlobColorRange::RangeTest Test2(nValues[2], nValues[3]);
         DBlob::DBlobColorRange::RangeTest Test3(nValues[4], nValues[5]);

         DBlob::DBlobColorRange ColorRange(Test1, Test2, Test3);

         m_Params.GetColors().push_back(DBlobColor(strBlobName.toStdString(),
               ColorRange, DCVRGB(DisplayColor)));
         } // end if
      } // end for

   if (!bRet)
      {
      QMessageBox::critical(this, tr("Error"), tr("Failure to extract color table"), QMessageBox::Ok);
      } // end if

   return (bRet);

   } // end of method DBlobTrackerMainWindow::ExtractColorTable

/******************************************************************************
*
***  DBlobTrackerMainWindow::LoadColorTable
*
* Transfer a set of blob colors to the ColorRangeTableWidget for display and
* editing.
*
******************************************************************************/

void DBlobTrackerMainWindow::LoadColorTable()
   {
   DColorRangeTableWidget* pTable = m_pColorTableEditor->GetColorTable();
   pTable->LoadColorTable(m_Params.GetColors());

   return;

   } // end of method DBlobTrackerMainWindow::LoadColorTable

/******************************************************************************
*
***  DBlobTrackerMainWindow::ReprocessImage
*
* Handle reprocessing the last image.  Only applicable when the camera is
* stopped.
*
******************************************************************************/

void DBlobTrackerMainWindow::ReprocessImage()
   {
   // Extract the color table in case the user modified it
   if (ExtractColorTable())
      {
      Base::ReprocessImage();
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::ReprocessImage

/******************************************************************************
*
***  DBlobTrackerMainWindow::SaveParameters()
*
* Save the program parameters.  In this case, the Blob Color Table.
*
******************************************************************************/

bool DBlobTrackerMainWindow::SaveParameters()
   {

   return (SaveBlobFile(GetParameterPath("xml").toStdString()));

   } // end of method DBlobTrackerMainWindow::SaveParameters

/******************************************************************************
*
***  DBlobTrackerMainWindow::LoadParameters()
*
* Load the program parameters.  In this case, the Blob Color Table.
*
******************************************************************************/

bool DBlobTrackerMainWindow::LoadParameters()
   {

   return (m_Params.Read(GetParameterPath("xml").toStdString()));

   } // end of method DBlobTrackerMainWindow::LoadParameters()

/******************************************************************************
*
***  DBlobTrackerMainWindow::OnSaveBlobFile
*
* Handle the menu item for saving the Blob Color Table to file.
*
******************************************************************************/

void DBlobTrackerMainWindow::OnSaveBlobFile()
   {
   QString strFileName = QFileDialog::getSaveFileName(this,
         tr("Save Blob File As"), QDir::currentPath(),
         tr("Blob files (*.xml)"));
   if (!strFileName.isEmpty())
      {
      SaveBlobFile(strFileName.toStdString());
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::OnSaveBlobFile

/******************************************************************************
*
***  DBlobTrackerMainWindow::OnLoadBlobFile
*
* Handle the menu item for loading the Blob Color Table from file.
*
******************************************************************************/

void DBlobTrackerMainWindow::OnLoadBlobFile()
   {
   QString strFileName = QFileDialog::getOpenFileName(this,
         tr("Load Blob files (*.xml)"));

   if (!strFileName.isEmpty())
      {
      if (!LoadBlobFile(strFileName.toStdString()))
         {
         QString strMsg(tr("Failed to load blob file "));
         strMsg += strFileName;
         QMessageBox::warning(this, tr("Load Blob File"), strMsg);
         } // end if
      } // end if

   return;

   } // end of method DBlobTrackerMainWindow::OnSaveLoadFile

/******************************************************************************
*
***  DBlobTrackerMainWindow::SaveBlobFile
*
* Handle the mechanics of actually saving the Blob Color Table to file.
*
******************************************************************************/

bool DBlobTrackerMainWindow::SaveBlobFile(const std::string& strPath)
   {
   bool bRet = ExtractColorTable();

   if (bRet)
      {
      bRet = m_Params.Write(strPath);
      } // end if

   return (bRet);

   } // end of method DBlobTrackerMainWindow::SaveBlobFile

/******************************************************************************
*
***  DBlobTrackerMainWindow::LoadBlobFile
*
* Handle the mechanics of actually loading the Blob Color Table from file.
*
******************************************************************************/

bool DBlobTrackerMainWindow::LoadBlobFile(const std::string& strPath)
   {
   bool bRet = m_Params.Read(strPath);

   if (bRet)
      {
      LoadColorTable();
      ReprocessImage();
      } // end if

   return (bRet);

   } // end of method DBlobTrackerMainWindow::LoadBlobFile

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::ProcessImage
 *
 ****************************************************************************/

void DBlobTrackerMainWindow::ProcessImage(DCVImage& Image)
   {
   m_CapturedImage = Image;

   // Display the input image or camera capture in the input widget
   DCVImage Display = Image.clone();
   DQImage InputImage = cvMatToQImage(Display);
   InputImage.SetROI(m_pInputImageWidget->GetImage().GetROI());
   m_pInputImageWidget->SetImage(InputImage);

   m_Timer.start();
   m_pBlobFinder->Find(&m_CapturedImage);
   qint64 nElapsed = m_Timer.elapsed();

   m_pProcessTime->setText(QString().setNum(nElapsed));

//   int nBlobs = m_pBlobFinder->GetBlobs().size();

   DisplayOutput();

   UpdateHistograms();

   return;

   } // end of method DBlobTrackerMainWindow::ProcessImage

/******************************************************************************
*
***  DBlobTrackerMainWindow::DisplayOutput
*
******************************************************************************/

void DBlobTrackerMainWindow::DisplayOutput()
   {
   DQImage OutputImage(320, 240, QImage::Format_RGB32);
   DQIBlobRenderer Renderer;

   // Render directly to the QImage for display
   Renderer.SetImage(&OutputImage);

// Uncomment to debug the categorized image
//   m_bDisplayCategories = true;
   if (m_bDisplayCategories)
      {
      m_pBlobFinder->DisplayCategories(&Renderer);
      } // end if
   else
      {
      m_pBlobFinder->DisplayBackground(&Renderer);

      if (m_bDisplayLargest)
         {
         m_pBlobFinder->DisplayLargestBlobs(&Renderer, m_nBlobDisplay,
               DCVRGB(255, 255, 255), 2);
         } // end if
      else
         {
         m_pBlobFinder->DisplayBlobs(&Renderer, m_nBlobDisplay,
               DCVRGB(255, 255, 255), 2);
         } // end else
      } // end else

   Renderer.End();

//   DQImage OutputImage = cvMatToQImage(OutputImage);
   m_pOutputImageWidget->SetImage(OutputImage);

   // Handle the status bar process message
   const DBlobs& Blobs = m_pBlobFinder->GetBlobs();
//   int nBlobs = Blobs.size();

   QString strBlobCount;
   if (Blobs.size() < 2)
     {
      // Only background blob found. (Zero would be an error)
     strBlobCount = "0";
      } // end if
   else
      {
      // Build a string showing the number of blobs in each category
      strBlobCount.setNum(Blobs[1].size());

      for (size_t i = 2 ; i < Blobs.size() ; i++)
         {
         QString strBlob;
         strBlobCount += ", ";
         strBlob.setNum(Blobs[i].size());
         strBlobCount += strBlob;
         } // end for
      } // end else
   m_pBlobCount->setText(strBlobCount);

   return;

   } // end of method DBlobTrackerMainWindow::DisplayOutput

/*****************************************************************************
 *
 ***  DBlobTrackerMainWindow::UpdateHistograms
 *
 ****************************************************************************/

void DBlobTrackerMainWindow::UpdateHistograms()
   {
   DQImage& Image = m_pInputImageWidget->GetImage();

   if (m_Params.GetColorSet() == DBlobTracker::eRGB)
      {
      Image.RedChannel().CalcHistogram(m_HistogramWidgets[0]->GetHistogram());
      Image.GreenChannel().CalcHistogram(m_HistogramWidgets[1]->GetHistogram());
      Image.BlueChannel().CalcHistogram(m_HistogramWidgets[2]->GetHistogram());

      m_pColorTableEditor->GetColorTable()->hideColumn(2);
      } // end if
   else if (m_Params.GetColorSet() == DBlobTracker::eHSV)
      {
      Image.HSVHue8Channel().CalcHistogram(m_HistogramWidgets[0]->GetHistogram());
      Image.HSVSaturationChannel().CalcHistogram(m_HistogramWidgets[1]->GetHistogram());
      Image.HSVValueChannel().CalcHistogram(m_HistogramWidgets[2]->GetHistogram());

      m_pColorTableEditor->GetColorTable()->showColumn(2);
      } // end else if
   else if (m_Params.GetColorSet() == DBlobTracker::eHSL)
      {
      Image.HSLHue8Channel().CalcHistogram(m_HistogramWidgets[0]->GetHistogram());
      Image.HSLSaturationChannel().CalcHistogram(m_HistogramWidgets[1]->GetHistogram());
      Image.HSLLightnessChannel().CalcHistogram(m_HistogramWidgets[2]->GetHistogram());

      m_pColorTableEditor->GetColorTable()->showColumn(2);
      } // end else if

   update();

   return;

      } // end of method DBlobTrackerMainWindow::UpdateHistograms
