/*
*
*  Created on: 2016.1.30
*      Author: chen
*/

#ifndef JSON_DATA_H_INCLUDED
#define JSON_DATA_H_INCLUDED

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <sstream>

#include "curl/curl.h"
#include "json.h"

Json::Value get_json_array(const std::string &str);

std::string get_toKen_by_url(const char* url);

std::string get_str_of_time();

void get_url_by_toKen(const std::string &url2_toKen, const std::string& url_time_key, std::string& url, int url_index);

std::string get_value_by_url(const std::string &url, std::string& token);

bool get_data_from_shanghai_1(std::string& url, std::string& value_base64_decode, std::string& token);

#endif // JSON_DATA_H