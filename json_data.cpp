/*
*  Created on: 2016.2.1
*      Author: chenxun
*/
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "json_data.h"
#include "CurlWrapper.h"
#include "base64.h"
#include "common.h"
#include "glog/logging.h"
using namespace std;

std::string LastSysTime_min = "";
Json::Value get_json_array(const std::string &str)
{
	Json::Value root;
	Json::Reader reader;

	if (!reader.parse(str, root, false))
	{
		std::cout << "--Plz check your url make sure your url is available--" << std::endl;
		return 0;
	}

	return root;
}

std::string get_toKen_by_url(const char* url, CURL *curl)
{
	std::string toKen = "";
	Json::Value root;

	string str="";
	str = CurlWrapper::get_instance()->access_http(url, curl);
	//cout << "get_toKen_by_url str=" << str << endl;
	if ("" != str)
	{
		root = get_json_array(str);
	}
	else
		return toKen;
	toKen = root["result"].asString();
	//std::cout << toKen << std::endl;
	return toKen;
}

std::string get_str_of_time()
{
	string str = "";
	time_t t = time(NULL);
	t -= TIME_SECONDS_DELAYED;
	struct tm* current_time = localtime(&t);
	char buffer[13] = { 0 };
	sprintf(buffer, "%d%02d%02d%02d%02d",
		current_time->tm_year + 1900,
		current_time->tm_mon + 1,
		current_time->tm_mday,
		current_time->tm_hour,
		current_time->tm_min);

	str = buffer;
	return str;
}

void get_url_by_toKen(const std::string &url2_toKen, const std::string& url_time_key, std::string& url, int url_index)
{
	std::string url1_ip = "http://61.129.39.71/telecom-dmp/kv/getValueByKey?token=";
	std::string url3_table = "table=kunyan_to_upload_inter_tab_up&key=";
	std::string  url_prefix = url1_ip + url2_toKen + "&" + url3_table + url_time_key + "_ky_";
	char str_i[10];
	sprintf(str_i, "%d", url_index);
	url = url_prefix + str_i;
}

std::string get_value_by_url(const std::string &url, std::string& token, CURL *curl)
{
	std::string value = "";
	Json::Value root;

	string str = "";
	str = CurlWrapper::get_instance()->access_http(url.c_str(), curl);
	if (str != "")
	{
		//cout << "str=" << str << endl;
		root = get_json_array(str);
	}
	else
		return value;
    //printf("%ld\n", root["code"].asInt64());
	long long code = root["code"].asInt64();
	if (code == MESSAGE_WRONG_TOKEN)
	{
		const char* token_url = "http://61.129.39.71/telecom-dmp/getToken?apiKey=98f5103019170612fd3a486e3d872c48&sign=6a653929c81a24ba14e41e25b6047e5dec55e76e";
		token = get_toKen_by_url(token_url, curl);
                return "";
	} else {
          // LOG(INFO) << (code == 200200);
        }
        if (!root["result"].isNull() && !root["result"]["value"].isNull())
	  value = root["result"]["value"].asString();
	return value;
}

bool get_data_from_shanghai_1(std::string& url, std::string& value_base64_decode, std::string& token, CURL *curl)
{
	value_base64_decode = get_value_by_url(url, token, curl);
	if ("" == value_base64_decode)
		return false;
	return true;
}
