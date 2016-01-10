/*
 * WWO.cpp
 *
 *  Created on: Feb 12, 2015
 *      Author: root
 */

#include "WWO.h"
#include <vector>
#include <string>
#include <map>
#include "jsoncpp/json/json.h"
#include "DCE/Logger.h"
#include <iostream>


using namespace std;
using namespace DCE;

WWO::WWO() {
	f_temp_F_.clear();
	f_text_.clear();
	f_weather_.clear();
	f_icon_.clear();
        d_units_="F";
        d_lang_="en";
	icons_.insert(make_pair("182","1"));icons_.insert(make_pair("185","1"));icons_.insert(make_pair("227","1"));icons_.insert(make_pair("230","1"));icons_.insert(make_pair("281","1"));icons_.insert(make_pair("311","1"));icons_.insert(make_pair("314","1"));icons_.insert(make_pair("317","1"));icons_.insert(make_pair("","1"));//,	//Unknown
	icons_.insert(make_pair("113","2"));	//Sunny
	icons_.insert(make_pair("113N","3"));icons_.insert(make_pair("nfew","3"));icons_.insert(make_pair("nsct","3"));icons_.insert(make_pair("nbkn","3"));	//Night Clear
	icons_.insert(make_pair("119","4"));icons_.insert(make_pair("119","4"));icons_.insert(make_pair("smoke","4"));	//Cloudy
	icons_.insert(make_pair("122","5"));icons_.insert(make_pair("143","5"));	//Moderately Cloudy
	icons_.insert(make_pair("116","6"));icons_.insert(make_pair("176","6"));	//Partly Cloudy
	icons_.insert(make_pair("260","7"));icons_.insert(make_pair("248","7"));icons_.insert(make_pair("dust","7"));icons_.insert(make_pair("mist","7"));	//Fog
	icons_.insert(make_pair("266","8"));icons_.insert(make_pair("353","8"));icons_.insert(make_pair("362","8"));icons_.insert(make_pair("365","8"));icons_.insert(make_pair("386","8"));icons_.insert(make_pair("389","8"));icons_.insert(make_pair("302","8"));	//Light Showers
	icons_.insert(make_pair("305","9"));icons_.insert(make_pair("293","9"));icons_.insert(make_pair("263","9"));icons_.insert(make_pair("356","9"));icons_.insert(make_pair("359","9"));icons_.insert(make_pair("308","9"));icons_.insert(make_pair("hi_shwrs","9"));icons_.insert(make_pair("hi_nshwrs","9"));icons_.insert(make_pair("ra","9"));icons_.insert(make_pair("299","9"));	//Showers
	icons_.insert(make_pair("200","10"));icons_.insert(make_pair("296","10"));icons_.insert(make_pair("ntsra","10"));icons_.insert(make_pair("hi_tsra","10"));icons_.insert(make_pair("hi_ntsra","10"));icons_.insert(make_pair("nsvrtsra","10"));	//Thunder Showers
	icons_.insert(make_pair("284","11"));icons_.insert(make_pair("320","11"));icons_.insert(make_pair("323","11"));icons_.insert(make_pair("329","11"));icons_.insert(make_pair("332","11"));icons_.insert(make_pair("335","11"));icons_.insert(make_pair("338","11"));icons_.insert(make_pair("392","11"));icons_.insert(make_pair("395","11"));icons_.insert(make_pair("326","11"));icons_.insert(make_pair("nsn","11"));	//Snow Showers
	icons_.insert(make_pair("179","12"));icons_.insert(make_pair("358","12"));icons_.insert(make_pair("350","12"));icons_.insert(make_pair("368","12"));icons_.insert(make_pair("371","12"));icons_.insert(make_pair("377","12"));icons_.insert(make_pair("374","12"));icons_.insert(make_pair("ip","12"));icons_.insert(make_pair("mix","12"));icons_.insert(make_pair("nmix","12"));icons_.insert(make_pair("raip","12"));icons_.insert(make_pair("rasn","12"));icons_.insert(make_pair("nrasn","12"));icons_.insert(make_pair("fzrara","12"));	//Rain Snow
	icons_.insert(make_pair("122N","5"));icons_.insert(make_pair("143N","14"));
	icons_.insert(make_pair("116N","15"));icons_.insert(make_pair("176N","15"));



}

WWO::~WWO() {
	// TODO Auto-generated destructor stub
}

bool WWO::Get_WWO(const string& data,const std::string& units,const std::string& lang,const std::string& radar){

	Json::Value root;
	Json::Reader reader;
        d_units_=units;
        d_lang_=lang;
        d_radar_=radar;
	bool parsingSuccessful = reader.parse(data, root);
    //LoggerWrapper::GetInstance()->Write(LV_ACTION,"Weather: Inside Get_WWO: %s",root.toStyledString().c_str());
    if (root["meta"]["status_code"].asString() == "504"){
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Get_WWO: Failed timeout");
        return false;

    }

    if(!parsingSuccessful)
    {
        //cout << "Failed to parse configuration. " << reader.getFormatedErrorMessages();
        LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Get_WWO: Failed to parse configuration");
        return false;
    }


//    cout << root;

    parse_data(root);

    return true;
}







void WWO::parse_data(Json::Value const &root){

    const Json::Value c_data = root["data"]["current_condition"][0];
    set_date(c_data.get("localObsDateTime","No Data").asString());
    set_temp_F(c_data.get("temp_"+d_units_,"No Data").asString());
    set_winds(c_data.get("windspeedKmph","No Data").asString());
    set_windd(c_data.get("winddirDegree","No Data").asString());
    set_vis(c_data.get("visibility","No Data").asString());
    set_slp(c_data.get("pressure","No Data").asString());
    set_wc(c_data.get("FeelsLike"+d_units_,"No Data").asString());
    set_relh(c_data.get("humidity","No Data").asString());
    set_weatherimage(c_data.get("weatherCode","No Data").asString());
    set_weatherc(c_data["lang_"+d_lang_][0].get("value","No Data").asString());
    set_areadisc(root["data"]["request"][0].get("query","No Data").asString());

    //Parse forecast
      const Json::Value w_data = root["data"]["weather"];
     struct tm tmlol,tm;
     char buffer[80];
     time_t rawtime;
     struct tm * timeinfo;
     time (&rawtime);
     timeinfo = localtime (&rawtime);
     night_="";
     for (int x = 0; x <=w_data.size()-1; x=x+1)
	{
        data_ =w_data[x].get("date","No Data").asString();
        strptime(data_.c_str(),"%Y-%m-%d", &tmlol);
	strftime(buffer,80,"%A",&tmlol);
        f_days_.push_back(buffer);    
        memset(&tm, 0, sizeof(struct tm));
        strftime(buffer,80,"%h%M",timeinfo);
        strftime (buffer,80,"H",timeinfo);
        icon_temp_=w_data[x]["hourly"][time_].get("weatherCode","").asString();
        // cout << "debug icon" << icon_temp_ << endl;
        if (((icon_temp_ =="113") or (icon_temp_ =="116") or (icon_temp_=="112")) and x==0){
                time_=w_data[x]["hourly"].size();
              	f_icon_.push_back(get_icon(w_data[x]["hourly"][time_].get("weatherCode","").asString()+"N"));
		f_weather_.push_back(w_data[x]["hourly"][time_]["lang_"+d_lang_][0].get("value","No Data").asString());
		f_temp_F_.push_back(w_data[time_].get("maxtemp"+d_units_,"No Data").asString());
		f_temp_F_.push_back(w_data[time_].get("mintemp"+d_units_,"No Data").asString());
        } else if (x==0){
                time_=w_data[x]["hourly"].size()-1;
	      	f_icon_.push_back(get_icon(w_data[x]["hourly"][time_].get("weatherCode","").asString()));
		f_weather_.push_back(w_data[x]["hourly"][time_]["lang_"+d_lang_][0].get("value","No Data").asString());
		f_temp_F_.push_back(w_data[time_].get("maxtemp"+d_units_,"No Data").asString());
		f_temp_F_.push_back(w_data[time_].get("mintemp"+d_units_,"No Data").asString());
        }
        if (atoi(buffer) < 12){
           time_=0;
           night_="";
        }else if (atoi(buffer) <18){
            time_ =w_data[x]["hourly"].size() -1;
            night_="";
        }else{
	    time_=w_data[x]["hourly"].size();
            night_ = "N";
        }
        if ((icon_temp_ =="113") or (icon_temp_ =="116") or (icon_temp_=="112")){
	    f_icon_.push_back(get_icon(w_data[x]["hourly"][time_].get("weatherCode","").asString()+night_));
        }else{
	    f_icon_.push_back(get_icon(w_data[x]["hourly"][time_].get("weatherCode","").asString()));
        }
       f_temp_F_.push_back(w_data[time_].get("maxtemp"+d_units_,"No Data").asString());
       f_temp_F_.push_back(w_data[time_].get("mintemp"+d_units_,"No Data").asString());
       f_weather_.push_back(w_data[x]["hourly"][time_]["lang_"+d_lang_][0].get("value","No Data").asString());

     }
     strftime (buffer,80,"%Y%m%d%H%M",timeinfo);
     radar_ = string("http://cdn.worldweatheronline.net/charts/mrn_")+string(d_radar_)+string("_3.gif?v=") + string(buffer);
     radar_ += string("|http://cdn.worldweatheronline.net/charts/mrn_")+string(d_radar_)+string("_6.gif?v=") + string(buffer);
     radar_ += string("|http://cdn.worldweatheronline.net/charts/mrn_")+string(d_radar_)+string("_9.gif?v=") + string(buffer);
     radar_ += string("|http://cdn.worldweatheronline.net/charts/mrn_")+string(d_radar_)+string("_12.gif?v=") +string(buffer);
     radar_ += string("|http://cdn.worldweatheronline.net/charts/mrn_")+string(d_radar_)+string("_15.gif?v=") +string(buffer);
     radar_ += string("|http://cdn.worldweatheronline.net/charts/mrn_")+string(d_radar_)+string("_18.gif?v=") +string(buffer);
     radar_ += string("|http://cdn.worldweatheronline.net/charts/mrn_")+string(d_radar_)+string("_21.gif?v=") +string(buffer);// <<"|http://cdn.worldweatheronline.net/charts/mrn_8_24.gif?v=" <<buffer <<"|http://cdn.worldweatheronline.net/charts/mrn_8_27.gif?v=" << buffer << "|http://cdn.worldweatheronline.net/charts/mrn_8_30.gif?v=" << buffer <<"|http://cdn.worldweatheronline.net/charts/mrn_8_33.gif?v=" << buffer << "|http://cdn.worldweatheronline.net/charts/mrn_8_36.gif?v=" << buffer <<"|http://cdn.worldweatheronline.net/charts/mrn_8_39.gif?v="<< buffer ;
     timeinfo->tm_mday +=5;
     time_t finaldate=mktime(timeinfo);
     strftime(buffer,80,"%A",localtime(&finaldate));
     f_days_.push_back(buffer);    
}


vector<vector<string> > WWO::get_msg(){

	set_wrap("temp_current",temp_F_+"°"+d_units_,temp_F_);
	set_wrap("temp_tonight",f_temp_F_[0]+"°"+d_units_,f_temp_F_[0]);
       	set_wrap("cond_tonight",f_weather_[0],f_icon_[0]);
	set_wrap("forecast_tonight","No Data");
	set_wrap("cond_current",weatherc_,weatherimage_);
	set_wrap("humidity_current",relh_+"%",relh_);
	set_wrap("pressure_current",slp_+" Hg",slp_);
	set_wrap("wind_current",winds_+" kmph",winds_);
	set_wrap("direction_current",windd_+" deg",windd_);
	set_wrap("visibility_current",vis_+" mi",vis_);
	set_wrap("feelslike_current",wc_+"°"+d_units_,wc_);
	set_wrap("location",areadisc_);
	set_wrap("data_age",date_);
	set_wrap("cond_day1",f_weather_[index_+1],f_icon_[index_+1]);
	set_wrap("cond_day2",f_weather_[index_+2],f_icon_[index_+2]);
	set_wrap("cond_day3",f_weather_[index_+3],f_icon_[index_+3]);
	set_wrap("cond_day4",f_weather_[index_+4],f_icon_[index_+4]);
	set_wrap("cond_day5",f_weather_[index_+5],f_icon_[index_+5]);
	set_wrap("cond_day6",f_weather_[index_+5],f_icon_[index_+5]);
	set_wrap("temp_hi_day1",f_temp_F_[1]+"°"+d_units_,f_temp_F_[1]);
	set_wrap("temp_lo_day1",f_temp_F_[2]+"°"+d_units_,f_temp_F_[2]);
	set_wrap("temp_hi_day2",f_temp_F_[3]+"°"+d_units_,f_temp_F_[3]);
	set_wrap("temp_lo_day2",f_temp_F_[4]+"°"+d_units_,f_temp_F_[4]);
	set_wrap("temp_hi_day3",f_temp_F_[5]+"°"+d_units_,f_temp_F_[5]);
	set_wrap("temp_lo_day3",f_temp_F_[6]+"°"+d_units_,f_temp_F_[6]);
	set_wrap("temp_hi_day4",f_temp_F_[7]+"°"+d_units_,f_temp_F_[7]);
	set_wrap("temp_lo_day4",f_temp_F_[8]+"°"+d_units_,f_temp_F_[8]);
	set_wrap("temp_hi_day5",f_temp_F_[9]+"°"+d_units_,f_temp_F_[9]);
	set_wrap("temp_lo_day5",f_temp_F_[10]+"°"+d_units_,f_temp_F_[10]);
	set_wrap("temp_hi_day6",f_temp_F_[9]+"°"+d_units_,f_temp_F_[9]);
	set_wrap("temp_lo_day6",f_temp_F_[10]+"°"+d_units_,f_temp_F_[10]);
	set_wrap("dow_day1",f_days_[0]);
	set_wrap("dow_day2",f_days_[1]);
	set_wrap("dow_day3",f_days_[2]);
	set_wrap("dow_day4",f_days_[3]);
	set_wrap("dow_day5",f_days_[4]);
	set_wrap("dow_day6",f_days_[5]);
	set_wrap("1",radar_,"Radar");
	return wrap_;
}

void WWO::set_temp_F(const string &temp_f){
	temp_F_=temp_f;
}

void WWO::set_vis(const string &vis){
	vis_=vis;
}

void WWO::set_slp(const string &slp){
	slp_=slp;
}

void WWO::set_wc(const string &wc){
	wc_=wc;
}

void WWO::set_date(const string &date){
	date_=date;
}

void WWO::set_winds(const string &winds){
	winds_=winds;
}

void WWO::set_windd(const string &windd){
	windd_=windd;
}

void WWO::set_relh(const string &relh){
	relh_=relh;
}

void WWO::set_weatherimage(const string &weatherimage){
//LoggerWrapper::GetInstance()->Write(LV_CRITICAL,"Weather: Inside image: %s",weatherimage.c_str());

	weatherimage_=get_icon(weatherimage);
}

void WWO::set_weatherc(const string &weatherc){
	weatherc_=weatherc;
}

void WWO::set_areadisc(const string &areadisc){
	areadisc_=areadisc;
}

//void WWO::set_f_days(Json::Value const & days){
//	for ( unsigned int index = 0; index < days.size(); ++index )
//		f_days_.push_back(days[index].asString());
//}

string WWO::get_icon(const string &code){
	string icon;
	if (icons_.find(code) != icons_.end())
		icon = icons_.find(code)->second;
	return icon;
}

void WWO::set_index(Json::Value const &data){
	for ( unsigned int index = 1; index < data.size(); ++index ){
		string str = data[index].asString();

		if (WWO::dayl_.find(str) != WWO::dayl_.end()){
			index_=index;
			return;
		}
	}
}

void WWO::set_wrap(const std::string& str1,const std::string& str2,const std::string& str3,const std::string& str4){
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

/**void WWO::set_dayl(const string &str2){
	dayl_.insert(str1);
}**/
