#ifndef _COMMON_H
#define _COMMON_H

#include "stdlib.h"
#include <vector>
#include <string>
#include <sstream>

using namespace std;

/* length(char *str)
*
* T         - array type.
*
*/
template<typename T>
int length(T *str)
{
	return _msize(str) / sizeof(T);
}

/* split(const string &s, char delim)
* 
* s         - input string.
* delim     - delimiter.
* elems     - return substrings.
*
*/
void split(const string &s, char delim, vector<string> &elems)
{
	stringstream ss;
	ss.str(s);
	string item;
	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}
}
//vector<string> split(const string &s, char delim)
//{
//	vector<string> elems;
//	split(s, delim, elems);
//	return elems;
//}

/* ulrand()
*
* return - ul
*
*/
unsigned long ulrand() {
	return (
		(((unsigned long)rand() << 24) & 0xFF000000ul)
		| (((unsigned long)rand() << 12) & 0x00FFF000ul)
		| (((unsigned long)rand()) & 0x00000FFFul));
}

#endif