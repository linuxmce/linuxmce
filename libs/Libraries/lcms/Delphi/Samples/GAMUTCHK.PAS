PROGRAM GamutChk;

USES lcmsdll, Windows;


VAR
        hLabProfile, hDisplayProfile, hProof: cmsHPROFILE;
        ProofTransform : cmsHTRANSFORM;
        Lab: cmsCIELab;
        wLab: ARRAY [0..2] OF WORD;
        wRGB: ARRAY [0..2] OF Byte;

BEGIN
     WriteLn('Gamut check test');

     hLabProfile     := cmsCreateLabProfile(NIL);
     hDisplayProfile := cmsCreate_sRGBProfile();
     hProof          := cmsOpenProfileFromFile('Epson 860.icm', 'r');


    ProofTransform:=cmsCreateProofingTransform(hLabProfile,
                        TYPE_Lab_16,
                        hDisplayProfile,
                        TYPE_RGB_8,
                        hProof,
                        INTENT_RELATIVE_COLORIMETRIC,
                        INTENT_ABSOLUTE_COLORIMETRIC,
                        cmsFLAGS_GAMUTCHECK OR cmsFLAGS_NOTPRECALC);


    cmsSetAlarmCodes(255,255,255);


    Write('L=?'); ReadLn(Lab.L);
    Write('a=?'); ReadLn(Lab.a);
    Write('b=?'); ReadLn(Lab.b);

    cmsFloat2LabEncoded(@wLab, @Lab);

    cmsDoTransform(ProofTransform, @wLab, @wRGB, 1);

    WriteLn('R=', wRGB[0], ' G=', wRGB[1], ' B=', wRGB[2]);

    cmsCloseProfile(hLabProfile);
    cmsCloseProfile(hDisplayProfile);
    cmsCloseProfile(hProof);
    cmsDeleteTransform(ProofTransform);

END.



