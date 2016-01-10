#ifndef HUEBULB_H
#define HUEBULB_H
//On	bool	On/Off state of the light. On=true, Off=false
//Bri	uint8	Brightness of the light. This is a scale from the minimum brightness the light is capable of, 0, to the maximum capable brightness, 255. Note a brightness of 0 is not off.
//Hue	uint16w	Hue of the light. This is a wrapping value between 0 and 65535. Both 0 and 65535 are red, 25500 is green and 46920 is blue.
//sat	uint8	Saturation of the light. 255 is the most saturated (colored) and 0 is the least saturated (white).
//xy	list 2..2 of float 4	The x and y coordinates of a color in CIE color space.

//The first entry is the x coordinate and the second entry is the y coordinate. Both x and y are between 0 and 1.
//ct	uint16	The Mired Color temperature of the light. 2012 connected lights are capable of 153 (6500K) to 500 (2000K).
//alert	string	The alert effect, which is a temporary change to the bulb’s state. This can take one of the following values:
//“none” – The light is not performing an alert effect.
//“select” – The light is performing one breathe cycle.
//“lselect” – The light is performing breathe cycles for 30 seconds or until an "alert": "none" command is received.

//Note that in version 1.0 this contains the last alert sent to the light and not its current state. This will be changed to contain the current state in an upcoming patch.
//effect	string	The dynamic effect of the light, can either be “none” or “colorloop”.

//If set to colorloop, the light will cycle through all hues using the current brightness and saturation settings.
//colormode	string 2, 2	Indicates the color mode in which the light is working, this is the last command type it received. Values are “hs” for Hue and Saturation, “xy” for XY and “ct” for Color Temperature. This parameter is only present when the light supports at least one of the values.
//reachable	bool	Indicates if a light can be reached by the bridge. Currently always returns true, functionality will be added in a future patch.

#include <QObject>
#include <QMap>
#include <abstractwirelessbulb.h>
class QVariant;
class HueBulb : public AbstractWirelessBulb
{
    Q_OBJECT
public:
    explicit HueBulb(HueControllerHardware *hw);
    HueBulb(){}

     QString resetColor;
signals:
    
public slots:
    void proccessStateInformation(QVariantMap d);
    
};

#endif // HUEBULB_H
