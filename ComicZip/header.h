#pragma once

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <io.h>
#include<time.h>
#include<sstream>
#include <Windows.h>
#include <direct.h>
#include"zip.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;



void zipFolder(string path, string name);
string findFileList(string path, string name);