/*****************************************************************************
 ********************************* DBlobs.h ***********************************
 *****************************************************************************/

#if !defined(__DBLOBS_H__)
#define __DBLOBS_H__

#pragma once

/*****************************************************************************
 ******************************  I N C L U D E  *******************************
 *****************************************************************************/

//#include "DConfig.h"
#include <QPainter>
#include <QDebug>

#include "DMath.h"
#include "DMatrix.h"
#include "CVImage.h"
#include "DQImage.h"

#include <vector>
#include <map>
#include <string>
//#include <boost/array.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "DQOpenCV.h"

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/version.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

/*****************************************************************************
 *
 ***  class DBlobRenderer
 *
 *  This class is an attempt to keep the Blob Library code free of specific
 *  GUI class code for portability while providing a standard rendering
 *  mechanism for displaying blob data visually.  This class is pure virtual
 *  to force the developer to provide a concrete class if rendering is
 *  desired.  This keeps the library code only dependent on OpenCV and not
 *  any GUI application framework such as Qt, wxWindows, or MFC.
 *
 *****************************************************************************/

class DBlobRenderer
   {
   public:
      DBlobRenderer() = default;

      DBlobRenderer(DBlobRenderer& src) = delete;

      virtual ~DBlobRenderer() = default;

      DBlobRenderer& operator=(DBlobRenderer& rhs) = delete;

      virtual void Point(int nRow, int nCol, cv::Scalar Color) = 0;
      virtual void Line(int nX1, int nY1, int nX2, int nY2,
            const cv::Scalar& Color, int nThickness = 1) = 0;
      virtual void Circle(int nXCenter, int nYCenter, int nRadius,
            const cv::Scalar& Color, int nThickness = 1) = 0;
      virtual void Rectangle(int xTopLeft, int yTopLeft, int nWidth,
            int nHeight, const cv::Scalar& Color, int nThickness = 1) = 0;

      virtual void CrossHairs(int nXCenter, int nYCenter, int nDiameter,
            const cv::Scalar& Color, bool bCircle = true, int nThickness = 1)
         {
         int nHS = nDiameter / 2;
         // Horizontal hair
         Line((nXCenter - nHS), nYCenter, (nXCenter + nHS), nYCenter, Color,
               nThickness);
         // Vertical hair
         Line(nXCenter, (nYCenter - nHS), nXCenter, (nYCenter + nHS), Color,
               nThickness);

         if (bCircle)
            {
            Circle(nXCenter, nYCenter, nHS, Color, nThickness);
            } // end if

         return;
         }

      virtual void ClearBackground(const cv::Scalar& Color) = 0;

   protected:

   private:

   }; // end of class DBlobRenderer

/*****************************************************************************
 *
 ***  class DCVBlobRenderer
 *
 *  Reference example of a concrete DBlobRenderer for rendering blobs on a
 *  DCVImage (cv::Mat OpenCV Image).
 *
 *****************************************************************************/

class DCVBlobRenderer : public DBlobRenderer
   {
   public:
      DCVBlobRenderer() :
            m_pImage(nullptr)
         {
         return;
         }

      DCVBlobRenderer(DCVImage* pImage) :
            m_pImage(pImage)
         {
         return;
         }

      DCVBlobRenderer(const DCVBlobRenderer& src) = delete;

      ~DCVBlobRenderer() = default;

      DCVBlobRenderer& operator=(const DCVBlobRenderer& rhs) = delete;

      void SetImage(DCVImage* pImage)
         {
         m_pImage = pImage;
         return;
         }

      DCVImage* GetImage() const
         {
         return (m_pImage);
         }

      virtual void Point(int nRow, int nCol, cv::Scalar Color)
         {
         if (m_pImage != nullptr)
            {
            unsigned char* pPixel = m_pImage->GetPixel(nRow, nCol);
            for (int i = 0 ; i < m_pImage->GetPixelSize() ; i++)
               {
               pPixel[i] = static_cast<unsigned char>(Color[i]);
               } // end for

            } // end if
         return;
         }

      virtual void Line(int nX1, int nY1, int nX2, int nY2,
            const cv::Scalar& Color, int nThickness = 1)
         {
         if (m_pImage != nullptr)
            {
            m_pImage->Line(cv::Point(nX1, nY1), cv::Point(nX2, nY2), Color,
                  nThickness);
            } // end if
         return;
         }

      virtual void Circle(int nXCenter, int nYCenter, int nRadius,
            const cv::Scalar& Color, int nThickness = 1)
         {
         if (m_pImage != nullptr)
            {
            m_pImage->Circle(cv::Point(nXCenter, nYCenter), nRadius, Color,
                  nThickness);
            } // end if
         return;
         }

      virtual void Rectangle(int xTopLeft, int yTopLeft, int nWidth,
            int nHeight, const cv::Scalar& Color, int nThickness = 1)
         {
         if (m_pImage != nullptr)
            {
            m_pImage->Rectangle(cv::Rect(xTopLeft, yTopLeft, nWidth, nHeight),
                  Color, nThickness);
            } // end if
         return;
         }

      virtual void ClearBackground(const cv::Scalar& Color)
         {
         if (m_pImage != nullptr)
            {
            *m_pImage = Color;
            } // end if
         return;
         }

   protected:
      DCVImage* m_pImage;

   private:

   }; // end of class DCVBlobRenderer

/*****************************************************************************
*
***  class DQIBlobRenderer
*
*  Reference example of a concrete DBlobRenderer for rendering blobs on a
*  DQImage (Qt QImage).
*
*****************************************************************************/

class DQIBlobRenderer : public DBlobRenderer
   {
   public:
      DQIBlobRenderer() :
         m_pImage(nullptr)
         {
         return;
         }

      DQIBlobRenderer(const DQIBlobRenderer& src) = delete;

      ~DQIBlobRenderer() = default;

      DQIBlobRenderer& operator=(const DQIBlobRenderer& rhs) = delete;

      void SetImage(DQImage* pImage)
         {
         m_pImage = pImage;
         m_Painter.begin(pImage);
         return;
         }

      DQImage* GetImage() const
         {
         return (m_pImage);
         }

      virtual void Point(int nRow, int nCol, cv::Scalar Color)
         {
         if (m_pImage != nullptr)
            {
            m_Painter.setPen(QColor(Color[2], Color[1], Color[0]));
            m_Painter.drawPoint(nCol, nRow);
            } // end if
         return;
         }

      virtual void Line(int nX1, int nY1, int nX2, int nY2,
            const cv::Scalar& Color, int nThickness = 1)
         {
         if (m_pImage != nullptr)
            {
            QPen Pen(QColor(Color[2], Color[1], Color[0]));
            Pen.setWidth(nThickness);
            m_Painter.setPen(Pen);
            m_Painter.drawLine(nX1, nY1, nX2, nY2);
            } // end if
         return;
         }

       virtual void Circle(int nXCenter, int nYCenter, int nRadius,
            const cv::Scalar& Color, int nThickness = 1)
         {
         if (m_pImage != nullptr)
            {
            QPen Pen(QColor(Color[2], Color[1], Color[0]));
            Pen.setWidth(nThickness);
            m_Painter.setPen(Pen);
            m_Painter.drawEllipse(QPoint(nXCenter, nYCenter), nRadius, nRadius);
            } // end if
         return;
         }

        virtual void Rectangle(int xTopLeft, int yTopLeft, int nWidth,
               int nHeight, const cv::Scalar& Color, int nThickness = 1)
            {
            if (m_pImage != nullptr)
               {
               QPen Pen(QColor(Color[2], Color[1], Color[0]));
               Pen.setWidth(nThickness);
               m_Painter.setPen(Pen);
               m_Painter.drawRect(xTopLeft, yTopLeft, nWidth, nHeight);                   ;
               } // end if
            return;
            }

      virtual void ClearBackground(const cv::Scalar& Color)
         {
         if (m_pImage != nullptr)
            {
            m_Painter.fillRect(m_pImage->rect(), QColor(Color[2], Color[1], Color[0]));
            } // end if
         return;
         }

      void End()
         {
         m_Painter.end();
         return;
         }

   protected:
      DQImage* m_pImage;
      QPainter m_Painter;

   private:

   }; // end of class DQIBlobRenderer

/*****************************************************************************
 ***************************** class DColorRange ******************************
 *****************************************************************************/

/*
 This class provides the basis for categorizing colors based on ranges
 in the three color axes.  The meaning of the three axes depends on the
 particular color space used.
 */

template <typename T>
class DColorRange
   {
   public:
      static const unsigned int m_nVersion = 0;

      using RangeTest = DRangeTest<T>;

      DColorRange() = default;

      DColorRange(const RangeTest& Color1, const RangeTest& Color2,
            const RangeTest& Color3) :
            m_Color1(Color1), m_Color2(Color2), m_Color3(Color3)
         {
         return;
         }

      DColorRange(T C1Low, T C1High, T C2Low, T C2High, T C3Low, T C3High) :
            m_Color1(C1Low, C1High), m_Color2(C2Low, C2High), m_Color3(C3Low,
                  C3High)
         {
         return;
         }

      DColorRange(const DColorRange& src)
         {
         m_Color1 = src.m_Color1;
         m_Color2 = src.m_Color2;
         m_Color3 = src.m_Color3;
         return;
         }

      ~DColorRange() = default;

      DColorRange& operator=(const DColorRange& rhs)
         {
         if (this != &rhs)
            {
            m_Color1 = rhs.m_Color1;
            m_Color2 = rhs.m_Color2;
            m_Color3 = rhs.m_Color3;
            } // end if

         return (*this);
         }

      bool Apply(const T Pixel[3])
         {
         return (m_Color1.Apply(Pixel[0]) && m_Color2.Apply(Pixel[1])
               && m_Color3.Apply(Pixel[2]));
         }

      const RangeTest& GetColor1() const
         {
         return (m_Color1);
         }

      const RangeTest& GetColor2() const
         {
         return (m_Color2);
         }

      const RangeTest& GetColor3() const
         {
         return (m_Color3);
         }

      RangeTest& GetColor1()
         {
         return (m_Color1);
         }

      RangeTest& GetColor2()
         {
         return (m_Color2);
         }

      RangeTest& GetColor3()
         {
         return (m_Color3);
         }

//      friend class boost::serialization::access;
//      friend std::ostream & operator<<(std::ostream &os, const DColorRange<T>& gp);

      template<class Archive>
      void serialize(Archive & ar, const unsigned int /* nVersion */)
         {
         ar & boost::serialization::make_nvp("Color1", m_Color1);
         ar & boost::serialization::make_nvp("Color2", m_Color2);
         ar & boost::serialization::make_nvp("Color3", m_Color3);

         return;
         }

   protected:
      RangeTest m_Color1;
      RangeTest m_Color2;
      RangeTest m_Color3;

   private:
   }; // end of class DColorRange

/* Handle the version information */
//BOOST_CLASS_VERSION(DColorRange, DColorRange::m_nVersion)

namespace boost
{
namespace serialization
{
template<typename T>
struct version<DColorRange<T>>
   {
      using type = mpl::int_<DColorRange<T>::m_nVersion>;
      using tag = mpl::integral_c_tag;
      BOOST_STATIC_CONSTANT(unsigned int, value = version::type::value);
      //      enum { value = 0 };
   };

template<typename T>
const unsigned int version<DColorRange<T>>::value;

} // end namespace serialization
} // end namespace boost

/*****************************************************************************
 *
 ***  class DRun
 *
 * Identifies a continuous run (line segment) of similar pixels in a
 * row (horizontal scanline).
 *
 *****************************************************************************/

class DRun
   {
   public:
      DRun() : m_nBlob(-1), m_nRow(-1), m_nStart(-1), m_nEnd(-1), m_nColor(-1)
         {
         return;
         }

      DRun(int nRow, int nStart, int nEnd, int nColor, int nBlob = -1) :
            m_nBlob(nBlob),
            m_nRow(nRow),
            m_nStart(nStart),
            m_nEnd(nEnd),
            m_nColor(nColor)
         {
         return;
         }

      DRun(const DRun& src)
         {
         Copy(src);
         return;
         }

      ~DRun() = default;

      DRun& operator=(const DRun& rhs)
         {
         if (this != &rhs)
            {
            Copy(rhs);
            } // end if
         return (*this);
         }

      void Copy(const DRun& src)
         {
         m_nBlob = src.m_nBlob;
         m_nRow = src.m_nRow;
         m_nStart = src.m_nStart;
         m_nEnd = src.m_nEnd;
         m_nColor = src.m_nColor;

         return;
         }

      int GetBlob() const
         {
         return (m_nBlob);
         }

      void SetBlob(int nBlob)
         {
         m_nBlob = nBlob;
         return;
         }

      int GetStart() const
         {
         return (m_nStart);
         }

      int GetEnd() const
         {
         return (m_nEnd);
         }

      int GetRow() const
         {
         return (m_nRow);
         }

      int GetColor() const
         {
         return (m_nColor);
         }

      void SetColor(int nColor)
         {
         m_nColor = nColor;
         return;
         }

      // Test if this run overlaps the passed Run.
      bool Overlap(const DRun& Run) const
         {
         return (((m_nStart >= Run.m_nStart) && (m_nStart <= Run.m_nEnd))
               || ((m_nEnd >= Run.m_nStart) && (m_nEnd <= Run.m_nEnd))
               || ((Run.m_nStart >= m_nStart) && (Run.m_nStart <= m_nEnd))
               || ((Run.m_nEnd >= m_nStart) && (Run.m_nEnd <= m_nEnd)));
         }

   protected:
      int m_nBlob;
      int m_nRow;
      int m_nStart;
      int m_nEnd;
      int m_nColor;

   private:

   }; // end of class DRun

/*****************************************************************************
 *
 ***  class DBlob
 *
 * Class for the properties of a single blob (a contiguous area of
 * similar color).
 *
 *****************************************************************************/

class DBlob
   {
   public:
      using DBlobColorRange = DColorRange<unsigned char>;
      using DIDVector = std::vector<int>;

      // Helper flags for the display functions
      enum EDisplay
         {
         eBlob = 1, eBoundingRect = 2, eCrossHairs = 4, eAll = eBlob | eBoundingRect | eCrossHairs,
         };

      DBlob();

      DBlob(int nID, int nType);

      DBlob(const DBlob& src)
         {
         Copy(src);

         return;
         }

      ~DBlob() = default;

      DBlob& operator=(const DBlob& rhs)
         {
         if (this != &rhs)
            {
            Copy(rhs);
            } // end if

         return (*this);
         }

      void AddRun(DRun& Run);
      void Merge(DBlob& Blob);

      int GetID() const
         {
         return (m_nID);
         }

      double GetArea() const
         {
         return (m_Moments.m00);
         }

      double GetCentroidX() const
         {
         return ((m_Moments.m00 > 0.0) ? m_Moments.m10 / m_Moments.m00 : 0.0);
         }

      double GetCentroidY() const
         {
         return ((m_Moments.m00 > 0.0) ? m_Moments.m01 / m_Moments.m00 : 0.0);
         }

      void Display(DBlobRenderer* pRenderer, cv::Scalar Color, int nDisplay =
            eBlob, cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const;

      void DisplayBoundingRect(DBlobRenderer* pRenderer, cv::Scalar Color) const
         {
         pRenderer->Rectangle(m_rcBound.x, m_rcBound.y, m_rcBound.width, m_rcBound.height, Color);

         return;
         }

      void DisplayCrossHairs(DBlobRenderer* pRenderer, cv::Scalar Color,
            int nThickness = 1) const
         {
         int nDiameter = std::min(m_rcBound.height, m_rcBound.width);
         pRenderer->CrossHairs(static_cast<int>(GetCentroidX()),
               static_cast<int>(GetCentroidY()), nDiameter, Color, nThickness);

         return;
         }

   protected:
      int m_nID;
      int m_nType;
      cv::Moments m_Moments;
      float m_fPerimeter;
      DCVRect m_rcBound;
      std::vector<DRun> m_Pixels;

      void Init();
      void Copy(const DBlob& src);

   private:

   }; // end of class DBlob


/*****************************************************************************
 *
 ***  class DBlobColor
 *
 *  Class to hold the color ranges used to categorize pixels into blobs.  Also,
 * has the display color to represent the range.
 *
 *****************************************************************************/

class DBlobColor
   {
   public:
      using Vector = std::vector<DBlobColor>;
      static const unsigned int m_nVersion = 0;

      DBlobColor() = default;

      DBlobColor(const std::string& strName, const DBlob::DBlobColorRange& ColorRange, const DCVRGB& DisplayColor);

      DBlobColor(const DBlobColor& src) :
            m_strName(src.m_strName),
            m_ColorRange(src.m_ColorRange),
            m_DisplayColor(src.m_DisplayColor)
         {
         return;
         }

      ~DBlobColor() = default;

      DBlobColor& operator=(const DBlobColor& rhs)
         {
         if (this != &rhs)
            {
            m_strName = rhs.m_strName;
            m_ColorRange = rhs.m_ColorRange;
            m_DisplayColor = rhs.m_DisplayColor;
            } // end if

         return (*this);
         }

      // Is the pixel a member of this color?
      bool ComparePixel(const unsigned char Pixel[3])
         {
         return (m_ColorRange.Apply(Pixel));
         }

      const DBlob::DBlobColorRange& GetColorRange() const
         {
         return (m_ColorRange);
         }

      DBlob::DBlobColorRange& GetColorRange()
         {
         return (m_ColorRange);
         }

      void SetDisplayColor(const DBlob::DBlobColorRange& ColorRange)
         {
         m_ColorRange = ColorRange;
         return;
         }

      DCVRGB GetDisplayColor() const
         {
         return (m_DisplayColor);
         }

      void SetDisplayColor(const DCVRGB& Color)
         {
         m_DisplayColor = Color;
         return;
         }

      template<class Archive>
      void save(Archive& ar, const unsigned int /* nVersion */) const
         {
         ar << boost::serialization::make_nvp("ColorName", m_strName);
         ar << boost::serialization::make_nvp("ColorRange", m_ColorRange);
         ar << boost::serialization::make_nvp("DisplayColor", m_DisplayColor);

         return;
         }

      template<class Archive>
      void load(Archive& ar, const unsigned int /* nVersion */)
         {
         ar >> boost::serialization::make_nvp("ColorName", m_strName);
         ar >> boost::serialization::make_nvp("ColorRange", m_ColorRange);
         ar >> boost::serialization::make_nvp("DisplayColor", m_DisplayColor);

         return;
         }

      BOOST_SERIALIZATION_SPLIT_MEMBER()

   protected:
      // Name of the blob color
      std::string m_strName;
      // Color range defining the pixels to include in the blob
      DBlob::DBlobColorRange m_ColorRange;
      // Single color to display to represent the blob
      DCVRGB m_DisplayColor;

   private:
}; // end of class DBlobColor

BOOST_CLASS_VERSION(DBlobColor, DBlobColor::m_nVersion)

/*****************************************************************************
 *
 ***  class DBlobMap
 *
 *****************************************************************************/

using DBlobMapBase = std::map<int, DBlob>;

class DBlobMap : public DBlobMapBase
   {
   public:
      using DSearchPair = std::pair<bool, DBlobMap::iterator>;
      using DConstSearchPair = std::pair<bool, DBlobMap::const_iterator>;

      DBlobMap() = default;

      DBlobMap(const DBlobMap& src) :
            DBlobMapBase(src)
         {
         return;
         }

      ~DBlobMap() = default;

      DBlobMap& operator=(const DBlobMap& rhs)
         {
         DBlobMapBase::operator=(rhs);
         return (*this);
         }

      int RemoveSmallBlobs(double dMinArea);

      int FindLargestBlob() const;

   protected:

   private:

}; // end of class DBlobMap

/*****************************************************************************
 *
 ***  class DBlobs
 *
 *  Class to contain all the blobs found in an image.  It is a vector of maps.
 * There's one vector element (map) for each color.  The maps are keyed on
 * blob ID.
 *
 *****************************************************************************/

using DBlobsBase = std::vector<DBlobMap>;

class DBlobs : public DBlobsBase
   {
   public:
      DBlobs() = default;

      DBlobs(const DBlobs& src) :
            DBlobsBase(src)
         {
         return;
         }

      ~DBlobs() = default;

      DBlobs& operator=(const DBlobs& rhs)
         {
         DBlobsBase::operator=(rhs);
         return (*this);
         }

      DBlobMap::DSearchPair FindBlob(int nID);
      DBlobMap::DConstSearchPair FindBlob(int nID) const;

      // Remove blobs smaller than specified area, return count of removed
      int RemoveSmallBlobs(double dMinArea);
      int RemoveSmallBlobs(int nColor, double dMinArea);

      void FindLargestBlobs(DBlob::DIDVector& Largest) const;

      // Display all the blobs of one category
      void DisplayBlobs(DBlobRenderer* pRenderer, const DBlobMap& BlobMap,
            cv::Scalar BlobColor, int nDisplay = DBlob::eBlob,
            cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const;

      // Display ALL blobs      
      void DisplayBlobs(DBlobRenderer* pRenderer,
            const DBlobColor::Vector& Colors, int nDisplay = DBlob::eBlob,
            cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const;

      // Display one blob with given ID      
      bool DisplayBlob(DBlobRenderer* pRenderer, int nBlobID,
            cv::Scalar BlobColor, int nDisplay = DBlob::eBlob,
            cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const;

      // Display blobs from a vector of IDs
      int DisplayBlobs(DBlobRenderer* pRenderer,
            const DBlob::DIDVector& BlobIDs,
            cv::Scalar BlobColor = cv::Scalar(255, 0, 0), int nDisplay =
                  DBlob::eBlob, cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const;

   protected:

   private:

}; // end of class DBlobs

/*****************************************************************************
 *
 ***  class DColorBlobFinder
 *
 *  Base class for performing the blob finding process.  Derived classes
 * override the color categorization process so different color spaces may
 * be used.  This base class uses the RGB color space by default.
 *
 *****************************************************************************/

class DColorBlobFinder
   {
   public:
      using COLORTYPE = unsigned char;

      DColorBlobFinder()
         {
         Init();

         return;
         }

      DColorBlobFinder(DBlobColor::Vector* pColors);

      DColorBlobFinder(const DColorBlobFinder& src) = delete;

      virtual ~DColorBlobFinder() = default;

      DColorBlobFinder& operator=(const DColorBlobFinder& rhs) = delete;

      bool IsValid() const
         {
         return (m_pColors != nullptr);
         }

      const DBlobs& GetBlobs() const
         {
         return (m_Blobs);
         }

      virtual bool Find(DCVImage* pImage);

      // Remove blobs smaller than specified area, return count of removed
      int RemoveSmallBlobs(double dMinArea)
         {
         return (m_Blobs.RemoveSmallBlobs(dMinArea));
         }

      // Find the largest blob for each color
      void FindLargestBlobs(DBlob::DIDVector& Largest) const
         {
         m_Blobs.FindLargestBlobs(Largest);

         return;
         }

      // Scan the image and designate each pixel into a color category
      virtual void CategorizePixels();

      // Display the background. IE, clear the image
      void DisplayBackground(DBlobRenderer* pRenderer) const
         {
         if (m_pColors != nullptr)
            {
            pRenderer->ClearBackground((*m_pColors)[0].GetDisplayColor());
            } // end if

         return;
         }

      // Display ALL blobs
      void DisplayBlobs(DBlobRenderer* pRenderer, int nDisplay = DBlob::eBlob,
            cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const
         {
         if (m_pColors != nullptr)
            {
            m_Blobs.DisplayBlobs(pRenderer, *m_pColors, nDisplay, CHColor,
                  nThickness);
            } // end if

         return;
         }

      // Display a individual blob by ID
      bool DisplayBlob(DBlobRenderer* pRenderer, int nBlobID,
            cv::Scalar BlobColor, int nDisplay = DBlob::eBlob,
            cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const
         {
         return (m_Blobs.DisplayBlob(pRenderer, nBlobID, BlobColor, nDisplay,
               CHColor, nThickness));
         }

      // Display blobs from a vector of IDs. All blobs will be the specified
      // color.
      int DisplayBlobs(DBlobRenderer* pRenderer,
            const DBlob::DIDVector& BlobIDs,
            cv::Scalar BlobColor = cv::Scalar(255, 0, 0), int nDisplay =
                  DBlob::eBlob, cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const
         {
         return (m_Blobs.DisplayBlobs(pRenderer, BlobIDs, BlobColor, nDisplay,
               CHColor, nThickness));
         }

      // Display the largest blob in each category.  Return the count of blobs
      // displayed.  The current color map will be used.
      int DisplayLargestBlobs(DBlobRenderer* pRenderer, int nDisplay =
            DBlob::eBlob, cv::Scalar CHColor = cv::Scalar(255, 255, 255),
            int nThickness = 1) const;

      virtual void DisplayCategories(DBlobRenderer* pRenderer) const;

      DBlobColor::Vector* GetColors()
         {
         return (m_pColors);
         }

      void SetColors(DBlobColor::Vector* pColors)
         {
         m_pColors = pColors;

         return;
         }

   protected:
      // Colors used to categorize pixels into blobs
      // Owned by the calling application.
      DBlobColor::Vector* m_pColors;

      // Convenience pointer for the current image
      DCVImage* m_pImage;

      // 2D matrix same size as image with each element the color category of
      // the corresponding pixel in the imagew
      DMatrix<COLORTYPE, DArray2D<COLORTYPE>> m_ColorMat;

      // Collection of blobs found
      DBlobs m_Blobs;

      void Init();
      virtual void ConvertRGBPixel(unsigned char RGBPixel[3], unsigned char Pixel[3]);
      bool AllocateMemory();

      bool Find();
      void FindRuns();

   private:

   }; // end of class DColorBlobFinder

/*****************************************************************************
 *
 ***  class DColorBlobFinderHSV
 *
 * Blob Finder using the HSV color space for pixel categorization.
 *
 *****************************************************************************/

class DColorBlobFinderHSV : public DColorBlobFinder
   {
   public:
      DColorBlobFinderHSV() = default;

      DColorBlobFinderHSV(DBlobColor::Vector* pColors) :
            DColorBlobFinder(pColors)
         {
         return;
         }

      DColorBlobFinderHSV(const DColorBlobFinderHSV& src) = delete;

      ~DColorBlobFinderHSV() = default;

      DColorBlobFinderHSV& operator=(const DColorBlobFinderHSV& rhs) = delete;

      virtual void ConvertRGBPixel(unsigned char RGBPixel[3], unsigned char Pixel[3]) override
         {
         DColorHSV HSV(RGBPixel[DCVImage::EColor::eRed], RGBPixel[DCVImage::EColor::eGreen],
               RGBPixel[DCVImage::EColor::eBlue]);
         HSV.ScaleTo255(Pixel);

         return;
         }

      virtual bool Find(DCVImage* pImage) override
         {
         WrapHue();

         return (DColorBlobFinder::Find(pImage));
         }

   protected:
      // Hue is circular so make sure we test that way
      void WrapHue()
         {
         for (auto& Color : *m_pColors)
            {
            Color.GetColorRange().GetColor1().SetCircular(true);
            } // end for

         return;
         }

   private:

   }; // end of class DColorBlobFinderHSV

/*****************************************************************************
 *
 ***  class DColorBlobFinderHSL
 *
 * Blob Finder using the HSL color space for pixel categorization.
 *
 *****************************************************************************/

class DColorBlobFinderHSL : public DColorBlobFinder
   {
   public:
      DColorBlobFinderHSL() = default;

      DColorBlobFinderHSL(DBlobColor::Vector* pColors) :
            DColorBlobFinder(pColors)
         {
         return;
         }

      DColorBlobFinderHSL(const DColorBlobFinderHSL& src) = delete;

      virtual ~DColorBlobFinderHSL() = default;

      DColorBlobFinderHSL& operator=(const DColorBlobFinderHSL& rhs) = delete;

      virtual void ConvertRGBPixel(unsigned char RGBPixel[3], unsigned char Pixel[3]) override
         {
         DColorHSL HSL(RGBPixel[DCVImage::EColor::eRed], RGBPixel[DCVImage::EColor::eGreen],
               RGBPixel[DCVImage::EColor::eBlue]);
         HSL.ScaleTo255(Pixel);

         return;
         }

      virtual bool Find(DCVImage* pImage) override
         {
         WrapHue();
         return (DColorBlobFinder::Find(pImage));
         }

   protected:
      // Hue is circular so make sure we test that way
      void WrapHue()
         {
         for (auto& Color : *m_pColors)
            {
            Color.GetColorRange().GetColor1().SetCircular(true);
            } // end for

         return;
         }

   private:

   }; // end of class DColorBlobFinderHSL

#endif // __DBLOBS_H__
