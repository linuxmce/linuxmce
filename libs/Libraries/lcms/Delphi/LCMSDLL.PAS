//
//  Little cms
//  Copyright (C) 1998-2001 Marti Maria
//
// THIS SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
// EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
// WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//
// IN NO EVENT SHALL MARTI MARIA BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
// INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
// OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
// WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
// LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
// OF THIS SOFTWARE.
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// ver 1.09a - BETA 0

UNIT lcmsdll;

INTERFACE

USES Windows;

 CONST

     // Intents

    INTENT_PERCEPTUAL               =  0;
    INTENT_RELATIVE_COLORIMETRIC    =  1;
    INTENT_SATURATION               =  2;
    INTENT_ABSOLUTE_COLORIMETRIC    =  3;

    // Flags

    cmsFLAGS_MATRIXINPUT            = $0001;
    cmsFLAGS_MATRIXOUTPUT           = $0002;
    cmsFLAGS_MATRIXONLY             = (cmsFLAGS_MATRIXINPUT OR cmsFLAGS_MATRIXOUTPUT);

    cmsFLAGS_NOTPRECALC             = $0100;
    cmsFLAGS_HIGHRESPRECALC         = $0400;  // Use bigger precalc tables

    cmsFLAGS_NULLTRANSFORM          = $0200;  // Don't transform anyway

    cmsFLAGS_GAMUTCHECK             = $1000;  // Mark Out of Gamut as alarm color (on proofing transform)
    cmsFLAGS_SOFTPROOFING           = $4000;  // Softproof of proofing profile

    cmsFLAGS_WHITEBLACKCOMPENSATION = $2000;  // Matches black and white on precalculated transforms



    // Format descriptors

    TYPE_GRAY_8             = $30009;
    TYPE_GRAY_8_REV         = $32009;
    TYPE_GRAY_16            = $3000a;
    TYPE_GRAY_16_REV        = $3200a;
    TYPE_GRAY_16_SE         = $3080a;
    TYPE_GRAYA_8            = $30089;
    TYPE_GRAYA_16           = $3008a;
    TYPE_GRAYA_16_SE        = $3088a;
    TYPE_GRAYA_8_PLANAR     = $31089;
    TYPE_GRAYA_16_PLANAR    = $3108a;
    TYPE_RGB_8              = $40019;
    TYPE_RGB_8_PLANAR       = $41019;
    TYPE_BGR_8              = $40419;
    TYPE_BGR_8_PLANAR       = $41419;
    TYPE_RGB_16             = $4001a;
    TYPE_RGB_16_PLANAR      = $4101a;
    TYPE_RGB_16_SE          = $4081a;
    TYPE_BGR_16             = $4041a;
    TYPE_BGR_16_PLANAR      = $4141a;
    TYPE_BGR_16_SE          = $40c1a;
    TYPE_RGBA_8             = $40099;
    TYPE_RGBA_8_PLANAR      = $41099;
    TYPE_RGBA_16            = $4009a;
    TYPE_RGBA_16_PLANAR     = $4109a;
    TYPE_RGBA_16_SE         = $4089a;
    TYPE_ABGR_8             = $40499;
    TYPE_ABGR_16            = $4049a;
    TYPE_ABGR_16_PLANAR     = $4149a;
    TYPE_ABGR_16_SE         = $40c9a;
    TYPE_BGRA_8             = $44499;
    TYPE_BGRA_16            = $4449a;
    TYPE_BGRA_16_SE         = $44c9a;
    TYPE_CMY_8              = $50019;
    TYPE_CMY_8_PLANAR       = $51019;
    TYPE_CMY_16             = $5001a;
    TYPE_CMY_16_PLANAR      = $5101a;
    TYPE_CMY_16_SE          = $5081a;
    TYPE_CMYK_8             = $60021;
    TYPE_CMYK_8_REV         = $62021;
    TYPE_YUVK_8             = $62021;
    TYPE_CMYK_8_PLANAR      = $61021;
    TYPE_CMYK_16            = $60022;
    TYPE_CMYK_16_REV        = $62022;
    TYPE_YUVK_16            = $62022;
    TYPE_CMYK_16_PLANAR     = $61022;
    TYPE_CMYK_16_SE         = $60822;
    TYPE_KYMC_8             = $60421;
    TYPE_KYMC_16            = $60422;
    TYPE_KYMC_16_SE         = $60c22;
    TYPE_KCMY_8             = $64021;
    TYPE_KCMY_8_REV         = $66021;
    TYPE_KCMY_16            = $64022;
    TYPE_KCMY_16_REV        = $66022;
    TYPE_KCMY_16_SE         = $64822;
    TYPE_CMYKcm_8           = $0031;
    TYPE_CMYKcm_8_PLANAR    = $1031;
    TYPE_CMYKcm_16          = $0032;
    TYPE_CMYKcm_16_PLANAR   = $1032;
    TYPE_CMYKcm_16_SE       = $0832;
    TYPE_CMYK7_8            = $0039;
    TYPE_CMYK7_16           = $003a;
    TYPE_CMYK7_16_SE        = $083a;
    TYPE_KYMC7_8            = $0439;
    TYPE_KYMC7_16           = $043a;
    TYPE_KYMC7_16_SE        = $0c3a;
    TYPE_CMYK8_8            = $0041;
    TYPE_CMYK8_16           = $0042;
    TYPE_CMYK8_16_SE        = $0842;
    TYPE_KYMC8_8            = $0441;
    TYPE_KYMC8_16           = $0442;
    TYPE_KYMC8_16_SE        = $0c42;
    TYPE_CMYK9_8            = $0049;
    TYPE_CMYK9_16           = $004a;
    TYPE_CMYK9_16_SE        = $084a;
    TYPE_KYMC9_8            = $0449;
    TYPE_KYMC9_16           = $044a;
    TYPE_KYMC9_16_SE        = $0c4a;
    TYPE_CMYK10_8           = $0051;
    TYPE_CMYK10_16          = $0052;
    TYPE_CMYK10_16_SE       = $0852;
    TYPE_KYMC10_8           = $0451;
    TYPE_KYMC10_16          = $0452;
    TYPE_KYMC10_16_SE       = $0c52;
    TYPE_CMYK11_8           = $0059;
    TYPE_CMYK11_16          = $005a;
    TYPE_CMYK11_16_SE       = $085a;
    TYPE_KYMC11_8           = $0459;
    TYPE_KYMC11_16          = $045a;
    TYPE_KYMC11_16_SE       = $0c5a;
    TYPE_CMYK12_8           = $0061;
    TYPE_CMYK12_16          = $0062;
    TYPE_CMYK12_16_SE       = $0862;
    TYPE_KYMC12_8           = $0461;
    TYPE_KYMC12_16          = $0462;
    TYPE_KYMC12_16_SE       = $0c62;
    TYPE_XYZ_16             = $9001a;
    TYPE_Lab_8              = $a0019;
    TYPE_LabA_8             = $a0099;
    TYPE_Lab_16             = $a001a;
    TYPE_Yxy_16             = $e001a;
    TYPE_YCbCr_8            = $70019;
    TYPE_YCbCr_8_PLANAR     = $71019;
    TYPE_YCbCr_16           = $7001a;
    TYPE_YCbCr_16_PLANAR    = $7101a;
    TYPE_YCbCr_16_SE        = $7081a;
    TYPE_YUV_8              = $80019;
    TYPE_YUV_8_PLANAR       = $81019;
    TYPE_YUV_16             = $8001a;
    TYPE_YUV_16_PLANAR      = $8101a;
    TYPE_YUV_16_SE          = $8081a;
    TYPE_HLS_8              = $d0019;
    TYPE_HLS_8_PLANAR       = $d1019;
    TYPE_HLS_16             = $d001a;
    TYPE_HLS_16_PLANAR      = $d101a;
    TYPE_HLS_16_SE          = $d081a;
    TYPE_HSV_8              = $c0019;
    TYPE_HSV_8_PLANAR       = $c1019;
    TYPE_HSV_16             = $c001a;
    TYPE_HSV_16_PLANAR      = $c101a;
    TYPE_HSV_16_SE          = $c081a;


    // Some utility functions to compute new descriptors


    FUNCTION COLORSPACE_SH(e: Integer):Integer;
    FUNCTION SWAPFIRST_SH(e: Integer):Integer;
    FUNCTION FLAVOR_SH(e: Integer):Integer;
    FUNCTION PLANAR_SH(e: Integer):Integer;
    FUNCTION ENDIAN16_SH(e: Integer):Integer;
    FUNCTION DOSWAP_SH(e: Integer):Integer;
    FUNCTION EXTRA_SH(e: Integer):Integer;
    FUNCTION CHANNELS_SH(c: Integer):Integer;
    FUNCTION BYTES_SH(b: Integer):Integer;


  TYPE

      DWord                  = Cardinal;

      icTagSignature         = DWord;
      icColorSpaceSignature  = DWord;
      icProfileClassSignature= DWord;

  CONST

    // icc color space signatures

    icSigXYZData              = $58595A20;
    icSigLabData              = $4C616220;
    icSigLuvData              = $4C757620;
    icSigYCbCrData            = $59436272;
    icSigYxyData              = $59787920;
    icSigRgbData              = $52474220;
    icSigGrayData             = $47524159;
    icSigHsvData              = $48535620;
    icSigHlsData              = $484C5320;
    icSigCmykData             = $434D594B;
    icSigCmyData              = $434D5920;

    // icc Profile class signatures

    icSigInputClass           = $73636E72;
    icSigDisplayClass         = $6D6E7472;
    icSigOutputClass          = $70727472;
    icSigLinkClass            = $6C696E6B;
    icSigAbstractClass        = $61627374;
    icSigColorSpaceClass      = $73706163;
    icSigNamedColorClass      = $6e6d636c;


    // Added by lcms

    lcmsSignature                = $6c636d73;
    icSigLuvKData                = $4C75764B;  {'LuvK'}
    icSigChromaticityTag         = $6368726d;  { As per Addendum 2 to Spec. ICC.1:1998-09 }
    icSigChromaticityType        = $6368726d;


  TYPE

    cmsHPROFILE            = Pointer;
    cmsHTRANSFORM          = Pointer;

      GAMMATABLE = PACKED RECORD
                        nEntries   : DWord;
                        GammaTable : ARRAY [0..1] OF Word;
                     END;

      LPGAMMATABLE = ^GAMMATABLE;


      // Colorimetric spaces

      cmsCIEXYZ = PACKED RECORD
                        X, Y, Z : Double;
                    END;

      LPcmsCIEXYZ = ^cmsCIEXYZ;

      cmsCIEXYZTRIPLE = PACKED RECORD
                        Red, Green, Blue : cmsCIEXYZ
                        END;

      LPcmsCIEXYZTRIPLE = ^cmsCIEXYZTRIPLE;

      cmsCIExyY = PACKED RECORD
                        x, y, YY : Double
                        END;

      LPcmsCIExyY = ^cmsCIExyY;


      cmsCIExyYTRIPLE = PACKED RECORD
                        Red, Green, Blue : cmsCIExyY
                        END;

      LPcmsCIExyYTRIPLE = ^cmsCIExyYTRIPLE;


      cmsCIELab = PACKED RECORD
                  L, a, b: Double
                  END;

      LPcmsCIELab = ^cmsCIELab;

      cmsCIELCh = PACKED RECORD
                  L, C, h : Double
                  END;

      LPcmsCIELCh = ^cmsCIELCh;

      // CIECAM97s

      cmsJCh   = PACKED RECORD
                   J, C, h : Double
                 END;

      LPcmsJCh  = ^cmsJCh;

  CONST

    AVG_SURROUND_4     = 0;
    AVG_SURROUND       = 1;
    DIM_SURROUND       = 2;
    DARK_SURROUND      = 3;
    CUTSHEET_SURROUND  = 4;

  TYPE

    cmsViewingConditions = PACKED RECORD

                WhitePoint: cmsCIEXYZ;
                Yb        : Double;
                La        : Double;
                surround  : Integer;
                D_value   : Double
              END;


    LPcmsViewingConditions = ^cmsViewingConditions;



// Input/Output

FUNCTION  cmsOpenProfileFromFile(ICCProfile: PChar; sAccess: PChar) : cmsHPROFILE; StdCall;
FUNCTION  cmsOpenProfileFromMem(MemPtr: Pointer; dwSize: DWord) : cmsHPROFILE; StdCall;
FUNCTION  cmsWhitePointFromTemp(TempK: Integer; WhitePoint: LPcmsCIExyY) : Boolean; StdCall;
FUNCTION  cmsCloseProfile(hProfile : cmsHPROFILE) : Boolean; StdCall;

FUNCTION  cmsCreateRGBProfile(WhitePoint : LPcmsCIExyY;
                                           Primaries: LPcmsCIExyYTRIPLE;
                                           TransferFunction: ARRAY OF LPGAMMATABLE) : cmsHPROFILE; StdCall;

FUNCTION  cmsCreateLabProfile(WhitePoint: LPcmsCIExyY): cmsHPROFILE; StdCall;
FUNCTION  cmsCreateXYZProfile:cmsHPROFILE; StdCall;
FUNCTION  cmsCreate_sRGBProfile:cmsHPROFILE; StdCall;

// Still experimental.
FUNCTION  _cmsSaveProfile(hProfile: cmsHPROFILE; FileName: PChar): Boolean; StdCall;


// Gamma curves

FUNCTION  cmsBuildGamma(nEntries : Integer; Gamma: Double) : LPGAMMATABLE; StdCall;
FUNCTION  cmsAllocGamma(nEntries : Integer): LPGAMMATABLE; StdCall;
PROCEDURE cmsFreeGamma(Gamma: LPGAMMATABLE); StdCall;
FUNCTION  cmsReverseGamma(nResultSamples: Integer; InGamma : LPGAMMATABLE): LPGAMMATABLE; StdCall;
FUNCTION  cmsJoinGamma(InGamma, OutGamma: LPGAMMATABLE): LPGAMMATABLE; StdCall;
FUNCTION  cmsSmoothGamma(Gamma: LPGAMMATABLE; SmoothingLambda: Double): Boolean; StdCall;

CONST

    lcmsParametricCurveExp          = 0;
    lcmsParametricCurveCIE_122_1966 = 1;
    lcmsParametricCurveIEC_61966_3  = 2;
    lcmsParametricCurveIEC_61966_2_1= 3;

FUNCTION cmsBuildParametricGamma(nEntries: Integer; TheType: Integer; Params: array of Double) : LPGAMMATABLE; StdCall;


// Access to Profile data.

FUNCTION cmsTakeMediaWhitePoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeMediaBlackPoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeIluminant(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeColorants(Dest: LPcmsCIEXYZTRIPLE; hProfile: cmsHPROFILE): Boolean; StdCall;
FUNCTION cmsTakeProductName(hProfile: cmsHPROFILE): PChar; StdCall;
FUNCTION cmsTakeProductDesc(hProfile: cmsHPROFILE): PChar; StdCall;
FUNCTION cmsIsTag(hProfile: cmsHPROFILE; sig: icTagSignature): Boolean; StdCall;
FUNCTION cmsTakeRenderingIntent( hProfile: cmsHPROFILE): Integer; StdCall;
FUNCTION cmsGetPCS(hProfile: cmsHPROFILE): icColorSpaceSignature; StdCall;
FUNCTION cmsGetColorSpace(hProfile: cmsHPROFILE): icColorSpaceSignature; StdCall;
FUNCTION cmsGetDeviceClass( hProfile: cmsHPROFILE): icProfileClassSignature; StdCall;

PROCEDURE cmsSetDeviceClass(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); StdCall;
PROCEDURE cmsSetColorSpace(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); StdCall;
PROCEDURE cmsSetPCS(hProfile: cmsHPROFILE; pcs: icColorSpaceSignature); StdCall;


FUNCTION _cmsAddTextTag(hProfile: cmsHPROFILE; sig: icTagSignature; Text: PChar): Boolean; StdCall;
FUNCTION _cmsAddXYZTag(hProfile: cmsHPROFILE;  sig: icTagSignature;  XYZ: LPcmsCIEXYZ): Boolean; StdCall;
FUNCTION _cmsAddLUTTag(hProfile: cmsHPROFILE;  sig: icTagSignature; lut: PByte): Boolean; StdCall;

CONST
        LCMS_USED_AS_INPUT   =   0;
        LCMS_USED_AS_OUTPUT  =   1;
        LCMS_USED_AS_PROOF   =   2;

FUNCTION cmsIsIntentSupported(hProfile: cmsHPROFILE; Intent, UsedDirection : Integer): Integer; StdCall;

// Transforms

FUNCTION cmsCreateTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Intent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; StdCall;

FUNCTION cmsCreateProofingTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Proofing: cmsHPROFILE;
                            Intent: Integer;
                            ProofingIntent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; StdCall;


FUNCTION cmsCreateMultiprofileTransform(hProfiles : ARRAY OF cmsHPROFILE;
                                        nProfiles : Integer;
                                        InputFormat: DWord;
                                        OutputFormat: DWord;
                                        Intent: Integer;
                                        dwFlags: DWord): cmsHTRANSFORM; StdCall;


PROCEDURE cmsDeleteTransform( hTransform: cmsHTRANSFORM); StdCall;

PROCEDURE cmsDoTransform( Transform: cmsHTRANSFORM;
                          InputBuffer: Pointer;
                          OutputBuffer: Pointer;
                          Size: LongInt); StdCall;


// CIECAM97s
FUNCTION    cmsCIECAM97sInit(pVC : LPcmsViewingConditions ) : Pointer; StdCall;
PROCEDURE   cmsCIECAM97sDone(hModel : Pointer); StdCall;

PROCEDURE   cmsCIECAM97sForward(hModel: Pointer; pIn: LPcmsCIEXYZ; pOut: LPcmsJCh ); StdCall;
PROCEDURE   cmsCIECAM97sReverse(hModel: Pointer; pIn: LPcmsJCh;   pOut: LPcmsCIEXYZ ); StdCall;


// Utils

PROCEDURE cmsXYZ2xyY(Dest: LPcmsCIExyY; Source: LPcmsCIEXYZ); StdCall;
PROCEDURE cmsxyY2XYZ(Dest: LPcmsCIEXYZ; Source: LPcmsCIExyY); StdCall;
PROCEDURE cmsXYZ2Lab(WhitePoint: LPcmsCIEXYZ; xyz: LPcmsCIEXYZ; Lab: LPcmsCIELab); StdCall;
PROCEDURE cmsLab2XYZ(WhitePoint: LPcmsCIEXYZ; Lab: LPcmsCIELab; xyz: LPcmsCIEXYZ); StdCall;
PROCEDURE cmsLabEncoded2Float(Lab: LPcmsCIELab; wLab: Pointer); StdCall;
PROCEDURE cmsFloat2LabEncoded(wLab: Pointer; Lab: LPcmsCIELab); StdCall;
PROCEDURE cmsXYZEncoded2Float(XYZ: Pointer; fxyz : LPcmsCIEXYZ); StdCall;
PROCEDURE cmsFloat2XYZEncoded(fXYZ: LPcmsCIEXYZ; XYZ: Pointer); StdCall;
PROCEDURE cmsLab2LCh(Lab: LPcmsCIELab; LCh: LPcmsCIELCh); StdCall;
PROCEDURE cmsLCh2Lab(LCh: LPcmsCIELCh; Lab: LPcmsCIELab); StdCall;

// CIELab handling

FUNCTION cmsDeltaE(Lab1, Lab2: LPcmsCIELab): Double; StdCall;
PROCEDURE cmsClampLab(Lab: LPcmsCIELab; amax, amin,  bmax, bmin: Double); StdCall;



// Gamut check
PROCEDURE cmsSetAlarmCodes(r, g, b: Integer); StdCall;

// Error handling
CONST

        LCMS_ERROR_ABORT   =  0;
        LCMS_ERROR_SHOW    =  1;
        LCMS_ERROR_IGNORE  =  2;

PROCEDURE cmsErrorAction(nAction: Integer); StdCall;


IMPLEMENTATION


// Helping functions to build format descriptors (C uses them as macros)

FUNCTION COLORSPACE_SH(e: Integer):Integer; BEGIN COLORSPACE_SH := ((e) shl 16) END;
FUNCTION SWAPFIRST_SH(e: Integer):Integer;  BEGIN SWAPFIRST_SH   := ((e) shl 13) END;
FUNCTION FLAVOR_SH(e: Integer):Integer;     BEGIN FLAVOR_SH   := ((e) shl 13) END;
FUNCTION PLANAR_SH(e: Integer):Integer;     BEGIN PLANAR_SH   := ((e) shl 12) END;
FUNCTION ENDIAN16_SH(e: Integer):Integer;   BEGIN ENDIAN16_SH := ((e) shl 11) END;
FUNCTION DOSWAP_SH(e: Integer):Integer;     BEGIN DOSWAP_SH   := ((e) shl 10) END;
FUNCTION EXTRA_SH(e: Integer):Integer;      BEGIN EXTRA_SH    := ((e) shl 7) END;
FUNCTION CHANNELS_SH(c: Integer):Integer;   BEGIN CHANNELS_SH := ((c) shl 3) END;
FUNCTION BYTES_SH(b: Integer):Integer;      BEGIN BYTES_SH    := (b) END;

CONST
   PT_ANY   =   0;   // Don't check colorspace
                     // 1 & 2 are reserved
   PT_GRAY  =   3;
   PT_RGB   =   4;
   PT_CMY   =   5;
   PT_CMYK  =   6;
   PT_YCbCr =   7;
   PT_YUV   =   8;     // Lu'v'
   PT_XYZ   =   9;
   PT_Lab   =   10;
   PT_YUVK  =   11;    // Lu'v'K
   PT_HSV   =   12;
   PT_HLS   =   13;
   PT_Yxy   =   14;


FUNCTION  cmsOpenProfileFromFile(ICCProfile : PChar; sAccess: PChar) : cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsOpenProfileFromMem(MemPtr: Pointer; dwSize: DWord) : cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsWhitePointFromTemp(TempK: Integer; WhitePoint: LPcmsCIExyY) : Boolean; EXTERNAL 'lcms.dll';
FUNCTION  cmsBuildGamma(nEntries : Integer;  Gamma: Double) : LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsAllocGamma(nEntries : Integer): LPGAMMATABLE; EXTERNAL 'lcms.dll';
PROCEDURE cmsFreeGamma(Gamma: LPGAMMATABLE); EXTERNAL 'lcms.dll';
FUNCTION  cmsReverseGamma(nResultSamples : Integer; InGamma : LPGAMMATABLE): LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsBuildParametricGamma(nEntries: Integer; TheType: Integer; Params: array of Double) : LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsJoinGamma(InGamma, OutGamma : LPGAMMATABLE): LPGAMMATABLE; EXTERNAL 'lcms.dll';
FUNCTION  cmsSmoothGamma(Gamma: LPGAMMATABLE; SmoothingLambda: Double): Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsCreateRGBProfile( WhitePoint : LPcmsCIExyY;
                                         Primaries: LPcmsCIExyYTRIPLE;
                                         TransferFunction: ARRAY OF LPGAMMATABLE) : cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsCreateLabProfile(WhitePoint: LPcmsCIExyY): cmsHPROFILE; EXTERNAL 'lcms.dll';

FUNCTION  cmsCreateXYZProfile: cmsHPROFILE; EXTERNAL 'lcms.dll';
FUNCTION  cmsCreate_sRGBProfile: cmsHPROFILE; EXTERNAL 'lcms.dll';

FUNCTION  cmsCloseProfile( hProfile : cmsHPROFILE) : Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsTakeMediaWhitePoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  cmsTakeMediaBlackPoint(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsTakeIluminant(Dest: LPcmsCIEXYZ; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  cmsTakeColorants(Dest: LPcmsCIEXYZTRIPLE; hProfile: cmsHPROFILE): Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsTakeProductName(hProfile: cmsHPROFILE): PChar; EXTERNAL 'lcms.dll';
FUNCTION  cmsTakeProductDesc(hProfile: cmsHPROFILE): PChar; EXTERNAL 'lcms.dll';
FUNCTION  cmsIsTag(hProfile: cmsHPROFILE; sig: icTagSignature): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  cmsTakeRenderingIntent( hProfile: cmsHPROFILE): Integer; EXTERNAL 'lcms.dll';
FUNCTION  cmsGetPCS(hProfile: cmsHPROFILE): icColorSpaceSignature; EXTERNAL 'lcms.dll';
FUNCTION  cmsGetColorSpace(hProfile: cmsHPROFILE): icColorSpaceSignature; EXTERNAL 'lcms.dll';
FUNCTION  cmsGetDeviceClass( hProfile: cmsHPROFILE): icProfileClassSignature; EXTERNAL 'lcms.dll';

PROCEDURE cmsSetDeviceClass(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); EXTERNAL 'lcms.dll';
PROCEDURE cmsSetColorSpace(hProfile: cmsHPROFILE; sig: icProfileClassSignature ); EXTERNAL 'lcms.dll';
PROCEDURE cmsSetPCS(hProfile: cmsHPROFILE; pcs: icColorSpaceSignature); EXTERNAL 'lcms.dll';

FUNCTION  _cmsAddTextTag(hProfile: cmsHPROFILE; sig: icTagSignature; Text: PChar): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  _cmsAddXYZTag(hProfile: cmsHPROFILE;  sig: icTagSignature;  XYZ: LPcmsCIEXYZ): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  _cmsAddLUTTag(hProfile: cmsHPROFILE;  sig: icTagSignature; lut: PByte): Boolean; EXTERNAL 'lcms.dll';
FUNCTION  _cmsSaveProfile(hProfile: cmsHPROFILE; FileName: PChar): Boolean; EXTERNAL 'lcms.dll';

FUNCTION  cmsIsIntentSupported(hProfile: cmsHPROFILE; Intent, UsedDirection : Integer): Integer; EXTERNAL 'lcms.dll';

FUNCTION  cmsCreateTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Intent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; EXTERNAL 'lcms.dll';

FUNCTION  cmsCreateProofingTransform(Input: cmsHPROFILE;
                            InputFormat: DWORD;
                            Output: cmsHPROFILE;
                            OutputFormat: DWORD;
                            Proofing: cmsHPROFILE;
                            Intent: Integer;
                            ProofingIntent: Integer;
                            dwFlags: DWord): cmsHTRANSFORM; EXTERNAL 'lcms.dll';

FUNCTION cmsCreateMultiprofileTransform(hProfiles : ARRAY OF cmsHPROFILE;
                                        nProfiles : Integer;
                                        InputFormat: DWord;
                                        OutputFormat: DWord;
                                        Intent: Integer;
                                        dwFlags: DWord): cmsHTRANSFORM; EXTERNAL 'lcms.dll';


PROCEDURE cmsDeleteTransform( hTransform: cmsHTRANSFORM); EXTERNAL 'lcms.dll';

PROCEDURE cmsDoTransform( Transform: cmsHTRANSFORM;
                          InputBuffer: Pointer;
                          OutputBuffer: Pointer;
                          Size: LongInt); EXTERNAL 'lcms.dll';

FUNCTION    cmsCIECAM97sInit(pVC : LPcmsViewingConditions ) : Pointer; EXTERNAL 'lcms.dll';
PROCEDURE   cmsCIECAM97sDone(hModel : Pointer); EXTERNAL 'lcms.dll';

PROCEDURE   cmsCIECAM97sForward(hModel: Pointer; pIn: LPcmsCIEXYZ; pOut: LPcmsJCh ); EXTERNAL 'lcms.dll';
PROCEDURE   cmsCIECAM97sReverse(hModel: Pointer; pIn: LPcmsJCh;   pOut: LPcmsCIEXYZ ); EXTERNAL 'lcms.dll';

// Utils

PROCEDURE cmsXYZ2xyY(Dest: LPcmsCIExyY; Source: LPcmsCIEXYZ); EXTERNAL 'lcms.dll';
PROCEDURE cmsxyY2XYZ(Dest: LPcmsCIEXYZ; Source: LPcmsCIExyY); EXTERNAL 'lcms.dll';
PROCEDURE cmsXYZ2Lab(WhitePoint: LPcmsCIEXYZ; xyz: LPcmsCIEXYZ; Lab: LPcmsCIELab); EXTERNAL 'lcms.dll';
PROCEDURE cmsLab2XYZ(WhitePoint: LPcmsCIEXYZ; Lab: LPcmsCIELab; xyz: LPcmsCIEXYZ); EXTERNAL 'lcms.dll';
PROCEDURE cmsLabEncoded2Float(Lab: LPcmsCIELab; wLab: Pointer); EXTERNAL 'lcms.dll';
PROCEDURE cmsFloat2LabEncoded(wLab: Pointer; Lab: LPcmsCIELab);  EXTERNAL 'lcms.dll';
PROCEDURE cmsXYZEncoded2Float(XYZ: Pointer; fxyz : LPcmsCIEXYZ); EXTERNAL 'lcms.dll';
PROCEDURE cmsFloat2XYZEncoded(fXYZ: LPcmsCIEXYZ; XYZ: Pointer); EXTERNAL 'lcms.dll';
PROCEDURE cmsLab2LCh(Lab: LPcmsCIELab; LCh: LPcmsCIELCh); EXTERNAL 'lcms.dll';
PROCEDURE cmsLCh2Lab(LCh: LPcmsCIELCh; Lab: LPcmsCIELab); EXTERNAL 'lcms.dll';

// CIELab handling

FUNCTION  cmsDeltaE(Lab1, Lab2: LPcmsCIELab): Double; EXTERNAL 'lcms.dll';
PROCEDURE cmsClampLab(Lab: LPcmsCIELab; amax, amin,  bmax, bmin: Double); EXTERNAL 'lcms.dll';


PROCEDURE cmsSetAlarmCodes(r, g, b: Integer); EXTERNAL 'lcms.dll';
PROCEDURE cmsErrorAction(nAction: Integer); EXTERNAL 'lcms.dll';


END.
