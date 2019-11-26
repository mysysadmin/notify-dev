#include <stdio.h>
#include <string.h>
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

int main(int argc, char **argv) {
  if(!inotifytools_initialize() || !inotifytools_watch_recursively("/dev/disk/by-uuid/", IN_CREATE|IN_DELETE|IN_DONT_FOLLOW)) {
    fprintf(stderr, "Unable to watch /dev/disk/by-uuid: %s\n", strerror(inotifytools_error()));
    return(-1);
  }

  inotifytools_set_printf_timefmt( "%T" );
  struct inotify_event * event = inotifytools_next_event( -1 );
  while ( event ) {
    inotifytools_printf( event, "%T %w%f %e\n" );
    event = inotifytools_next_event( -1 );
  }

  return(0);
}

