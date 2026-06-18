#include <sys/stat.h>
#include <sys/time.h>
#include <errno.h>
#include <stdint.h>

#define SYS_gettimeofday 169
#define SYS_openat 56
#define SYS_close 57
#define SYS_read 63
#define SYS_write 64
#define SYS_fstat 80
#define SYS_lseek 62
#define SYS_exit 93
#define AT_FDCWD -100

static inline long sc4(long n,long a0,long a1,long a2,long a3){register long a7 __asm__("a7")=n;register long r0 __asm__("a0")=a0;register long r1 __asm__("a1")=a1;register long r2 __asm__("a2")=a2;register long r3 __asm__("a3")=a3;__asm__ volatile("ecall":"+r"(r0):"r"(a7),"r"(r1),"r"(r2),"r"(r3):"memory");return r0;}
static inline long sc2(long n,long a0,long a1){register long a7 __asm__("a7")=n;register long r0 __asm__("a0")=a0;register long r1 __asm__("a1")=a1;__asm__ volatile("ecall":"+r"(r0):"r"(a7),"r"(r1):"memory");return r0;}
static inline long sc1(long n,long a0){register long a7 __asm__("a7")=n;register long r0 __asm__("a0")=a0;__asm__ volatile("ecall":"+r"(r0):"r"(a7):"memory");return r0;}

extern "C" {
int _open(const char* p,int fl,int m){
    // Translate newlib flags to Linux flags
    int lfl = fl & 3;
    if (fl & 0x200) lfl |= 0x40;   // O_CREAT
    if (fl & 0x400) lfl |= 0x200;  // O_TRUNC
    if (fl & 0x800) lfl |= 0x80;   // O_EXCL
    if (fl & 0x8)   lfl |= 0x400;  // O_APPEND
    long r=sc4(SYS_openat,AT_FDCWD,(long)p,lfl,m);
    if(r<0){errno=-r;return -1;}return(int)r;}
int _close(int fd){long r=sc1(SYS_close,fd);if(r<0){errno=-r;return -1;}return 0;}
long _read(int fd,void* b,size_t n){long r=sc4(SYS_read,fd,(long)b,n,0);if(r<0){errno=-r;return -1;}return r;}
long _write(int fd,const void* b,size_t n){long r=sc4(SYS_write,fd,(long)b,n,0);if(r<0){errno=-r;return -1;}return r;}
int _fstat(int fd,struct stat* s){s->st_mode=0x2000;return 0;}
off_t _lseek(int fd,off_t o,int w){long r=sc4(SYS_lseek,fd,o,w,0);if(r<0){errno=-r;return -1;}return(off_t)r;}
int _isatty(int fd){return(fd==0||fd==1||fd==2)?1:0;}
// int _getpid(){return 1;}
// int _kill(int p,int s){errno=EINVAL;return -1;}
// void _exit handled by newlib
// gettimeofday handled by newlib
}
