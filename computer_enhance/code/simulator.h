#if !defined(_SIMULATOR_H_)
#define _SIMULATOR_H_

#include "ecb_utils.h"

file_scope u8 Memory[MegaBytes(1)];

typedef plex
{
  //@TODO(Emilio): CPU Registers go here.
  s32 Placeholder;


} CPU;


#endif //@NOTE(Emilio): _SIMULATOR_H_
