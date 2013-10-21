/*

Copyright (c) 2013, John A. Brunelle
All rights reserved.

*/


#include "cmapreduce_extra.h"
#include "mapreduce.h"
#include "keyvalue.h"

using namespace MAPREDUCE_NS;

void *MR_get_kv(void *MRptr)
{
  MapReduce *mr = (MapReduce *) MRptr;
  return (void *) mr->kv;
}
