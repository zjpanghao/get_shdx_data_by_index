#ifndef _COMMON_H_
#define _COMMON_H_

#include "KafkaWrapper.h"
#include <pthread.h>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <iterator>
#include <iostream>
using namespace std;

#define MAX_THREAD_NUM  80
#define MAX_INDEX_NUM 160000
#define MAX_TASKS_TOTAL_SIZE 60
#define TASKS_PER_NUM 10
#define TIME_SECONDS_DELAYED 5 * 60
#define MESSAGE_WRONG_TOKEN 400402
#define GET_TOKEN_URL "http://61.129.39.71/telecom-dmp/getToken?apiKey=98f5103019170612fd3a486e3d872c48&sign=6a653929c81a24ba14e41e25b6047e5dec55e76e"

enum TaskState {
	TASK_RESULT_NULL,
	TASK_RUNNING,
	TASK_FINISHED,
};

typedef struct thread_info {
	pthread_t *td;
	void *args;
}thread_info;

class work_info {

public:
	work_info(wrapper_Info* producer) {
		pthread_mutex_init(&mutex_, NULL);
		producer_ = producer;
	}
	~work_info() {

	}

  time_t get_timestamp_from_str(string time_str) {
      struct tm save_tm;
      memset(&save_tm, 0, sizeof(save_tm));
      char buffer[5] = {0};
      strncpy(buffer, time_str.c_str(), 4);
      save_tm.tm_year = atoi(buffer) - 1900;
      printf("%d\n", save_tm.tm_year);
      memset(buffer, 0, sizeof(buffer));
      strncpy(buffer, time_str.c_str()+4, 2);
      save_tm.tm_mon = atoi(buffer) - 1;
      memset(buffer, 0, sizeof(buffer));
      strncpy(buffer, time_str.c_str()+6, 2);
      save_tm.tm_mday = atoi(buffer);
      memset(buffer, 0, sizeof(buffer));
      strncpy(buffer, time_str.c_str()+8, 2);
      save_tm.tm_hour = atoi(buffer);
      memset(buffer, 0, sizeof(buffer));
      strncpy(buffer, time_str.c_str()+10, 2);
      save_tm.tm_min = atoi(buffer);

      return mktime(&save_tm);
  }

  void check_and_clear_task_map() {
    pthread_mutex_lock(&mutex_);
    // keep 60 
    while (tasks_per_minute_.size() > 60) {
      tasks_per_minute_.erase(tasks_per_minute_.begin());
    }
    pthread_mutex_unlock(&mutex_);
  }

	std::string current_time_str_;
	std::string last_time_str_;
	std::map<std::string, int> tasks_per_minute_;
	wrapper_Info* producer_;
	pthread_mutex_t mutex_;
};

class GetLock {

public:
	GetLock(pthread_mutex_t* mutex): mutex_(mutex) {
		printf("lock=%p\n", mutex_);
		pthread_mutex_lock(mutex_);
	}
	~GetLock() {
		pthread_mutex_unlock(mutex_);
	}
	pthread_mutex_t* mutex_;
};

#endif
