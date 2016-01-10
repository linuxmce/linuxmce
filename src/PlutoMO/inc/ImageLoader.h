/*
 Main

 Copyright (C) 2004 Pluto, Inc., a Florida Corporation

 www.plutohome.com
 

 Phone: +1 (877) 758-8648


 This program is distributed according to the terms of the Pluto Public License, available at:
 http://plutohome.com/index.php?section=public_license

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.

 */
/* imageloader.h */ 
#ifndef __IMAGELOADER_H 
#define __IMAGELOADER_H 

#include <e32base.h> 
#include <mdaimageconverter.h> 
#include <fbs.h> 
#include <e32math.h> 
#define IMAGEREAD_TIMEOUT 5 * 1000 * 1000 

/** 
* Color frequency array item. 
*/ 
struct FreqItem { 
        TUint16 freq; 
        TUint16 color; 
}; 

/** 
* Utility methods for loading images. 
*/ 
class CImageLoader : public CActive, public MMdaImageUtilObserver 
{ 
   public: 
       /** 
        * Error codes for texture loading 
        */ 
       enum TTextureLoadingError { KBadImageWidth = 1666001 }; 

       /*! 
         @function LoadTextureL 
  
         @discussion Loads a image file and converts it into a CTexture object 
         @param aFilename filename of the image file 
         */ 
       static CFbsBitmap * LoadBitmap(const TDesC &aFilename); 


       // from MMdaImageUtilObserver 
       virtual void MiuoOpenComplete(TInt aError); 
       virtual void MiuoConvertComplete(TInt aError); 
       virtual void MiuoCreateComplete(TInt aError); 

       // from CActive 
       void RunL(); 
       void DoCancel(); 

   private: 
       CImageLoader(const TDesC *aFilename); 
       ~CImageLoader(); 
       void ReadImageL(); 
       void SortFreqTable(TInt aLeft, TInt aRight); 
       TUint8 FindNearestColor(TUint16 aColor, TInt aPaletteSize); 

       TDesC *iFilename; 
       CMdaImageFileToBitmapUtility *iConverter; 
       RTimer *iTimer; 
   CFbsBitmap *iBitmap; 
       TInt iErrorCode; 
       FreqItem *iFreqTable; 
}; 

#endif // __IMAGELOADER_H 
