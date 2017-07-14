#ifndef ACCESS_TOKEN_H_
#define ACCESS_TOKEN_H_

#include "common.h"
#include "json.h"
#include "CurlWrapper.h"
#include "json_data.h"

class AccessToken {
 public:
  AccessToken();
  ~AccessToken();

  std::string AccessToken::UpdateToken();

  std::string url2_toKen;
  pthread_mutex_t* token_mutex;
};

#endif
