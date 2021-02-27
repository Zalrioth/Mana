#pragma once
#ifndef AUDIO_CLIP_H
#define AUDIO_CLIP_H

#include "mana/core/memoryallocator.h"
//
#include <mana/mana.h>
#define SOUNDIO_STATIC_LIBRARY
#include <sndfile.h>
#include <soundio.h>

enum AudioClipType {
  SOUND_AUDIO_CLIP,
  MUSIC_AUDIO_CLIP
};

struct AudioClipCache {
  SF_INFO sfinfo;
  SNDFILE* infile;
};

static inline int audio_clip_cache_init(struct AudioClipCache* audio_clip_cache, char* file_location) {
  if ((audio_clip_cache->infile = sf_open(file_location, SFM_READ, &(audio_clip_cache->sfinfo))) == NULL)
    return 1;

  return 0;
}

static inline void audio_clip_cache_delete(struct AudioClipCache* audio_clip_cache) {
  sf_close(audio_clip_cache->infile);
}

struct AudioClip {
  struct AudioClipCache* audio_clip_cache;
  enum AudioClipType audio_clip_type;
  float seconds_offset;
  float volume;
  int loop;
  float start_offset;
  int remove;
};

static inline int audio_clip_init(struct AudioClip* audio_clip, struct AudioClipCache* audio_clip_cache, enum AudioClipType audio_clip_type, int loop, float volume, float start_offset) {
  audio_clip->audio_clip_cache = audio_clip_cache;
  audio_clip->audio_clip_type = audio_clip_type;
  audio_clip->volume = volume;
  audio_clip->loop = loop;
  audio_clip->start_offset = start_offset;
  audio_clip->remove = 0;

  return 0;
}

#endif  // AUDIO_CLIP_H
