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

struct AudioClip {
  SF_INFO sfinfo;
  SNDFILE* infile;
  enum AudioClipType audio_clip_type;
  float seconds_offset;
  float volume;
  int loop;
  int remove;
};

static inline int audio_clip_init(struct AudioClip* audio_clip, char* file_location, enum AudioClipType audio_clip_type, int loop) {
  SNDFILE* infile = NULL;
  SF_INFO sfinfo;
  if ((infile = sf_open(file_location, SFM_READ, &sfinfo)) == NULL)
    return 1;

  audio_clip->infile = infile;
  audio_clip->sfinfo = sfinfo;
  audio_clip->audio_clip_type = audio_clip_type;
  audio_clip->loop = loop;
  audio_clip->remove = 0;
  audio_clip->volume = 0.5f;

  return 0;
}

static inline int audio_clip_delete(struct AudioClip* audio_clip) {
  sf_close(audio_clip->infile);
}

#endif  // AUDIO_CLIP_H
