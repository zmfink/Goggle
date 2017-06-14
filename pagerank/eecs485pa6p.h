#ifndef p6_eecs485pa6p_h
#define p6_eecs485pa6p_h

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <math.h>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <stdlib.h>     /* atof */

using namespace std;

struct Node{
	string word;
	vector<long int> incoming;
	vector<long int> outgoing;
	
	// Node(string inWord) {
	// 	word = inWord;
	// }
	double newPR;
	double oldPR;
};

#endif
