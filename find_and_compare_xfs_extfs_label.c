#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#define EVOLNAMSZ 16
#define XVOLNAMSZ 12
#define XMAGIC "XFSB"
#define EMAGIC 0xEF53
#define VNAME "quobyte-dev"

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "Usage: %s <devname>\n", argv[0]);
    return(-1);
  }

  unsigned char emagic[2];
  char xmagic[5];
  char elabel[EVOLNAMSZ+1];
  char xlabel[XVOLNAMSZ+1];
  int fd = open(argv[1], O_RDONLY);
  if(fd < 0) {
    fprintf(stderr, "Can't open %s: %s\n", argv[1], strerror(errno));
    return(127);
  }

  if(pread(fd, xmagic, 4, 0) != 4) {
    fprintf(stderr, "Error reading first 4 bytes of %s\n", argv[1]);
    close(fd);
    return(128);
  }

  xmagic[4] = '\0';
  if(strncmp(xmagic, XMAGIC, 4) == 0) {
    if(pread(fd, xlabel, XVOLNAMSZ, 108) != XVOLNAMSZ) {
      fprintf(stderr, "Error reading xfs label of %s\n", argv[1]);
      close(fd);
      return(129);
    } else {
      xlabel[XVOLNAMSZ] = '\0';
      if(strncmp(xlabel, VNAME, XVOLNAMSZ) == 0) {
        printf("Found a quobyte disk formated with xfs label='%s'\n", xlabel);
      } else {
        fprintf(stderr, "Found non-matching label '%s'\n", xlabel);
      }
    }
  } else {
    if(pread(fd, emagic, 2, 1080) != 2) {
      fprintf(stderr, "Error reading extfs magic number of %s\n", argv[1]);
      close(fd);
      return(130);
    }

    fprintf(stderr, "[0] = %x [1] = %x\n", emagic[0], emagic[1]);
    if(emagic[0] + 256*emagic[1] != EMAGIC) {
      fprintf(stderr, "Unable to find magic number for xfs and extfs, not a quobyte disk.\n");
      fprintf(stderr, "[0] = %x [1] = %x\n", emagic[0], emagic[1]);
      close(fd);
      return(135);
    }

    if(pread(fd, elabel, EVOLNAMSZ, 1144) != EVOLNAMSZ) {
      fprintf(stderr, "Error reading extfs label of %s\n", argv[1]);
      close(fd);
      return(131);
    } else {
      elabel[EVOLNAMSZ] = '\0';
      if(strncmp(elabel, VNAME, EVOLNAMSZ) == 0) {
        printf("Found a quobyte disk formated with extfs label='%s'\n", elabel);
      } else {
        fprintf(stderr, "Found non-matching label '%s'\n", elabel);
      }
    }
  }

  close(fd);
  return(0);
}

