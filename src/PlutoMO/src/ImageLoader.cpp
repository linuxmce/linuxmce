/*
 ImageLoader
 
 Copyright (C) 2004 Pluto, Inc., a Florida Corporation
 
 www.plutohome.com		
 
 Phone: +1 (877) 758-8648
 
 This program is distributed according to the terms of the Pluto Public License, available at: 
 http://plutohome.com/index.php?section=public_license 
 
 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. See the Pluto Public License for more details.
 
 */

#include "ImageLoader.h"

////////////////////////////////////// 
// CImageLoader 
////////////////////////////////////// 

void CImageLoader::DoCancel() 
{ 
   // cancel the timer    
   iTimer->Cancel(); 
   iTimer->Close(); 

   // end blocking in ReadTextureL() 
   CActiveScheduler::Stop(); 
} 

void CImageLoader::RunL() 
{ 
   // timer timeout - loading failed 
   Cancel(); 
   iTimer->Close(); 
} 

CFbsBitmap * CImageLoader::LoadBitmap(const TDesC &aFilename) 
{ 
   // construct new loader instance 
   CImageLoader *loader = new (ELeave) CImageLoader(&aFilename); 
   CleanupStack::PushL(loader); 

   // add the loader to active scheduler 
   CActiveScheduler::Add(loader);    

   // perform operations for reading and converting the image 
   loader->ReadImageL(); 
    
   // start a nested scheduling; blocks until CActiveScheduler::Stop() 
   // is called in DoCancel() 
   CActiveScheduler::Start(); 

   // if error, leave with correct error code 
   if( loader->iBitmap == NULL ) { 
       // instance will be destroyed by the cleanupstack 
       User::Leave(loader->iErrorCode); 
   } 

   // get a local copy of the instance's created iBitmap 
   // if error(s) in process, this will be NULL 
   CFbsBitmap *iBitmap = loader->iBitmap; 

   // deallocate the instance 
   CleanupStack::PopAndDestroy(); 

   // return the created texture 
   return iBitmap; 
} 

CImageLoader::CImageLoader(const TDesC *aFilename) 
   : CActive(CActive::EPriorityStandard) 
{ 
   // make a local copy of the filename 
   iFilename = aFilename->Alloc(); 
} 

CImageLoader::~CImageLoader() 
{ 
   RDebug::Print(_L("CImageLoader::~CImageLoader()")); 

   // deallocate all data 
   delete iFilename; 
   delete iTimer; 
   delete iConverter; 
} 

// performs the actual reading and conversion of the image 
void CImageLoader::ReadImageL() 
{ 
   // reset the iBitmap 
   iBitmap = NULL; 

   // create + initialize operation timeout timer 
   iTimer = new RTimer(); 
   iTimer->CreateLocal(); 

   // set timeout for the image read + conversion process 
   iTimer->After(iStatus, IMAGEREAD_TIMEOUT); 
   SetActive(); 

   // start loading the image 
   iConverter = CMdaImageFileToBitmapUtility::NewL(*this); 
   iConverter->OpenL(*iFilename); 
} 

// called when OpenL() finishes 
void CImageLoader::MiuoOpenComplete(TInt aError) 
{ 
   if( aError != KErrNone ) { 
       iErrorCode = aError; 
       Cancel(); 
       return; 
   } 

        TFrameInfo info; 
        iConverter->FrameInfo(0, info); 
        
        // create a bitmap to write into 
        iBitmap = new (ELeave) CFbsBitmap(); 
        TInt rc = iBitmap->Create(info.iOverallSizeInPixels, EColor4K); 
        if( rc != KErrNone ) 
        { 
       iErrorCode = rc; 
       Cancel(); 
       return; 
   } 

   // convert the gif into a bitmap 
   TRAPD(error, iConverter->ConvertL(*iBitmap)); 

   // handle the error 
   if( error != KErrNone) { 
       iErrorCode = error; 
       Cancel(); 
       return; 
        } 
} 

// called when ConvertL() finishes 
void CImageLoader::MiuoConvertComplete(TInt aError) 
{ 
   if( aError != KErrNone ) { 
       iErrorCode = aError; 
       Cancel(); 
       return; 
   } 

   // cancel the timeout timer to end blocking in LoadTextureL() 
   Cancel(); 
} 

// quicksorts the color frequency table 
void CImageLoader::SortFreqTable(TInt aLeft, TInt aRight) 
{ 
        TInt qleft = aLeft; 
        TInt qright = aRight; 
        TInt qpivot        = iFreqTable[(qleft + qright) >> 1].freq; 

        do { 
                while( (iFreqTable[qleft].freq > qpivot) && (qleft < aRight) ) { 
                        qleft++; 
                } 

                while( (qpivot > iFreqTable[qright].freq) && (qright > aLeft) ) { 
                        qright--; 
                } 

                if( qleft <= qright ) { 
                        // swap elements 
                        TUint16 tmp = iFreqTable[qleft].freq; 
                        iFreqTable[qleft].freq = iFreqTable[qright].freq; 
                        iFreqTable[qright].freq = tmp; 
                        tmp = iFreqTable[qleft].color; 
                        iFreqTable[qleft].color = iFreqTable[qright].color; 
                        iFreqTable[qright].color = tmp; 

                        qleft++; 
                        qright--; 
                } 
        } while( qleft <= qright ); 
        
        // recursively sort left side 
        if( aLeft < qright ) SortFreqTable(aLeft, qright); 

        // recursively sort right side 
        if( qleft < aRight ) SortFreqTable(qleft, aRight); 
} 

// find color from the first 256 entries with smallest cubic difference 
// component wise for the given 12bit color 
inline TUint8 CImageLoader::FindNearestColor(TUint16 aColor, TInt aPaletteSize) 
{ 
        TInt index = -1; 
        TUint difference = 0xffffffff; 

   // extract original color components 
        TUint red0 = (aColor >> 8) & 0xf; 
        TUint green0 = (aColor >> 4) & 0xf; 
        TUint blue0 = aColor & 0xf; 

        for( TInt i = 0; i < aPaletteSize; i++ ) { 
                TUint16 color = iFreqTable[i].color; 

       // extract components of palette color 
                TUint red = (color >> 8) & 0xf; 
                TUint green = (color >> 4) & 0xf; 
                TUint blue = color & 0xf; 

       // calculate cubic difference 
                TUint diff = ((red0 - red) * (red0 - red)) + 
                                         ((green0 - green) * (green0 - green)) + 
                                         ((blue0 - blue) * (blue0 - blue)); 

                // try early-out if exact match 
                if( diff == 0 ) { 
                        return (TUint8)i; 
                } 

                if( diff < difference ) { 
                        difference = diff; 
                        index = i; 
                } 
        } 

        return (TUint8)index; 
} 

// not used 
void CImageLoader::MiuoCreateComplete(TInt /*aError*/) 
{ 
} 