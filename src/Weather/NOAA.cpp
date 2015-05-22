/*
 * NOAA.cpp
 *
 *  Created on: Feb 12, 2015
 *      Author: root
 */

#include "NOAA.h"
#include <vector>
#include <string>
#include <map>
#include "jsoncpp/json/json.h"
#include "DCE/Logger.h"
#include <iostream>


using namespace std;
using namespace DCE;

NOAA::NOAA() {
/**	f_temp_F_.clear();
	f_text_.clear();
	f_weather_.clear();
	f_icon_.clear();
	wrap_.clear();
	dayl_.clear();
	icons_.clear();**/

	icons_.insert(make_pair("","1"));//,	//Unknown
	icons_.insert(make_pair("skc","2"));	//Sunny
	icons_.insert(make_pair("nskc","3"));icons_.insert(make_pair("nfew","3"));icons_.insert(make_pair("nsct","3"));icons_.insert(make_pair("nbkn","3"));	//Night Clear
	icons_.insert(make_pair("ovc","4"));icons_.insert(make_pair("novc","4"));icons_.insert(make_pair("smoke","4"));	//Cloudy
	icons_.insert(make_pair("bkn","5"));	//Moderately Cloudy
	icons_.insert(make_pair("few","6"));icons_.insert(make_pair("sct","6"));	//Partly Cloudy
	icons_.insert(make_pair("fg","7"));icons_.insert(make_pair("nfg","7"));icons_.insert(make_pair("dust","7"));icons_.insert(make_pair("mist","7"));	//Fog
	icons_.insert(make_pair("ra1","8"));icons_.insert(make_pair("nra1","8"));	//Light Showers
	icons_.insert(make_pair("shra","9"));icons_.insert(make_pair("hi_shwrs","9"));icons_.insert(make_pair("hi_nshwrs","9"));icons_.insert(make_pair("ra","9"));icons_.insert(make_pair("nra","9"));	//Showers
	icons_.insert(make_pair("tsra","10"));icons_.insert(make_pair("ntsra","10"));icons_.insert(make_pair("hi_tsra","10"));icons_.insert(make_pair("hi_ntsra","10"));icons_.insert(make_pair("nsvrtsra","10"));	//Thunder Showers
	icons_.insert(make_pair("sn","11"));icons_.insert(make_pair("nsn","11"));	//Snow Showers
	icons_.insert(make_pair("fzra","12"));icons_.insert(make_pair("ip","12"));icons_.insert(make_pair("mix","12"));icons_.insert(make_pair("nmix","12"));icons_.insert(make_pair("raip","12"));icons_.insert(make_pair("rasn","12"));icons_.insert(make_pair("nrasn","12"));icons_.insert(make_pair("fzrara","12"));	//Rain Snow

	const string str[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Washington's Birthday"};
	set<string> dayl_t(str,str + sizeof(str)/sizeof(str[0]));
	dayl_=dayl_t;
}

NOAA::~NOAA() {
	// TODO Auto-generated destructor stub
}

bool NOAA::Get_NOAA(const string& data,const std::string& units,const std::string& lang){

	Json::Value root;
	Json::Reader reader;
       if (units=="1")
       {
	  d_units_="C";
	}else {
	    d_units_="F";
        }
        d_lang_=lang;


    bool parsingSuccessful = reader.parse(data, root);


    if(!parsingSuccessful)
    {
        //cout << "Failed to parse configuration. " << reader.getFormatedErrorMessages();
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Get_NOAA: Failed to parse configuration");
        return false;
    }

    //Set the global index
    set_index(root["time"]["startPeriodName"]);

    //cout << root;

    parse_data(root);

	return true;
}

void NOAA::parse_data(Json::Value const &root){
    set_date(root.get("creationDateLocal","No Date").asString());

    //Parse Days
    const Json::Value d_data = root["time"]["startPeriodName"];
    //cout << d_data;
    set_f_days(d_data);

    //Parse current conditions
    const Json::Value c_data = root["currentobservation"];
    //cout<< c_data;

    set_temp_F(c_data.get("Temp","No Data").asString());
    set_winds(c_data.get("Winds","No Data").asString());
    set_windd(c_data.get("Windd","No Data").asString());
    set_vis(c_data.get("Visibility","No Data").asString());
    set_slp(c_data.get("SLP","No Data").asString());
    set_wc(c_data.get("WindChill","No Data").asString());
    set_relh(c_data.get("Relh","No Data").asString());
    set_weatherimage(c_data.get("Weatherimage","No Data").asString());
    set_weatherc(c_data.get("Weather","No Data").asString());
    set_areadisc(root["location"].get("areaDescription","No Data").asString());

    //Parse forecast
    const Json::Value w_data = root["data"];
    //cout << w_data;
    set_f_temp_F(w_data["temperature"]);
    set_f_text(w_data["text"]);
    set_f_weather(w_data["weather"]);
    set_f_icon(w_data["iconLink"]);
    radar_ = "http://www.weather.gov/images/nws/satellite_images/3.jpg|http://www.weather.gov/images/nws/satellite_images/2.jpg|http://www.weather.gov/images/nws/satellite_images/1.jpg";
    //radar_ += "http://www.weather.gov/images/nws/satellite_images/4.jpg|http://www.weather.gov/images/nws/satellite_images/5.jpg|http://www.weather.gov/images/nws/satellite_images/6.jpg";
   // radar_ += "http://www.weather.gov/images/nws/satellite_images/7.jpg|http://www.weather.gov/images/nws/satellite_images/8.jpg|http://www.weather.gov/images/nws/satellite_images/9.jpg";
   // radar_ += "http://www.weather.gov/images/nws/satellite_images/10.jpg|http://www.weather.gov/images/nws/satellite_images/11.jpg|http://www.weather.gov/images/nws/satellite_images/12.jpg";
   // radar_ += "http://www.weather.gov/images/nws/satellite_images/13.jpg|http://www.weather.gov/images/nws/satellite_images/14.jpg|http://www.weather.gov/images/nws/satellite_images/15.jpg";
   // radar_ += "http://www.weather.gov/images/nws/satellite_images/16.jpg|http://www.weather.gov/images/nws/satellite_images/17.jpg|http://www.weather.gov/images/nws/satellite_images/18.jpg";


}

vector<vector<string> > NOAA::get_msg(){

	set_wrap("temp_current",temp_F_+"°F",temp_F_);
	set_wrap("temp_tonight",f_temp_F_[0]+"°F",f_temp_F_[0]);
	set_wrap("cond_tonight",f_weather_[0],f_icon_[0]);
	set_wrap("forecast_tonight",f_text_[0]);
	set_wrap("cond_current",weatherc_,weatherimage_);
	set_wrap("humidity_current",relh_+"%",relh_);
	set_wrap("pressure_current",slp_+" Hg",slp_);
	set_wrap("wind_current",winds_+" mph",winds_);
	set_wrap("direction_current",windd_+" deg",windd_);
	set_wrap("visibility_current",vis_+" mi",vis_);
	set_wrap("feelslike_current",wc_+"°F",wc_);
	set_wrap("location",areadisc_);
	set_wrap("data_age",date_);
	set_wrap("cond_day1",f_weather_[index_],f_icon_[index_]);
	set_wrap("cond_day2",f_weather_[index_+2],f_icon_[index_+2]);
	set_wrap("cond_day3",f_weather_[index_+4],f_icon_[index_+4]);
	set_wrap("cond_day4",f_weather_[index_+6],f_icon_[index_+6]);
	set_wrap("cond_day5",f_weather_[index_+8],f_icon_[index_+8]);
	set_wrap("cond_day6",f_weather_[index_+10],f_icon_[index_+10]);
	set_wrap("temp_hi_day1",f_temp_F_[index_]+"°F",f_temp_F_[index_]);
	set_wrap("temp_lo_day1",f_temp_F_[index_+1]+"°F",f_temp_F_[index_+1]);
	set_wrap("temp_hi_day2",f_temp_F_[index_+2]+"°F",f_temp_F_[index_+2]);
	set_wrap("temp_lo_day2",f_temp_F_[index_+3]+"°F",f_temp_F_[index_+3]);
	set_wrap("temp_hi_day3",f_temp_F_[index_+4]+"°F",f_temp_F_[index_+4]);
	set_wrap("temp_lo_day3",f_temp_F_[index_+5]+"°F",f_temp_F_[index_+5]);
	set_wrap("temp_hi_day4",f_temp_F_[index_+6]+"°F",f_temp_F_[index_+6]);
	set_wrap("temp_lo_day4",f_temp_F_[index_+7]+"°F",f_temp_F_[index_+7]);
	set_wrap("temp_hi_day5",f_temp_F_[index_+8]+"°F",f_temp_F_[index_+8]);
	set_wrap("temp_lo_day5",f_temp_F_[index_+9]+"°F",f_temp_F_[index_+9]);
	set_wrap("temp_hi_day6",f_temp_F_[index_+10]+"°F",f_temp_F_[index_+10]);

	//This could overrun the vector on occasion so we will check
	if(index_+11<f_temp_F_.size())
		set_wrap("temp_lo_day6",f_temp_F_[index_+11]+"°F",f_temp_F_[index_+11]);

	set_wrap("dow_day1",f_days_[index_]);
	set_wrap("dow_day2",f_days_[index_+2]);
	set_wrap("dow_day3",f_days_[index_+4]);
	set_wrap("dow_day4",f_days_[index_+6]);
	set_wrap("dow_day5",f_days_[index_+8]);
	set_wrap("dow_day6",f_days_[index_+10]);
	set_wrap("1",radar_,"Radar");

	return wrap_;
}

void NOAA::set_temp_F(const string &temp_f){
	temp_F_=temp_f;
}

void NOAA::set_vis(const string &vis){
	vis_=vis;
}

void NOAA::set_slp(const string &slp){
	slp_=slp;
}

void NOAA::set_wc(const string &wc){
	wc_=wc;
}

void NOAA::set_date(const string &date){
	date_=date;
}

void NOAA::set_winds(const string &winds){
	winds_=winds;
}

void NOAA::set_windd(const string &windd){
	windd_=windd;
}

void NOAA::set_relh(const string &relh){
	relh_=relh;
}

void NOAA::set_weatherimage(const string &weatherimage){
	weatherimage_=get_icon(weatherimage);
}

void NOAA::set_weatherc(const string &weatherc){
	weatherc_=weatherc;
}

void NOAA::set_areadisc(const string &areadisc){
	areadisc_=areadisc;
}

void NOAA::set_f_temp_F(Json::Value const & f_temp_F){
	for ( unsigned int index = 0; index < f_temp_F.size(); ++index )
		f_temp_F_.push_back(f_temp_F[index].asString());
}

void NOAA::set_f_text(Json::Value const &f_text){
	for ( unsigned int index = 0; index < f_text.size(); ++index )
		f_text_.push_back(f_text[index].asString());
}

void NOAA::set_f_weather(Json::Value const &f_weather){
	for ( unsigned int index = 0; index < f_weather.size(); ++index )
		f_weather_.push_back(f_weather[index].asString());
}

void NOAA::set_f_icon(Json::Value const & f_icon){
	for ( unsigned int index = 0; index < f_icon.size(); ++index )
		f_icon_.push_back(get_icon(f_icon[index].asString()));
}

void NOAA::set_f_days(Json::Value const & days){
	for ( unsigned int index = 0; index < days.size(); ++index )
		f_days_.push_back(days[index].asString());
}

string NOAA::get_icon(const string &url){
	string icon,str;

	//Strip out all not needed characters

	//Check if url
	if(url.find("/") != string::npos){
		size_t pos = url.find_last_of("/");
		str=url.substr(pos+1);
	}else{
		str=url;
	}
	size_t pos2 = str.find(".");
	string str2 = str.substr(0,pos2);
	size_t pos3 = str2.find_first_of("0987654321");
	string str3 = str2.substr(0,pos3);

	if(icons_.count(str3))
		icon = icons_.find(str3)->second;

	return icon;
}

void NOAA::set_index(Json::Value const &data){
	for ( unsigned int index = 1; index < data.size(); ++index ){
		string str = data[index].asString();

		if (NOAA::dayl_.find(str) != NOAA::dayl_.end()){
			index_=index;
			return;
		}
	}
}

void NOAA::set_wrap(const std::string& str1,const std::string& str2,const std::string& str3){
	vector<string> str;
	if(str3==""){
		string msg[2];
		msg[0]=str1;
		msg[1]=str2;
		str = makeVector(msg);
	}else{
		string msg[3];
		msg[0]=str1;
		msg[1]=str2;
		msg[2]=str3;
		str = makeVector(msg);
	}
	wrap_.push_back(str);
}

