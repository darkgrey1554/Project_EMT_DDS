#pragma once

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include  <stddef.h>

static volatile  sig_atomic_t hupReceived = 0;

//#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

void sigterm(int signo);

void sighup(int signo);

void sigstop(int signo);

sig_atomic_t get_sign();

void set_sign(int a);

unsigned int InitDaemon();

