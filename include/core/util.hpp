#pragma once
#ifndef UTIL_HPP_
#define UTIL_HPP_

//#include <IL/il.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <thread>

// https://msdn.microsoft.com/en-us/library/4141z1cx.aspx

const char* readShaderFileToMemory(const char* filePath);
double getTime();
void sleep(int amount);

#endif  // UTIL_HPP_