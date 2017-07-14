#ifndef TELE_CONSUMER_H
#define TELE_CONSUMER_H
#define MAX_TELE_QUEUE 300000
#include <queue>
#include <string>
struct  TeleData {
 public:
  std::string data;
  int task_index;
};

class TeleQueue {
 public:
  TeleQueue() {
    pthread_mutex_init(&lock_, NULL);
  }

  bool Add(TeleData  data) {
    pthread_mutex_lock(&lock_);
    if (data_queue_.size() < MAX_TELE_QUEUE) {
      data_queue_.push(data);
      pthread_mutex_unlock(&lock_);
      return true;
    }
    ClearHalf();
    pthread_mutex_unlock(&lock_);
    return false;
  }
  
  bool Pop(TeleData *data) {
    pthread_mutex_lock(&lock_);
    if (data_queue_.empty()) {
      pthread_mutex_unlock(&lock_);
      return false;
    }
    *data = data_queue_.front();
    data_queue_.pop();
    pthread_mutex_unlock(&lock_);
    return true;
  }
  
  void ClearHalf() {
    for (int i = 0; i < MAX_TELE_QUEUE / 2;i++) {
      if (!data_queue_.empty())
        data_queue_.pop();
    }
  }

 private:
   std::queue<TeleData> data_queue_;
   pthread_mutex_t lock_;
};

TeleQueue& GetTeleQueue() {
  static TeleQueue queue;
  return queue;
}

#endif
