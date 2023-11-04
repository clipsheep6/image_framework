#ifndef HWE_SOURCE_RECORD_H
#define HWE_SOURCE_RECORD_H

#include "hwe_type.h"

void RecordMallocMemCount();
void RecordFreeMemCount();
void RecordInitMutexCount();
void RecordDestoryMutexCount();
void RecordInitCondCount();
void RecordDestoryCondCount();
void RecordInitThreadCount();
void RecordDestoryThreadCount();

void InitResourceInfo();
void DestroyResourceInfo();
#endif
