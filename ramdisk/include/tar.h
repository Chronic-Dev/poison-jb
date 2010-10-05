#include <syscalls.h>

#define UNTAR_BUFFER_SIZE 512
#define UNTAR_DEBUG 0

static long long int untar_numBytes = 0;

typedef struct struct_FileHeader {
  char filename[100];
  char mode[8];
  char owner[8];
  char group[8];
  char filesize[12];
  char lastmodified[12];
  char checksum[8];
  char linkindicator;
  char linkedfile[100];
  char padding[255];
} FileHeader;

int isNull(FileHeader *header) {
  long long int *iheader = (long long int *)header;
  int i;
  for(i=0; i<sizeof(FileHeader) / sizeof(long long int); i++) {
    if(iheader[i] != 0) {
      return 0;
    }
  }
  return 1;
}

void untar_create_link_(char * fullpath, char * linkpath, int filesize, int fp){
  int result = symlink(linkpath, fullpath);
  if(result != 0) return;
}

void untar_create_directory_(char * fullpath, mode_t mode, int filesize, int fp){
  int result = mkdir(fullpath, mode);
  if(result != 0) {
    if(!errno == EEXIST) return;
  }
}

void untar_create_file_(char * fullpath, mode_t mode, int filesize, int fp) {
  int o = open(filepath, 2, 0777);
  char buffer[UNTAR_BUFFER_SIZE]; // read until we run out of filled blocks
  while(filesize / UNTAR_BUFFER_SIZE != 0) {
	read(fp, buffer, UNTAR_BUFFER_SIZE);
	untar_numBytes += UNTAR_BUFFER_SIZE;
	if(o) write(o, buffer, UNTAR_BUFFER_SIZE);
    filesize -= UNTAR_BUFFER_SIZE;
  }
	
  if(filesize > 0) { // read in and write the last block...
	read(fp, buffer, UNTAR_BUFFER_SIZE);
    untar_numBytes += UNTAR_BUFFER_SIZE;
	write(o, buffer, UNTAR_BUFFER_SIZE);  
  }
  
  if(o) close(o);
  chmod(fullpath, mode);
}

char* strcpy(char *to, const char *from) {
  char *save = to;
  for(;(*to=*from)!='\0';++from,++to);
  return(save);
}

char isspace(unsigned char c) {
  if(c == UC(' ') || c == UC('\f') || c == UC('\n') || c == UC('\r') || c == UC('\t') || c == UC('\v')) return 1;
  else return 0;
}

unsigned long strtoul(const char *s, char **ptr, int base) {
  unsigned long total = 0;
  unsigned digit;
  const char *start=s;
  int did_conversion=0;
  int overflow = 0;
  int negate = 0;
  unsigned long maxdiv, maxrem;
  
  if(s==NULL) {
    if(!ptr) *ptr = (char *)start;
    return 0L;
  }
  
  while(isspace(*s)) s++;
  if(*s == '+') s++;
  else if(*s == '-') s++, negate = 1;
  if(base==0 || base==16) {
    int tmp;
    if(*s != '0') tmp = 10;
    else if(s[1] == 'X' || s[1] == 'x') tmp = 16, s += 2;
    else tmp = 8;
    if(base==0) base = (int)tmp;
  }
	
  maxdiv = ULONG_MAX / base;
  maxrem = ULONG_MAX % base;
	
  while ((digit = *s) != '\0') {
    if(digit >= '0' && digit < ('0'+base)) digit -= '0';
    else if(base > 10) {
      if(digit >= 'a' && digit < ('a'+(base-10))) digit = digit - 'a' + 10;
      else if(digit >= 'A' && digit < ('A'+(base-10))) digit = digit - 'A' + 10;
      else break;
    }
    else break;
	did_conversion = 1;
	if(total > maxdiv || (total == maxdiv && digit > maxrem)) overflow = 1;
	total = (total * base) + digit;
	s++;
  }
  if(overflow) {
    errno = ERANGE;
    if(ptr != NULL) *ptr = (char *)s;
    return (ULONG_MAX);
  }
  if(ptr != NULL) *ptr = (char *)((did_conversion)?(char *)s:(char *)start);
  return negate?-total:total;
}

int strlen(const char *s) {
  const char *ss = s;
  while(*ss) ss++;
  return ss-s;
}

void untar(char *filename, char *basepath) {
  int fp = open(filename, 0, 0666);
  
  int consecutive_zero_fields = 0;
  untar_numBytes = 0;
	
  if(fp) {
    for(;;) {
      FileHeader header;
	  read(fp, &header, sizeof(FileHeader));
	  untar_numBytes += sizeof(FileHeader);
      if(header[0] == NULL) break; /* if at end of file, we are done */
      if(isNull(&header)) {
        consecutive_zero_fields++;
        continue;
      }

      char fullpath[strlen(basepath)+strlen(header.filename)+1];
      strcpy(fullpath, basepath);
      strcpy(fullpath+strlen(basepath), header.filename);
		
	  char linkpath[strlen(basepath)+strlen(header.linkedfile)+1];
	  strcpy(linkpath, basepath);
	  strcpy(linkpath+strlen(basepath), header.linkedfile);			
			
      mode_t mode = strtoul(header.mode, 0, 8);
      long int filesize = strtoul(header.filesize, 0, 8);

      switch(header.linkindicator) {
        case '0':
          untar_create_file_(fullpath, mode, filesize, fp);
          break;
		case '2':					
		  untar_create_link_(fullpath, header.linkedfile, filesize, fp);
		  break;
        case '5':
          untar_create_directory_(fullpath, mode, filesize, fp);
          break;
        default:
      }
    }
  }
}

