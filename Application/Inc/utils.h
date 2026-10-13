#ifndef _UTILS_H_
#define _UTILS_H_

#include "userdefine.h"

void PrintAddress(uint32_t address);
void Dump(const void *address, size_t size);
void PrintParam(const char *pre, const Parameters_t *param);

#endif // _UTILS_H_