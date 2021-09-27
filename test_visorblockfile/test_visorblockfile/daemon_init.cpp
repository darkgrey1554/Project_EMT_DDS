#include "daemon_init.h"

void sigterm(int signo)
{
    hupReceived = 1;
}

void sighup(int signo)
{
    hupReceived = 2;
}

void sigstop(int signo)
{
    hupReceived = 3;
}

sig_atomic_t get_sign()
{
    return hupReceived;
}

void set_sign(int a)
{
    hupReceived = a;
}

unsigned int InitDaemon()
{
    struct rlimit rl;
    pid_t pid;
    struct sigaction sa;
    int fd0, fd1, fd2;
    int f;
    unsigned result = 0;

    umask(0);
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        result |= 1;
    }

    pid = fork();
    if (pid < 0) { result |= 2; return result; }
    else if (pid != 0) exit(0);
    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    pid = fork();
    if (pid < 0) { result |= 4; return result; }
    else if (pid != 0) exit(0);

    chdir("/");

    if (rl.rlim_max == RLIM_INFINITY) rl.rlim_max = 1024;
    for (int i = 0; i < rl.rlim_max; i++)
    {
        close(i);
    }

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    struct flock lock;
    int res;
    lock.l_type = F_RDLCK | F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_pid = 0;

    res = fcntl(f, F_SETLK, &lock);

    sa.sa_handler = sigterm;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGTERM);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
  

    sa.sa_handler = sighup;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    sigaction(SIGHUP, &sa, NULL);
 

    sa.sa_handler = sigstop;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGSTOP);
    sa.sa_flags = 0;
    sigaction(SIGSTOP, &sa, NULL);

    return result;
}
