#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/dllist.h"
#include "/home/bryan/Desktop/PersonalCodeRepo/CS-360/Libfdr/include/jrb.h"

typedef struct {
  Dllist text;
  JRB members;
  pthread_mutex_t *lock;
  pthread_cond_t *send;
  int n;
} Room;

typedef struct {
    int fd;
    JRB rooms
} Client;


typedef struct {
    char name[300];
    FILE *fout;
} User;
