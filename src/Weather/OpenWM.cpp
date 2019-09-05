/*
 * OpenWM.cpp
 *
 *  Created on: Feb 21, 2015
 *      Author: jim
 */

#include "OpenWM.h"
#include <string>
#include "jsoncpp/json/json.h"
#include "DCE/Logger.h"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;
using namespace DCE;

OpenWM::OpenWM() {

	api_key="9ccae8a11a0069f2c3c3ed5b3afff3fe";

	icons_.insert(make_pair("","1"));//,	//Unknown
	icons_.insert(make_pair("01d","2"));	//Sunny
	icons_.insert(make_pair("01n","3"));icons_.insert(make_pair("02n","3"));	//Night Clear
	icons_.insert(make_pair("04d","4"));icons_.insert(make_pair("04n","4"));	//Cloudy
	icons_.insert(make_pair("03d","5"));icons_.insert(make_pair("03n","5"));	//Moderately Cloudy
	icons_.insert(make_pair("02d","6"));icons_.insert(make_pair("02n","6"));	//Partly Cloudy
	icons_.insert(make_pair("50d","7"));icons_.insert(make_pair("50n","7"));	//Fog
	icons_.insert(make_pair("10d","8"));icons_.insert(make_pair("10n","8"));	//Light Showers
	icons_.insert(make_pair("09d","9"));icons_.insert(make_pair("09n","9"));	//Showers
	icons_.insert(make_pair("11d","10"));icons_.insert(make_pair("11n","10"));	//Thunder Showers
	icons_.insert(make_pair("13d","11"));icons_.insert(make_pair("13n","11"));	//Snow Showers
	//icons_.insert(make_pair("fzra","12"));icons_.insert(make_pair("ip","12"));icons_.insert(make_pair("mix","12"));icons_.insert(make_pair("nmix","12"));icons_.insert(make_pair("raip","12"));icons_.insert(make_pair("rasn","12"));icons_.insert(make_pair("nrasn","12"));icons_.insert(make_pair("fzrara","12"));	//Rain Snow

}

OpenWM::~OpenWM() {
	// TODO Auto-generated destructor stub
}

std::string OpenWM::get_cid(const string & data){
	Json::Value root;
	Json::Reader reader;

    bool parsingSuccessful = reader.parse(data, root);

	cout << root.toStyledString() << endl;

    if(!parsingSuccessful){
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OWM::get_cid: Failed to parse configuration");
        return string("");

    }
    if(root.get("cod",500).asInt()==404){
    	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OWM::get_cid:Error: %s",root.get("message","??").asCString());
    	return string("");
    }
    const int id=root.get("id",0).asInt();
	ostringstream s;
	s << id;
	return s.str();
}

bool OpenWM::parse_cur(const string &data, const string &units){
	Json::Value root;
	Json::Reader reader;

    bool parsingSuccessful = reader.parse(data, root);

	cout << root.toStyledString() << endl;

    if(!parsingSuccessful){
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OWM::set_cur: Failed to parse configuration");
        return false;
    }
    if(root.get("cod",500).asInt()==404){
    	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OWM::get_cur:Error: %s",root.get("message","??").asCString());
    	return false;
    }
    set_date((time_t)root.get("dt",0).asInt());
    if(units=="imperial"){
    	set_temp_I(root["main"].get("temp",0).asDouble());
    	set_winds_I(root["wind"].get("speed",0).asDouble());
    }
    if(units=="metric"){
    	set_temp_M(root["main"].get("temp",0).asDouble());
    	set_winds_M(root["wind"].get("speed",0).asDouble());
    }
    set_slp(root["main"].get("pressure",0).asDouble());
    set_windd(root["wind"].get("deg",0).asDouble());
    set_relh(root["wind"].get("humidity",0).asDouble());
    set_areadisc(root["sys"].get("country","").asString(),root.get("name","").asString());
    set_weatherc(root["weather"][0].get("description","").asString());
    set_weatherimage(root["weather"][0].get("icon","").asString());

	return true;
}

bool OpenWM::parse_forc(const string &res, const string &units){
	Json::Value root;
	Json::Reader reader;

    bool parsingSuccessful = reader.parse(res, root);

	cout << root.toStyledString() << endl;

    if(!parsingSuccessful){
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OWM::set_cur: Failed to parse configuration");
        return false;
    }
    string i =root.get("cod",500).asString();
    if(root.get("cod","500").asString()=="404"){
    	LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"OWM::get_cur:Error: %s",root.get("message","??").asCString());
    	return false;
    }
    if(units=="imperial"){
    	set_f_temp(root["list"],"°F");
    }
    if(units=="metric"){
    	set_f_temp(root["list"],"°C");
    }
    set_f_weather(root["list"]);
    set_f_icon(root["list"]);
    set_f_days(root["list"]);
	return true;
}

vector<vector<string> > OpenWM::get_msg(){

	set_wrap("temp_current",s_temp_,temp_);
	//set_wrap("temp_tonight",f_temp_F_[0]+"°F",f_temp_F_[0]);
	//set_wrap("cond_tonight",f_weather_[0],f_icon_[0]);
	//set_wrap("forecast_tonight",f_text_[0]);
	set_wrap("cond_current",weatherc_,weatherimage_);
	set_wrap("humidity_current",s_relh_,relh_);
	set_wrap("pressure_current",s_slp_,slp_);
	set_wrap("wind_current",s_winds_,winds_);
	set_wrap("direction_current",s_windd_,windd_);
	//set_wrap("visibility_current",s_vis_,vis_);
	//set_wrap("feelslike_current",wc_+"°F",wc_);
	set_wrap("location",areadisc_);
	set_wrap("data_age",date_);
	set_wrap("cond_day1",f_weather_[0],f_icon_[0]);
	set_wrap("cond_day2",f_weather_[1],f_icon_[1]);
	set_wrap("cond_day3",f_weather_[2],f_icon_[2]);
	set_wrap("cond_day4",f_weather_[3],f_icon_[3]);
	set_wrap("cond_day5",f_weather_[4],f_icon_[4]);
	set_wrap("cond_day6",f_weather_[5],f_icon_[5]);
	set_wrap("temp_hi_day1",s_f_temp_max_[0],f_temp_max_[0]);
	set_wrap("temp_lo_day1",s_f_temp_min_[0],f_temp_min_[0]);
	set_wrap("temp_hi_day2",s_f_temp_max_[1],f_temp_max_[1]);
	set_wrap("temp_lo_day2",s_f_temp_min_[1],f_temp_min_[1]);
	set_wrap("temp_hi_day3",s_f_temp_max_[2],f_temp_max_[2]);
	set_wrap("temp_lo_day3",s_f_temp_min_[2],f_temp_min_[2]);
	set_wrap("temp_hi_day4",s_f_temp_max_[3],f_temp_max_[3]);
	set_wrap("temp_lo_day4",s_f_temp_min_[3],f_temp_min_[3]);
	set_wrap("temp_hi_day5",s_f_temp_max_[4],f_temp_max_[4]);
	set_wrap("temp_lo_day5",s_f_temp_min_[4],f_temp_min_[4]);
	set_wrap("temp_hi_day6",s_f_temp_max_[5],f_temp_max_[5]);
	set_wrap("temp_lo_day6",s_f_temp_min_[5],f_temp_min_[5]);

	set_wrap("dow_day1",f_days_[0]);
	set_wrap("dow_day2",f_days_[1]);
	set_wrap("dow_day3",f_days_[2]);
	set_wrap("dow_day4",f_days_[3]);
	set_wrap("dow_day5",f_days_[4]);
	set_wrap("dow_day6",f_days_[5]);

	return wrap_;
}

void OpenWM::set_date(time_t date){
	struct tm * timeinfo;
	timeinfo = localtime (&date);
	date_=asctime(timeinfo);
}

void OpenWM::set_temp_I(double temp_F){
	ostringstream s;
	int t = round(temp_F);
	s << t;
	s_temp_=s.str()+"°F";
	temp_=s.str();
}

void OpenWM::set_slp(double slp){
	ostringstream s;
	int si = round(slp);
	s << si;
	s_slp_=s.str()+"mb";
	slp_=s.str();

}

void OpenWM::set_temp_M(double temp_M){
	ostringstream s;
	int t = round(temp_M);
	s << t;
	s_temp_=s.str()+"°C";
	temp_=s.str();

}

void OpenWM::set_winds_I(double winds){
	ostringstream s;
	int w = round(winds);
	s << w;
	s_winds_=s.str()+"mph";
	winds_=s.str();
}

void OpenWM::set_winds_M(double winds){
	ostringstream s;
	int w = round(winds);
	s << w;
	s_winds_=s.str()+"kph";
	winds_=s.str();
}

void OpenWM::set_windd(double windd){
	ostringstream s;
	int w = round(windd);
	s << w;
	s_windd_=s.str()+"deg";
	windd_=s.str();
}

void OpenWM::set_relh(double relh){
	ostringstream s;
	int r = round(relh);
	s << r;
	s_windd_=s.str()+"";
	windd_=s.str();
}

void OpenWM::set_areadisc(string const &country, string const &city){
	areadisc_=city+", "+country;
}

void OpenWM::set_weatherc(const string &disc){
	weatherc_=disc;
}

string OpenWM::get_icon(const string &url){
	string icon;

	if(icons_.count(url))
		icon = icons_.find(url)->second;

	return icon;
}

void OpenWM::set_f_temp(Json::Value const & f_temp, const string &str){
	for ( unsigned int index = 0; index < f_temp.size(); ++index ){

		ostringstream s,s1;
		s << round(f_temp[index]["temp"].get("min","??").asDouble());
		f_temp_min_.push_back(s.str());
		s_f_temp_min_.push_back(s.str()+str);

		s1 << round(f_temp[index]["temp"].get("max","??").asDouble());
		f_temp_max_.push_back(s1.str());
		s_f_temp_max_.push_back(s1.str()+str);
	}

}
void OpenWM::set_f_weather(Json::Value const &f_weather){
	for ( unsigned int index = 0; index < f_weather.size(); ++index )
		f_weather_.push_back(f_weather[index]["weather"][0].get("description","??").asString());
}

void OpenWM::set_f_icon(Json::Value const & f_icon){
	for ( unsigned int index = 0; index < f_icon.size(); ++index )
		f_icon_.push_back(get_icon(f_icon[index]["weather"][0].get("icon","??").asString()));
}

void OpenWM::set_f_days(Json::Value const & days){
	for ( unsigned int index = 0; index < days.size(); ++index ){
		time_t t = (time_t)days[index].get("dt",0).asInt();
		struct tm * timeinfo;
		timeinfo = localtime (&t);
		switch (timeinfo->tm_wday){
		case 0:
			f_days_.push_back("Sunday");
			break;
		case 1:
			f_days_.push_back("Monday");
			break;
		case 2:
			f_days_.push_back("Tuesday");
			break;
		case 3:
			f_days_.push_back("Wednesday");
			break;
		case 4:
			f_days_.push_back("Thursday");
			break;
		case 5:
			f_days_.push_back("Friday");
			break;
		case 6:
			f_days_.push_back("Saturday");
			break;
		default:
			f_days_.push_back("??");
			break;
		}

	}
}

void OpenWM::set_wrap(const string& str1,const string& str2,const string& str3){
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

void OpenWM::set_weatherimage(const string &weatherimage){
	weatherimage_=get_icon(weatherimage);
}
