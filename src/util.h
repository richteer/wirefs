#ifndef U_UTIL
#define U_UTIL

int u_format(int diskSizeBytes, char* file_name);
int recover_file_system(char *file_name);
int u_clean_shutdown();

#endif
