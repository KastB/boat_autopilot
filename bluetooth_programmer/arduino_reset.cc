#include <sys/ioctl.h>
#include <termios.h>
#include <linux/serial.h> 
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stropts.h>
/*

int setdtr (int fd, int on)
{
  int controlbits = TIOCM_DTR;
  if(on)
    return(ioctl(tty_fd, TIOCMBIC, &controlbits));
  else
    return(ioctl(tty_fd, TIOCMBIS, &controlbits));
} 
*/
int main()
{
  const char *dev="/dev/rfcomm0";
  int fd=open(dev,O_RDONLY);
  if(fd<0)
  {
    fprintf(stderr,"Couldn't open %s\n",dev);
    return(1);
  }

  fprintf(stderr,"Opened\n");
 // setdtr(fd,1);

  // Pause for three seconds
  sleep(30);

  fprintf(stderr,"Clearing DTR\n");
 // setdtr(fd,0);

  close(fd);
  return(0);
}
