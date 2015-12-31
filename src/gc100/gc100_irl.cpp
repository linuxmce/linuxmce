#include "gc100_irl.h"
#include "PlutoUtils/StringUtils.h"

string gc100_irl::IRL_uncompress(const string &IRL_string)
{
	bool done = false;
	string sPair[4];
	unsigned int iPairs = 0, i, pair_num = 0;
	string::size_type pos = 0;
	string sResult = "";

	//LoggerWrapper::GetInstance()->Write(LV_WARNING, "IRL_uncompress: compressed string: %s", IRL_string.c_str());
	
	string token1, token2, digits2, comp2;

	// take prefix and frequency verbatim
	string IRL_string_copy = IRL_string;
	sResult += StringUtils::Tokenize(IRL_string_copy, ",", pos) + ",";
	sResult += StringUtils::Tokenize(IRL_string_copy, ",", pos);

	while (! done)
	{
		if ((pos >= IRL_string.length()) || (++pair_num > 5000))
		{
			//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_uncompress: no more pairs; we're done");
			done = true;
		}
		else
		{
			token1 = StringUtils::Tokenize(IRL_string_copy, ",", pos); // ON value
			token2 = StringUtils::Tokenize(IRL_string_copy, ",", pos); // OFF value

			if (token1 == "X" || token2 == "X" || token1 == "Z" || token2 == "Z")
			{
				//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_uncompress: end-of sequence X or Z character detected");
				done = true;
			}
			else
			{
				digits2 = token2;
				comp2 = "";
				
				for (i = 0; i < token2.length(); i++)
				{
					if (token2[i] >= 'A' && token2[i] <= 'D')
					{
						digits2 = token2.substr(0, i);
						comp2 = token2.substr(i);
						break;
					}
				}

				if (iPairs < 4)
				{
					sPair[iPairs] = string("") + "," + token1 + "," + digits2;
					iPairs++;
				}

				sResult += string("") + "," + token1 + "," + digits2;
				for (i = 0; i < comp2.length(); i++)
				{
					sResult += sPair[comp2[i] - 'A'];
				}
			}
		}
	}

	//LoggerWrapper::GetInstance()->Write(LV_WARNING, "IRL_uncompress: uncompressed code: %s", sResult.c_str());
	return sResult;
}

string gc100_irl::IRL_to_pronto(const string &IRL_string)
{
	std::string result,token,token2,pronto_pairs;
	string::size_type pos;
	std::string learned_string;
	int freq,freq_token;
	int pair_num;
	int v1,v2;
	bool done;
	char conv_buf[256];
	int trunc_prefix;

	string raw_learned_string = IRL_uncompress(IRL_string);
	trunc_prefix = raw_learned_string.rfind(LEARN_PREFIX_IRL);
	if (trunc_prefix < 0)
	{
		trunc_prefix = raw_learned_string.rfind(LEARN_PREFIX_IRE);
		if (trunc_prefix < 0)
		{
			//LoggerWrapper::GetInstance()->Write(LV_WARNING, "IRL_to_pronto: trunc_prefix was %d", trunc_prefix);
			trunc_prefix = 0;
		}
	}

	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_to_pronto: trunc_prefix is %d",trunc_prefix);

	learned_string = IRL_uncompress(raw_learned_string.substr(trunc_prefix));
	pos=0;

	// Output: 0000 format
	result="0000 "; // Pronto raw pwm prefix

	token=StringUtils::Tokenize(learned_string,",",pos); // First token is "GC-IRL" or "receiveir" header
	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_to_pronto: prefix is %s",token.c_str());

	token=StringUtils::Tokenize(learned_string,",",pos); // Next token is frequency
	sscanf(token.c_str(),"%d",&freq);
	//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_to_pronto: freq is %d",freq);
	freq_token = (int) (4145146 / freq);
	sprintf(conv_buf,"%04x ",freq_token);
	// Output: frequency word
	result+=std::string(conv_buf);

	done=false;
	pair_num=1;

	while (!done)
	{
		if ((pos>=learned_string.length())||(pair_num>5000))
		{
			//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_to_pronto: no more pairs; we're done");
			done=true;
		}
		else
		{
			token=StringUtils::Tokenize(learned_string,",",pos); // ON value
			token2=StringUtils::Tokenize(learned_string,",",pos); // OFF value
			
			sscanf(token.c_str(),"%d",&v1);
			sscanf(token2.c_str(),"%d",&v2);

			//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_to_pronto: data pair #%d: %d/%d",pair_num,v1,v2);
			// Do any necessary correcting of values here

			if (token == "X" || token2 == "X" || token == "Z" || token2 == "Z")
			{
				//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_to_pronto: end-of sequence X or Z character detected");
				done=true;
			}
			else
			{
				if (v2==0)
				{
					v2=v1;
					//LoggerWrapper::GetInstance()->Write(LV_STATUS, "IRL_to_pronto: corrected 0 to %d",v2);
				}

				sprintf(conv_buf," %04x %04x",v1,v2);
				pronto_pairs+=std::string(conv_buf);

				pair_num++;
			}
		}
	}

	pair_num--;
	// Output: # of pairs for single // DEBUG
	result+="0000 ";

	sprintf(conv_buf,"%04x",pair_num); // DEBUG
	//sprintf(conv_buf,"%04x ",pair_num);
	// Output: # of pairs for single burst
	result+=std::string(conv_buf);

	// Output: # of pairs for repeat sequence
	//result+="0000";

	// Output: all of the on/off pairs
	result+=pronto_pairs;

	return result;
}
