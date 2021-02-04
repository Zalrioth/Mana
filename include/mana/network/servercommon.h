#pragma once
#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H

#if defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
#define PLATFORM_WIN32
#else
#define PLATFORM_OTHER
#endif

#endif  // SERVER_COMMON_H
