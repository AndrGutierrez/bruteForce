
typedef struct {
  char username[256];
  char hash[33];
} UserHash;

int read_user_hashes(const char *filename, UserHash **users, int *count);
void process_user_hashes(UserHash *users, int count);
void print_processing_time(const char *username, double time_taken);
