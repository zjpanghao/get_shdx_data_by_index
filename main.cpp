        /*
*    author: chxun
*/
#include <sys/types.h>
#include <sys/syscall.h>
#include <iostream>
#include <set>
#include <string>
#include <map>
#include <iterator>
#include <glog/logging.h>
#include "common.h"
#include "json_data.h"
#include "KafkaWrapper.h"
#include "CurlWrapper.h"
#include "tele_compress.h"
using namespace std;

CurlWrapper curl_wrapper;
std::string url2_toKen;
int task_start_index = 0;
int max_thread_num = 80;
void start_work_threads(work_info* tasks_info);
enum TeleCommand{
	INVALID_COMMAND = 0,
	KAFKA_COMMAND = 1,
	INDEX_COMMAND = 2,
	THREAD_NUM_COMMAND = 3,
};

static void glog_init(std::string name, int task_start_index) {
	google::InitGoogleLogging(name.c_str());
	char buf[64];
	char dir[32];
	sprintf(dir, "log%d", task_start_index);
	char cmd[64];
	sprintf(cmd, "mkdir %s", dir);
	system(cmd);
	sprintf(buf, "%s/teleinfo", dir);
	google::SetLogDestination(google::INFO, buf);	
	sprintf(buf, "%s/telewarn", dir);
	google::SetLogDestination(google::WARNING, buf);   
	sprintf(buf, "%s/teleerr", dir);
	google::SetLogDestination(google::ERROR, buf);   
	FLAGS_logbufsecs = 10;
}
int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		printf("test_run [-k] [kafka_server] -[i] [index] -p [thead_num]\n");
		return -1;
	}
	TeleCommand cmd = INVALID_COMMAND;
	//default config
	std::string kafka_server = "localhost:9092";
	task_start_index = 0;
	
	for (int i = 0; i < argc; i++) {
		switch (cmd) {
		  case KAFKA_COMMAND:
		  	kafka_server = argv[i];
			  break;
		  case INDEX_COMMAND:
		  	task_start_index = atoi(argv[i]);
			  break;
			case THREAD_NUM_COMMAND:
				max_thread_num = atoi(argv[i]);
				break;
		}
		cmd = INVALID_COMMAND;
		if (strcmp(argv[i], "-k") == 0) {
			cmd = KAFKA_COMMAND;
		} else if (strcmp(argv[i], "-i") == 0) {
			cmd = INDEX_COMMAND;
		} else if (strcmp(argv[i], "-p") == 0) {
			cmd = THREAD_NUM_COMMAND;
		}
	}
	glog_init(argv[0], task_start_index);
	//task_start_index = atoi(argv[1]);
	LOG(INFO) << "task_start_index:" << task_start_index <<  "Kafka server:" << kafka_server\
			<< "Thread num:" << max_thread_num;
	
	char push_data[100] = {0};
	int partition = 0;
	const char* topic = "kun_origin";
	/*init kafka*/
	wrapper_Info test_info;
	if (PRODUCER_INIT_SUCCESS == producer_init(partition, topic, kafka_server.c_str(), &test_msg_delivered, &test_info))
		LOG(INFO) << ("producer init success!\n");
	else {
		LOG(ERROR) << ("producer init failed\n");
		return -1;
	}

  CURL *curl = CurlWrapper::get_instance()->CreateCurl();
	const char * token_url = "http://61.129.39.71/telecom-dmp/getToken?apiKey=98f5103019170612fd3a486e3d872c48&sign=6a653929c81a24ba14e41e25b6047e5dec55e76e";
	url2_toKen = get_toKen_by_url(token_url, curl);
  curl_easy_cleanup(curl);
	

	work_info tasks_info(&test_info);
	start_work_threads(&tasks_info);

	printf("------------------------------------------------------------------------------\n");
	while (true)
	{
		sleep(60*360*24*60*60);
	}
	producer_close(&test_info);

	return 0;
}

void get_tasks(work_info* info, std::string& url_time_key, std::map<int, TaskState>& tasks_to_run, int task_num) {
	//GetLock(&info->mutex_);
	int re = pthread_mutex_lock(&info->mutex_);
	int start_index = 0;
	if (0 != re) {
		printf("re = %d errno=%d\n", re, errno);
	}
	 start_index = info->tasks_per_minute_[url_time_key];
   if (start_index > MAX_INDEX_NUM) {
       re = pthread_mutex_unlock(&info->mutex_);
      	if (0 != re) {
      		printf("re = %d errno=%d\n", re, errno);
      	}
      	return;
   }
	 info->tasks_per_minute_[url_time_key] = start_index + TASKS_PER_NUM;
	re = pthread_mutex_unlock(&info->mutex_);
	if (0 != re) {
		printf("re = %d errno=%d\n", re, errno);
	}

	if (start_index >= MAX_INDEX_NUM)
		return;
	for (int i = task_start_index; i < task_start_index + TASKS_PER_NUM; i++) {
		if (i >= task_start_index + MAX_INDEX_NUM)
			break;
		int curr_index = start_index + i;
		
		tasks_to_run.insert(std::make_pair(curr_index, TASK_RUNNING));
	}

}

time_t get_fetch_timestamp() {
  return time(NULL) - TIME_SECONDS_DELAYED;
}

static void *clear_task_thread(void* arg) {
  work_info* info = (work_info*)arg;
  sleep(3600);
  while(true) {
    info->check_and_clear_task_map();
    sleep(5*60);
  }
  return NULL;
}

static std::string get_strtime(time_t timestamp)
{
  struct tm current_tm;
  struct tm* current_time = &current_tm;
  localtime_r(&timestamp, current_time);
  char buffer[13] = { 0 };
  sprintf(buffer, "%04d%02d%02d%02d%02d",
		current_time->tm_year + 1900,
		current_time->tm_mon + 1,
		current_time->tm_mday,
		current_time->tm_hour,
		current_time->tm_min);

  string str = buffer;
  return str;
}

std::string get_fetch_url_time_key(std::string url_time_key) {
  char buffer[15] = {0};
  strncpy(buffer, url_time_key.c_str(), 12);
  char hour[3];
  memset(hour, 0, sizeof(hour));
  strncpy(hour, url_time_key.c_str() + 8, 2);
  int new_hour = atoi(hour) % 3;
  snprintf(hour, sizeof(hour), "%02d", new_hour);
  strncpy(buffer + 8, hour, 2);
  std::string s = buffer;
  return s;
}

static bool TokenInvalid(std::string value) {
  const std::string valid_str = ":400402";
  return value.find(valid_str) != std::string::npos;
}

void* run(void* arg) {
	std::string token = url2_toKen;
        CURL *curl = CurlWrapper::get_instance()->CreateCurl();
	while (true) {
		work_info* info = (work_info*)arg;
		std::vector<std::string> url_vec;
		std::set<std::string> value_set;
		std::map<int, TaskState> task_indexs;
                time_t fetch_time = get_fetch_timestamp();
		std::string url_time_key = get_strtime(fetch_time);
		get_tasks(info, url_time_key, task_indexs, TASKS_PER_NUM);
		if (task_indexs.size() == 0) {
			sleep(1);
			continue;
		}

		for (std::map<int, TaskState>::iterator iter = task_indexs.begin(); iter != task_indexs.end(); iter++) {
			std::string url;
     			std::string fetch_url_time_key = get_fetch_url_time_key(url_time_key);
			get_url_by_toKen(token, fetch_url_time_key, url, iter->first);
			std::string result;
			if (!get_data_from_shanghai_1(url, result, token, curl)) {
         if (iter->first % 10000 == 0)
				   LOG(ERROR) << url << "feth url failed" << url_time_key;
				iter->second = TASK_RESULT_NULL;
				continue;
			}
			else {
	      if(iter->first % 10000 == 0)		
          LOG(INFO) << url << " success " << url_time_key;
				iter->second = TASK_FINISHED;
        int rc;
				TeleCompress tele;

				result =  tele.GetProcessResult(result.c_str());
				if (PUSH_DATA_SUCCESS != (rc = producer_push_data(result.c_str(), result.length(), info->producer_))) {
          if (iter->first % 10000 == 0)
					  LOG(ERROR) << "push data failed" << result.length() << "rc:" << rc;
          sleep(1);
        }
			}
		}
	}
	return NULL;
}

void start_work_thread(work_info* tasks_info) {
	thread_info* t_info = new thread_info;

	t_info->td = new pthread_t;
	int err = pthread_create(t_info->td, NULL, run, tasks_info);
	if (err != 0)
		printf("can not create thread errno=%d\n", errno);

}

void start_work_threads(work_info* tasks_info) {
  pthread_t clear_id;
  pthread_t retry_id;

	for (int i = 0; i < max_thread_num; i++)
	{
		start_work_thread(tasks_info);
		printf("start thread %d.\n", i);
	}

  pthread_create(&clear_id, NULL, clear_task_thread, tasks_info);


}


