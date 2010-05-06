#ifndef __DISPLAYRESCREEN_H__
#define __DISPLAYRESCREEN_H__

#include <vector>
#include <map>
#include <algorithm>

#include <QString>

#include "mythexp.h"

class MPUBLIC DisplayResScreen
{
  public:
    // Constructors, initializers
    DisplayResScreen()
        : width(0), height(0), width_mm(0), height_mm(0), aspect(-1.0), custom(false) {;}
    DisplayResScreen(int w, int h, int mw, int mh,
                     double aspectRatio/* = -1.0*/, double refreshRate/* = 0*/);
    DisplayResScreen(int w, int h, int mw, int mh,
                     const std::vector<double>& refreshRates);
    DisplayResScreen(int w, int h, int mw, int mh,
                     const std::vector<double>& refreshRates, const std::map<double, short>& realrates);
    DisplayResScreen(int w, int h, int mw, int mh,
                     const double* refreshRates, uint rr_length);
    DisplayResScreen(int w, int h, int mw, int mh,
                     const short* refreshRates, uint rr_length);
    DisplayResScreen(const QString &str);
    inline void Init();

    // Gets
    int Width() const { return width; }
    int Height() const { return height; }
    int Width_mm() const { return width_mm; }
    int Height_mm() const { return height_mm; }
    bool Custom() const { return custom; }

    inline double AspectRatio() const;
    inline double RefreshRate() const;
    const std::vector<double>& RefreshRates() const { return refreshRates; }

    // Sets, adds
    void SetAspectRatio(double a);
    void AddRefreshRate(double rr)
    {
        refreshRates.push_back(rr);
        std::sort(refreshRates.begin(), refreshRates.end());
    }
    void ClearRefreshRates(void)
    {
        refreshRates.clear();
    }
    void SetCustom(bool b)
    {
        custom = b;
    }

    // Map for matching real rates and xrandr rate;
    std::map<double, short> realRates;

    // Converters & comparitors
    QString toString() const;
    inline bool operator < (const DisplayResScreen& b) const;
    inline bool operator == (const DisplayResScreen &b) const;

    // Statics
    static QStringList Convert(const std::vector<DisplayResScreen>& dsr);
    static std::vector<DisplayResScreen> Convert(const QStringList& slist);
    static int FindBestMatch(const std::vector<DisplayResScreen>& dsr,
                             const DisplayResScreen& d,
                             double& target_rate);
    static inline int CalcKey(int w, int h, double rate);
    static bool compare_rates(double f1, double f2, double precision = 0.01);

  private:
    int width, height; // size in pixels
    int width_mm, height_mm; // physical size in millimeters
    double aspect; // aspect ratio, calculated or set
    std::vector<double> refreshRates;
    bool custom; // Set if resolution was defined manually
};

typedef std::vector<DisplayResScreen>          DisplayResVector;
typedef DisplayResVector::iterator        DisplayResVectorIt;
typedef DisplayResVector::const_iterator  DisplayResVectorCIt;

typedef std::map<uint, class DisplayResScreen> DisplayResMap;
typedef DisplayResMap::iterator           DisplayResMapIt;
typedef DisplayResMap::const_iterator     DisplayResMapCIt;

inline void DisplayResScreen::Init()
{
    width = height = width_mm = height_mm = 0;
    aspect = -1.0;
}

inline double DisplayResScreen::AspectRatio() const
{
    if (aspect<=0.0)
    {
        if (0 == height_mm)
            return 1.0;
        return ((double)(width_mm))/((double)(height_mm));
    }
    return aspect;
}

inline double DisplayResScreen::RefreshRate() const
{
    if (refreshRates.size() >= 1)
        return refreshRates[0];
    else return 0.0;
}

inline bool DisplayResScreen::operator < (const DisplayResScreen& b) const
{
    if (width < b.width)
        return true;
    if (height < b.height)
        return true;
    return false;
}
            
inline bool DisplayResScreen::operator == (const DisplayResScreen &b) const
{
    return width == b.width && height == b.height;
}  

inline int DisplayResScreen::CalcKey(int w, int h, double rate)
{
    int irate = (int) (rate * 1000.0);
    return (w << 19) | (h << 5) | irate;
}

#endif // __DISPLAYRESCREEN_H__
