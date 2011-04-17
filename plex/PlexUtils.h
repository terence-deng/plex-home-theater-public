#pragma once

#include <string>

std::string GetHostName();

#ifdef _WIN32

#include <string>

bool Cocoa_IsHostLocal(const std::string& host);

struct timezone
{
  int  tz_minuteswest; /* minutes W of Greenwich */
  int  tz_dsttime;     /* type of dst correction */
};
 
int gettimeofday(struct timeval *tv, struct timezone *tz);

#endif
