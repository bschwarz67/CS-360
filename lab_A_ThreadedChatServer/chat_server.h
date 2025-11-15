#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"

typedef struct {
  Dllist text;
  Dllist members;
  pthread_mutex_t *lock;
  pthread_cond_t *send;
} Room;