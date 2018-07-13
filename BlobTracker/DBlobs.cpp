/*****************************************************************************
 ********************************* DBlobs.cpp *********************************
 *****************************************************************************/

/*****************************************************************************
 ******************************  I N C L U D E  *******************************
 *****************************************************************************/

#include "DBlobs.h"

/*****************************************************************************
 ************************* Class DRun Implementation **************************
 *****************************************************************************/

/*****************************************************************************
 ************************* Class DBlob Implementation *************************
 *****************************************************************************/

/*****************************************************************************
 *
 *  DBlob::DBlob
 *
 *****************************************************************************/

DBlob::DBlob()
   {
   Init();

   return;

   } // End of function DBlob::DBlob 

/*****************************************************************************
 *
 *  DBlob::DBlob
 *
 *****************************************************************************/

DBlob::DBlob(int nID, int nType)
   {
   Init();
   m_nID = nID;
   m_nType = nType;

   return;

   } // End of function DBlob::DBlob 

/*****************************************************************************
 *
 *  DBlob::Init
 *
 *  Common initialization.
 *
 *****************************************************************************/

void DBlob::Init()
   {
   m_nID = -1;
   m_nType = -1;
   m_fPerimeter = 0.0f;

   return;

   } // End of function DBlob::Init

/*****************************************************************************
 *
 *  DBlob::Copy
 *
 *****************************************************************************/

void DBlob::Copy(const DBlob& src)
   {
   m_nID = src.m_nID;
   m_nType = src.m_nType;
   m_fPerimeter = src.m_fPerimeter;
   m_Moments = src.m_Moments;
   m_rcBound = src.m_rcBound;
   m_Pixels = src.m_Pixels;

   return;

   } // End of function DBlob::Copy 

/*****************************************************************************
 *
 *  DBlob::AddRun
 *
 *  Add a pixel run to this blob and update the properties.
 *
 *****************************************************************************/

void DBlob::AddRun(DRun& Run)
   {
   Run.SetBlob(m_nID);

   // The area of a run is only 1 pixel tall
   int nRunArea = Run.GetEnd() - Run.GetStart() + 1;
   m_Moments.m00 += nRunArea;

   // Update first moment data
   m_Moments.m10 += nRunArea * ((Run.GetStart() + Run.GetEnd()) / 2.0f);
   m_Moments.m01 += nRunArea * Run.GetRow();

   // Update the bounding rectangle
   if (m_Pixels.empty())
      {
      // First run so initialize the rectangle
      m_rcBound.x = Run.GetStart();
      m_rcBound.width = nRunArea;
      m_rcBound.y = Run.GetRow();
      m_rcBound.height = 1;
      } // end if
   else
      {
      m_rcBound.InflateXLeft(Run.GetStart());
      m_rcBound.InflateXRight(Run.GetEnd());
      m_rcBound.InflateYTop(Run.GetRow());
      m_rcBound.InflateYBottom(Run.GetRow());
      } // end else

   m_Pixels.push_back(Run);

   return;

   } // End of function DBlob::AddRun 

/*****************************************************************************
 *
 *  DBlob::Merge
 *
 *  Two blobs have been found that touch and are the same color.  One gets
 *  absorbed by the other.
 *
 *****************************************************************************/

void DBlob::Merge(DBlob& Blob)
   {
   // Absorb the passed blob's pixel runs
   for (auto& Run : Blob.m_Pixels)
      {
      Run.SetBlob(m_nID);
      AddRun(Run);
      } // end for
   Blob.m_Pixels.clear();

   return;

   } // End of function DBlob::Merge 

/*****************************************************************************
 *
 *  DBlob::Display
 *
 *****************************************************************************/

void DBlob::Display(DBlobRenderer* pRenderer, cv::Scalar Color,
      int nDisplay /* = eBlob */,
      cv::Scalar CHColor /* = cv::Scalar(255, 255, 255) */,
      int nThickness /* = 1 */) const
      {
   if ((nDisplay & eBlob) != 0)
      {
      // Draw horizontal lines for the pixel runs
      for (const auto& Run : m_Pixels)
         {
         pRenderer->Line(Run.GetStart(), Run.GetRow(), Run.GetEnd(),
               Run.GetRow(), Color);
         } // end for
      } // end if

   if ((nDisplay & eBoundingRect) != 0)
      {
      DisplayBoundingRect(pRenderer, Color);
      } // end if

   if ((nDisplay & eCrossHairs) != 0)
      {
      DisplayCrossHairs(pRenderer, CHColor, nThickness);
      } // end if

   return;

   } // End of function DBlob::Display 

/*****************************************************************************
 ********************** Class DBlobColor Implementation ***********************
 *****************************************************************************/

/*****************************************************************************
 *
 *  DBlobColor::DBlobColor
 *
 *****************************************************************************/

DBlobColor::DBlobColor(const std::string& strName, const DBlob::DBlobColorRange& ColorRange,
      const DCVRGB& DisplayColor) :
      m_strName(strName),
      m_ColorRange(ColorRange),
      m_DisplayColor(DisplayColor)
   {

   return;

   } // End of function DBlobColor::DBlobColor 

/*****************************************************************************
 *********************** Class DBlobMap Implementation ************************
 *****************************************************************************/

/*****************************************************************************
 *
 *  DBlobMap::RemoveSmallBlobs
 *
 *  Remove blobs smaller than fMinArea in this map which holds the blobs of
 *  one color.
 *
 *****************************************************************************/

int DBlobMap::RemoveSmallBlobs(double dMinArea)
   {
   int nCount = 0;

   iterator i = begin();
   while (i != end())
      {
      if (i->second.GetArea() < dMinArea)
         {
         i = this->erase(i);
         nCount++;
         } // end if
      else
         {
         ++i;
         } // end else
      } // end while

   return (nCount);

   } // End of function DBlobMap::RemoveSmallBlobs 

/*****************************************************************************
 *
 *  DBlobMap::FindLargestBlob
 *
 *  Find the largest blob in this category.  Return its ID or -1 if none is
 *  found.
 *
 *****************************************************************************/

int DBlobMap::FindLargestBlob() const
   {
   int nID = -1;
   double dMaxArea = 0.0;

   for (const_iterator i = begin() ; i != end() ; ++i)
      {
      if (i->second.GetArea() > dMaxArea)
         {
         dMaxArea = i->second.GetArea();
         nID = i->second.GetID();
         } // end if
      } // end for

   return (nID);

   } // End of function DBlobMap::FindLargestBlob 

/*****************************************************************************
 ************************ Class DBlobs Implementation *************************
 *****************************************************************************/

/*****************************************************************************
 *
 *  DBlobs::FindBlob
 *
 *  Find a particular blob by blob ID.  ID is unique across all colors.
 *
 *****************************************************************************/

DBlobMap::DSearchPair DBlobs::FindBlob(int nID)
   {
   DBlobMap::DSearchPair Result;
   Result.first = false;

   // Start with 1, 0 is background, and check each color
   for (size_t c = 1 ; !Result.first && (c < size()) ; c++)
      {
      DBlobMap& Map = (*this)[c];
      Result.second = Map.find(nID);
      if (Result.second != Map.end())
         {
         Result.first = true;
         } // end if
      } // end for

   return (Result);

   } // End of function DBlobs::FindBlob 

/*****************************************************************************
 *
 *  DBlobs::FindBlob
 *
 *  Find a particular blob by blob ID const version.  ID is unique across all
 *  colors.
 *
 *****************************************************************************/

DBlobMap::DConstSearchPair DBlobs::FindBlob(int nID) const
      {
   DBlobMap::DConstSearchPair Result;
   Result.first = false;

   // Start with 1, 0 is background, and check each color
   for (size_t c = 1 ; !Result.first && (c < size()) ; c++)
      {
      const DBlobMap& Map = (*this)[c];
      Result.second = Map.find(nID);
      if (Result.second != Map.end())
         {
         Result.first = true;
         } // end if
      } // end for

   return (Result);

   } // End of function DBlobs::FindBlob 

/*****************************************************************************
 *
 *  DBlobs::FindLargestBlobs
 *
 *  Find the largest blob for each color category and return the IDs by
 *  filling in the passed vector.  Element 0 of the array will contain -1 since
 *  by convention this is the background.  -1 is used to indicate a category
 *  has no blobs found.
 *
 *****************************************************************************/

void DBlobs::FindLargestBlobs(DBlob::DIDVector& Largest) const
      {
   Largest.resize(this->size());
   if (Largest.size() > 0)
      {
      // Element 0 is background and has no members
      Largest[0] = -1;

      // Start with 1, 0 is background, and check each color
      for (size_t c = 1 ; c < this->size() ; c++)
         {
         const DBlobMap& Map = (*this)[c];
         Largest[c] = Map.FindLargestBlob();
         } // end for
      } // end if

   return;

   } // End of function DBlobs::FindLargestBlobs 

/*****************************************************************************
 *
 *  DBlobs::RemoveSmallBlobs
 *
 *  Remove blobs smaller than the specified area.  Return the number of blobs
 *  removed.
 *
 *****************************************************************************/

int DBlobs::RemoveSmallBlobs(double dMinArea)
   {
   int nCount = 0;

   // Start with 1, 0 is background, and check each color
   for (size_t c = 1 ; c < this->size() ; c++)
      {
      DBlobMap& Map = (*this)[c];
      nCount += Map.RemoveSmallBlobs(dMinArea);
      } // end for

   return (nCount);

   } // End of function DBlobs::RemoveSmallBlobs 

/*****************************************************************************
 *
 *  DBlobs::RemoveSmallBlobs
 *
 *  Remove blobs of the specified color smaller than the specified area.
 *  Return the number of blobs removed.
 *
 *****************************************************************************/

int DBlobs::RemoveSmallBlobs(int nColor, double dMinArea)
   {
   DBlobMap& Map = (*this)[nColor];

   return (Map.RemoveSmallBlobs(dMinArea));

   } // End of function DBlobs::RemoveSmallBlobs 

/*****************************************************************************
 *
 *  DBlobs::DisplayBlobs
 *
 *  Display all the blobs of one color and optionally their decoration.
 *
 *****************************************************************************/

void DBlobs::DisplayBlobs(DBlobRenderer* pRenderer, const DBlobMap& BlobMap,
      cv::Scalar BlobColor, int nDisplay /* = DBlob:: eBlob */,
      cv::Scalar CHColor /* = cv::Scalar(255, 255, 255) */,
      int nThickness /* = 1 */) const
      {
   for (DBlobMap::const_iterator b = BlobMap.begin() ; b != BlobMap.end()
         ; ++b)
      {
      b->second.Display(pRenderer, BlobColor, nDisplay, CHColor, nThickness);
      } // end for

   return;

   } // End of function DBlobs::DisplayBlobs 

/*****************************************************************************
 *
 *  DBlobs::DisplayBlobs
 *
 *  Display all the blobs and optionally their decoration.
 *
 *****************************************************************************/

void DBlobs::DisplayBlobs(DBlobRenderer* pRenderer,
      const DBlobColor::Vector& Colors, int nDisplay /* = DBlobs::eBlob */,
      cv::Scalar CHColor /* = cv::Scalar(255, 255, 255) */,
      int nThickness /* = 1 */) const
      {
   for (size_t c = 1 ; c < size() ; c++)
      {
      DisplayBlobs(pRenderer, (*this)[c], Colors[c].GetDisplayColor(), nDisplay,
            CHColor, nThickness);
      } // end for

   return;

   } // End of function DBlobs::DisplayBlobs 

/*****************************************************************************
 *
 *  DBlobs::DisplayBlob
 *
 *****************************************************************************/

bool DBlobs::DisplayBlob(DBlobRenderer* pRenderer, int nBlobID, cv::Scalar BlobColor,
      int nDisplay /* = DBlob::eBlob */,
      cv::Scalar CHColor /* = cv::Scalar(255, 255, 255) */,
      int nThickness /* = 1 */) const
      {
   DBlobMap::DConstSearchPair Pair = FindBlob(nBlobID);
   if (Pair.first)
      {
      Pair.second->second.Display(pRenderer, BlobColor, nDisplay,
            CHColor, nThickness);
      } // end if

   return (Pair.first);

   } // End of function DBlobs::DisplayBlob 

/*****************************************************************************
 *
 *  DBlobs::DisplayBlobs
 *
 *  Display blobs from a vector of IDs.  Optionally, display their decoration.
 *  Return a count of the blobs actually displayed.
 *
 *****************************************************************************/

int DBlobs::DisplayBlobs(DBlobRenderer* pRenderer, const DBlob::DIDVector& BlobIDs,
      cv::Scalar BlobColor /* = cv::Scalar(255, 0, 0) */,
      int nDisplay /* = DBlobs::eBlob */,
      cv::Scalar CHColor /* = cv::Scalar(255, 255, 255) */,
      int nThickness /* = 1 */) const
      {
   int nCount = 0;
   for (auto& BlobID : BlobIDs)
      {
      if (DisplayBlob(pRenderer, BlobID, BlobColor, nDisplay, CHColor,
            nThickness))
         {
         nCount++;
         } // end if
      } // end for

   return (nCount);

   } // End of function DBlobs::DisplayBlobs 

/*****************************************************************************
 ******************* Class DColorBlobFinder Implementation ********************
 *****************************************************************************/

/*****************************************************************************
 *
 *  DColorBlobFinder::DColorBlobFinder
 *
 *****************************************************************************/

DColorBlobFinder::DColorBlobFinder(DBlobColor::Vector* pColors)
   {
   Init();

   m_pColors = pColors;

   return;

   } // End of function DColorBlobFinder::DColorBlobFinder 

/*****************************************************************************
 *
 *  DColorBlobFinder::Init
 *
 *  Handle common initialization.
 *
 *****************************************************************************/

void DColorBlobFinder::Init()
   {
   m_pColors = nullptr;
   m_pImage = nullptr;

   return;

   } // End of function DColorBlobFinder::Init 

/*****************************************************************************
 *
 *  DColorBlobFinder::ConvertRGBPixel
 *
 *  Convert the RGB pixel from the original image to the color space to be
 *  used for testing.
 *
 *  This default implementation simply does a copy so testing would be RGB.
 *
 *****************************************************************************/

void DColorBlobFinder::ConvertRGBPixel(unsigned char RGBPixel[3],
      unsigned char Pixel[3])
   {
   for (int i = 0 ; i < 3 ; i++)
      {
      Pixel[i] = RGBPixel[i];
      } // end for

   return;

   } // End of function DColorBlobFinder::ConvertRGBPixel 

/*****************************************************************************
 *
 *  DColorBlobFinder::AllocateMemory
 *
 *  Allocate or reallocate the memory necessary to support finding the blobs.
 *
 *****************************************************************************/

bool DColorBlobFinder::AllocateMemory()
   {
   // (Re)Create the matrix of pixel color categories
   m_ColorMat.Resize(m_pImage->GetNumRows(), m_pImage->GetNumCols());

   // Clear out the old blobs
   for (auto& Blob : m_Blobs)
      {
      Blob.clear();
      } // end for

   m_Blobs.resize(m_pColors->size());

   return (true);

   } // End of function DColorBlobFinder::AllocateMemory 

/*****************************************************************************
 *
 *  DColorBlobFinder::Find
 *
 *  The primary entry point of find color blobs.
 *
 *****************************************************************************/

bool DColorBlobFinder::Find()
   {
   bool bRet = (m_pImage != nullptr) && (m_pColors != nullptr) && AllocateMemory();
   if (bRet)
      {
      CategorizePixels();
      FindRuns();
      } // end if

   return (bRet);

   } // End of function DColorBlobFinder::Find 

/*****************************************************************************
 *
 *  DColorBlobFinder::Find
 *
 *  The external find entry point.  Assumes the finder is initialized and
 *  ready to process an image.
 *
 *****************************************************************************/

bool DColorBlobFinder::Find(DCVImage* pImage)
   {
   m_pImage = pImage;

   return (Find());

   } // End of function DColorBlobFinder::Find 

/*****************************************************************************
 *
 *  DColorBlobFinder::CategorizePixels
 *
 *  Apply the color thresholds and categorize each pixel into background or
 *  color category.  Background is always category zero.
 *
 *****************************************************************************/

void DColorBlobFinder::CategorizePixels()
   {
   // Check each pixel in the input and catgorize it as to color
   for (int r = 0 ; r < m_pImage->GetNumRows() ; r++)
      {
      unsigned char* pRow = m_pImage->GetRow(r);
      COLORTYPE*  ColorRow = m_ColorMat[r];
//      unsigned char* ColorRow = m_ColorMat[r];

      for (int c = 0 ; c < m_pImage->GetNumCols() ; c++)
         {
         // Set the pixel to background as a default
         ColorRow[c] = 0;

         // Convert the RGB image pixel to the testing color space         
         unsigned char Pixel[3];
         ConvertRGBPixel(m_pImage->GetPixel(pRow, c), Pixel);

         // Check each color.  No distance metric is used.
         // First match wins!
         for (size_t i = 1 ; i < m_pColors->size() ; i++)
            {
            if ((*m_pColors)[i].ComparePixel(Pixel))
               {
               ColorRow[c] = static_cast<COLORTYPE>(i);
               break;
               } // end if
            } // end for
         } // end for
      } // end for

   return;

   } // End of function DColorBlobFinder::CategorizePixels 

/*****************************************************************************
 *
 *  DColorBlobFinder::FindRuns
 *
 *  Find all the runs of pixels for each color.
 *
 *****************************************************************************/

void DColorBlobFinder::FindRuns()
   {
   // Provide storage for two image rows of horizontal color run data
   size_t nColorCount = m_pColors->size();
   int nPrevRuns = 0;
   int nCurRuns = 1;
   std::vector<std::vector<DRun> > ColorRuns[2];
   ColorRuns[0].resize(nColorCount);
   ColorRuns[1].resize(nColorCount);

   // Each blob found will have a unique number
   int nNextBlob = 0;

   // Check the color of each pixel and keep track of horizontal contiguous
   // runs
   for (int r = 0 ; r < m_pImage->GetNumRows() ; r++)
      {
      COLORTYPE* ColorRow = m_ColorMat[r];
      // New row starting so a new color run is starting, too
      COLORTYPE nCurColor = ColorRow[0];
      int nStart = 0;
      for (int c = 1 ; c < m_pImage->GetNumCols() ; c++)
         {
         // If the current pixel color isn't the same as the current run,
         // a new run is starting
         if (ColorRow[c] != nCurColor)
            {
            //  New run starting
            if (nCurColor != 0)
               {
               // Current run is not background so save it
               ColorRuns[nCurRuns][nCurColor].push_back(
                     DRun(r, nStart, c - 1, nCurColor));
               } // end if

            // Mark a new color run starting   
            nCurColor = ColorRow[c];
            nStart = c;
            } // end if
         } // end for Column Loop

      // At end of a row so save the current run if not background
      if (nCurColor != 0)
         {
         ColorRuns[nCurRuns][nCurColor].push_back(
               DRun(r, nStart, m_pImage->GetNumCols() - 1, nCurColor));
         } // end if          `

      // If runs found in this row, try to match them with existing blobs in 
      // the  previous row.
      for (size_t nColor = 1 ; nColor < nColorCount ; nColor++)
         {
         size_t nCurCount = ColorRuns[nCurRuns][nColor].size();
         size_t nPrevCount = ColorRuns[nPrevRuns][nColor].size();
         for (size_t c = 0 ; c < nCurCount ; c++)
            {
            DRun& CurRun = ColorRuns[nCurRuns][nColor][c];
            for (size_t p = 0 ; p < nPrevCount ; p++)
               {
               DRun& PrevRun = ColorRuns[nPrevRuns][nColor][p];
               if (CurRun.Overlap(PrevRun))
                  {
                  // We have overlap so decide what to do with it
                  if (CurRun.GetBlob() == -1)
                     {
                     // Aren't part of a blob so join the existing blob
                     int nBlob = PrevRun.GetBlob();
                     m_Blobs[nColor][nBlob].AddRun(CurRun);
                     } // end if
                  else
                     {
                     // Two blobs may be in contact
                     if (CurRun.GetBlob() != PrevRun.GetBlob())
                        {
                        // Two blobs are in contact, merge them otherwise it's
                        // just another point of contact on a complex blob
                        int nPrevBlob = PrevRun.GetBlob();
                        int nCurBlob = CurRun.GetBlob();
                        DBlob& PrevBlob = m_Blobs[nColor][nPrevBlob];
                        m_Blobs[nColor][nCurBlob].Merge(PrevBlob);
                        m_Blobs[nColor].erase(nPrevBlob);

                        // Update the runs of the eliminated blob
                        for (size_t i = 0 ; i < nPrevCount ; i++)
                           {
                           DRun& Run = ColorRuns[nPrevRuns][nColor][i];
                           if (Run.GetBlob() == nPrevBlob)
                              {
                              Run.SetBlob(nCurBlob);
                              } // end if
                           } // end for

                        for (size_t i = 0 ; i < nCurCount ; i++)
                           {
                           DRun& Run = ColorRuns[nCurRuns][nColor][i];
                           if (Run.GetBlob() == nPrevBlob)
                              {
                              Run.SetBlob(nCurBlob);
                              } // end if
                           } // end for
                        } // end if
                     } // end else
                  } // end if
               } // end for

            // Is the current run assigned to a blob?      
            if (CurRun.GetBlob() == -1)
               {
               // Wasn't joined to an existing blob so start a new one
               DBlob Blob(nNextBlob, CurRun.GetColor());
               Blob.AddRun(CurRun);
               m_Blobs[CurRun.GetColor()].insert(
                     DBlobMap::value_type(nNextBlob, Blob));
               nNextBlob++;
               } // end if   
            } // end for   
         } // end for Color Loop

      // Make the current row runs the previous row runs
      nCurRuns ^= 1;
      nPrevRuns ^= 1;

      // Clear the previous row's runs which is now the current row
      for (auto& ColorRun : ColorRuns[nCurRuns])
         {
         ColorRun.clear();
         } // end for
      } // end for Row Loop

   return;

   } // End of function DColorBlobFinder::FindRuns 

/*****************************************************************************
 *
 *  DColorBlobFinder::DisplayCategories
 *
 *  Build an Image where each pixel's color indicates the blob color.  This
 *  routine is relatively slow but is mostly for debugging.
 *
 *****************************************************************************/

void DColorBlobFinder::DisplayCategories(DBlobRenderer* pRenderer) const
      {
   if ((m_pImage != nullptr) && (m_pColors != nullptr))
      {
      for (int r = 0 ; r < m_pImage->GetNumRows() ; r++)
         {
         const COLORTYPE* ColorRow = m_ColorMat[r];
         for (int c = 0 ; c < m_pImage->GetNumCols() ; c++)
            {
            DBlobColor BlobColor = (*m_pColors)[ColorRow[c]];
            pRenderer->Point(r, c, BlobColor.GetDisplayColor());
            } // end for
         } // end for
      } // end if

   return;

   } // End of function DColorBlobFinder::DisplayCategories 

/*****************************************************************************
 *
 *  DColorBlobFinder::DisplayLargestBlobs
 *
 *****************************************************************************/

int DColorBlobFinder::DisplayLargestBlobs(DBlobRenderer* pRenderer,
      int nDisplay /* = DBlob::eBlob */,
      cv::Scalar CHColor /* = cv::Scalar(255, 255, 255) */,
      int nThickness /* = 1 */) const
      {
   int nCount = 0;
   DBlob::DIDVector Largest;

   if (m_pColors != nullptr)
      {
      FindLargestBlobs(Largest);
      for (size_t i = 1 ; i < Largest.size() ; i++)
         {
         if (Largest[i] != -1)
            {
            m_Blobs.DisplayBlob(pRenderer, Largest[i],
                  (*m_pColors)[i].GetDisplayColor(), nDisplay,
                  CHColor, nThickness);
            nCount++;
            } // end if
         } // end for
      } // end if

   return (nCount);

   } // End of function DColorBlobFinder::DisplayLargestBlobs



