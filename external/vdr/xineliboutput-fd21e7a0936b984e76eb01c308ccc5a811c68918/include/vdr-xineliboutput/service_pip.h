/*
 * vdr-xineliboutput/service_pip.h: xineliboutput Picture-In-Picture service interface
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id: service_pip.h,v 1.1 2010-03-12 21:15:05 phintuka Exp $
 *
 */

#ifndef VDR_XINELIBOUTPUT_SERVICE_PIP_H
#define VDR_XINELIBOUTPUT_SERVICE_PIP_H

/*
 * Example:
 *
 * cXineliboutputPipService *pip_if;
 *
 * if (Plugin->Service(XINELIBOUTPUT_SERVICE_PIP_ID, &pip_if)) {
 *   while(...)
 *     pip_if->PlayTs(...);
 *   delete pip_if;
 * }
 *
 */

#define XINELIBOUTPUT_SERVICE_PIP_ID      "Xineliboutput_Pip_Open"

#define XINELIBOUTPUT_SERVICE_PIP_VERSION 0x0100

class cXineliboutputPipService {

  public:

    /* Set location and size (in % of full screen)
     * Example:
     *   Blend PIP picture to left-top corner, resize to 20% of full screen:
     *     SetPosition(5, 5, 20, 20);
     */
    virtual void SetPosition(uint X, uint Y, uint W, uint H) = 0;

    /* PIP input: single MPEG-TS video packet */
    virtual int  PlayTs(const uint8_t *Data) = 0;

    virtual ~cXineliboutputPipService() {};
};


#endif /* VDR_XINELIBOUTPUT_SERVICE_PIP_H */
