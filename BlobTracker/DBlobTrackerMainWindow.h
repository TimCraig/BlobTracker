/*
 * DBlobTrackerMainWindow.h
 *
 *  Created on: May 25, 2014
 *      Author: Tim
 */

#ifndef DBLOBTRACKERMAINWINDOW_H_
#define DBLOBTRACKERMAINWINDOW_H_

/*****************************************************************************
******************************  I N C L U D E  ******************************
****************************************************************************/

#include "DCVCameraMainWindow.h"
#include "DBlobs.h"
#include "DQHistogramWidget.h"
#include "DColorRangeTableWidget.h"
#include "DBlobTracker.h"

#include <QAction>
#include <QLabel>
#include <QElapsedTimer>

/*****************************************************************************
***************************** class DBlobParams ******************************
*****************************************************************************/

class DBlobParams
   {
   public :
      static const unsigned int m_nVersion = 0;

      DBlobParams() : m_nColorSet{0}
         {
         return;
         }

      DBlobParams(const DBlobParams& src) = delete;

      ~DBlobParams() = default;

      DBlobParams& operator=(const DBlobParams& rhs) = delete;

      int GetColorSet() const
         {
         return (m_nColorSet);
         }

      void SetColorSet(int nColorSet)
         {
         m_nColorSet = nColorSet;

         return;
         }

      DBlobColor::Vector& GetColors()
         {
         return (m_Colors);
         }

      bool Read(const std::string& strPath);
      bool Write(const std::string& strPath) const;

      template<class Archive>
      void serialize(Archive & ar, const unsigned int /* nVersion */)
         {
         ar & boost::serialization::make_nvp("ColorSet", m_nColorSet);
         ar & boost::serialization::make_nvp("Colors", m_Colors);

         return;
         }

   protected :
      int m_nColorSet;
      DBlobColor::Vector m_Colors;

   private :

   };  // end of class DBlobParams

BOOST_CLASS_VERSION(DBlobParams, DBlobParams::m_nVersion)

/*****************************************************************************
*
***  class DBlobTrackerMainWindow
*
*****************************************************************************/

class DBlobTrackerMainWindow : public DCVCameraMainWindow
   {
   Q_OBJECT

   using Base = DCVCameraMainWindow;

   public:
       explicit DBlobTrackerMainWindow(QWidget* pParent  = nullptr);

      DBlobTrackerMainWindow(const DBlobTrackerMainWindow& src) = delete;

      ~DBlobTrackerMainWindow() = default;

      DBlobTrackerMainWindow& operator=(const DBlobTrackerMainWindow& rhs) = delete;

      virtual void Initialize(Qt::Orientation eOrientation = Qt::Vertical);

   protected:
      DColorBlobFinder* m_pBlobFinder;
      DColorRangeTableEditor* m_pColorTableEditor;
      mutable DBlobParams m_Params;
      bool m_bDisplayCategories;
      bool m_bDisplayLargest;
      bool m_bShowBoundingBox;
      bool m_bShowCrossHairs;
      int m_nBlobDisplay;
      QLabel* m_pBlobCount;
      QLabel* m_pProcessTime;
      QElapsedTimer m_Timer;

      QAction* m_pActionSaveBlobFile;
      QAction* m_pActionLoadBlobFile;

      QVector<DQHistogramWidget*> m_HistogramWidgets;
      QVector<QLabel*> m_HistogramLabels;
      QVector<QStringList> m_ColorComponentNames;
      QStringList m_ColorSchemeNames;

      virtual void SetupCentralWidget();
      virtual void SetupStatusBar();
      virtual void ProcessImage(DCVImage& Image);
      virtual void DisplayOutput();
      virtual QMenu* AddBlobMenu();
      virtual void CreateHistogramWidgets();
      virtual void SetColorScheme(int nColorScheme);
      virtual void CreateColorTable();
      virtual void SetupColorStrings();
      virtual void CameraStarted();
      virtual void CameraStopped();
      bool ExtractColorTable();
      void LoadColorTable();

      bool SaveBlobFile(const std::string& strPath);
      bool LoadBlobFile(const std::string& strPath);

   protected slots:
      virtual void UpdateHistograms();
      virtual void ChangeColorScheme(QAction* pAction);
      virtual void DisplayLargestAction(bool bChecked);
      virtual void ShowBoundingBoxAction(bool bChecked);
      virtual void ShowCrossHairsAction(bool bChecked);
      virtual void ReprocessImage();

      virtual void OnSaveBlobFile();
      virtual void OnLoadBlobFile();

      virtual bool LoadParameters() override;
      virtual bool SaveParameters() override;

      virtual void UpdateFileActions();

   private:

   }; // end of class DBlobTrackerMainWindow



#endif /* DBLOBTRACKERMAINWINDOW_H_ */
