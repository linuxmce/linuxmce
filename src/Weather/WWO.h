/*
 * WWO.h
 *
 *  Created on: Feb 12, 2015
 *      Author: root
 */

#ifndef WWO_H_
#define WWO_H_

#include "jsoncpp/json/json.h"
#include <map>
#include <set>

class WWO {
private:
	int index_;
	std::string temp_F_;
	std::string vis_;
	std::string slp_;
	std::string wc_;
	std::string date_;
	std::string winds_;
	std::string windd_;
	std::string relh_;
	std::string d_units_;
	std::string d_lang_;
	std::string d_radar_;
	std::string weatherimage_;
	std::string weatherc_;
	std::string areadisc_;
	std::string radar_;
	std::string data_;
	std::string night_;
	std::string icon_temp_;

	int time_;
	std::vector<std::string> f_temp_F_;
	std::vector<std::string> f_text_;
	std::vector<std::string> f_weather_;
	std::vector<std::string> f_icon_;
	std::vector<std::string> f_days_;
	std::string get_icon(const std::string &png);
	std::map<std::string,std::string> icons_ ;
	std::set<std::string> dayl_;
	void parse_data(Json::Value const &root);
	void set_index(Json::Value const &data);
	void set_icons_(std::map<std::string,std::string>);

	template< typename T, size_t N >
	std::vector<T> makeVector( const T (&data)[N] )
	{
	    return std::vector<T>(data, data+N);
	}

	void set_wrap(const std::string& str1,const std::string& str2,const std::string& str3="",const std::string& str4="");
	std::vector<std::vector<std::string> > wrap_;

	void set_dayl(const std::string&);


public:
	WWO();
	virtual ~WWO();
	bool Get_WWO(const std::string& data,const std::string& units,const std::string& lang,const std::string& radar);

	void set_date(std::string const &date);
	void set_temp_F(std::string const & temp_F);
	void set_winds(std::string const &winds);
	void set_windd(std::string const &windd);
	void set_vis(std::string const &vis);
	void set_slp(std::string const &slp);
	void set_wc(std::string const &wc);
	void set_relh(std::string const &winds);
	void set_weatherimage(std::string const &weatherimage); //Pass url png
	void set_weatherc(std::string const &weatherc);
	void set_areadisc(std::string const &areadisc);
	std::vector<std::vector<std::string> > get_msg();
	const std::string& date() const;
	const std::string& temp_F(int index) const;
	const std::string& winds() const;
	const std::string& weatherimage() const;
	const std::string& weather(int index) const;
};

#endif /* WWO_H_ */
