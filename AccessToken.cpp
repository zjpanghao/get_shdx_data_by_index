#include "AccessToken.h"


AccessToken::AccessToken()
{
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	pthread_mutexattr_settype(&mattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&token_mutex, &mattr);
	pthread_mutexattr_destroy(&mattr);
}


AccessToken::~AccessToken()
{
}

std::string AccessToken::UpdateToken() {
	pthread_mutex_lock(token_mutex);
	std::string url = "http://61.129.39.71/telecom-dmp/getToken?apiKey=98f5103019170612fd3a486e3d872c48&sign=6a653929c81a24ba14e41e25b6047e5dec55e76e";
	std::string toKen = "";
	Json::Value root;

	string str = "";
	str = CurlWrapper::get_instance()->access_http(url.c_str());
	if ("" == str)
	{
		return str;
	}

	root = get_json_array(str);
	toKen = root["result"].asString();
	pthread_mutex_unlock(token_mutex);
	//std::cout << toKen << std::endl;
	return toKen;
}