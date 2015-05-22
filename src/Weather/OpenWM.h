/*
 * OpenWM.h
 *
 *  Created on: Feb 21, 2015
 *      Author: jim
 */

#ifndef OPENWM_H_
#define OPENWM_H_

#include "jsoncpp/json/json.h"
#include <map>
#include <set>

class OpenWM {
private:
	int index_;
	std::string s_temp_;
	std::string temp_;
	//std::string vis_;
	std::string s_slp_;
	std::string slp_;
	//std::string wc_;
	std::string date_;
	std::string s_winds_;
	std::string winds_;
	std::string s_windd_;
	std::string windd_;
	std::string s_relh_;
	std::string relh_;
	std::string weatherimage_;
	std::string weatherc_;
	std::string areadisc_;
	std::vector<std::string> s_f_temp_min_;
	std::vector<std::string> s_f_temp_max_;
	std::vector<std::string> f_temp_min_;
	std::vector<std::string> f_temp_max_;
//	std::vector<std::string> f_text_;
	std::vector<std::string> f_weather_;
	std::vector<std::string> f_icon_;
	std::vector<std::string> f_days_;
	std::string get_icon(const std::string &png);
	std::map<std::string,std::string> icons_ ;
	//std::set<std::string> dayl_;
	void parse_data(Json::Value const &root);
	//void set_index(Json::Value const &data);
	void set_icons_(std::map<std::string,std::string>);

	template< typename T, size_t N >
	std::vector<T> makeVector( const T (&data)[N] )
	{
	    return std::vector<T>(data, data+N);
	}

	void set_wrap(const std::string& str1,const std::string& str2,const std::string& str3="");
	std::vector<std::vector<std::string> > wrap_;
public:
	OpenWM();
	virtual ~OpenWM();
	std::string api_key;
	std::string c_id;
	std::string get_cid(const std::string &res);
	bool parse_cur(const std::string &res, const std::string &units);
	bool parse_forc(const std::string &res, const std::string &units);
	std::vector<std::vector<std::string> > get_msg();
	void set_date(time_t date);
	void set_temp_I(double temp_F);
	void set_temp_M(double temp_F);
	void set_winds_I(double winds);
	void set_winds_M(double winds);
	void set_windd(double windd);
	void set_slp(double slp);
	void set_relh(double relh);
	void set_areadisc(std::string const &country, std::string const &city);
	void set_weatherc(std::string const &weatherc);
	void set_weatherimage(std::string const &weatherimage); //Pass url png
	void set_f_temp(Json::Value const & f_temp, std::string const &str);
	void set_f_weather(Json::Value const &f_weather);
	void set_f_icon(Json::Value const & f_icon);
	void set_f_days(Json::Value const & days);

};

#endif /* OPENWM_H_ */
