#pragma once
#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H
#undef __cplusplus
#include "mana/core/memoryallocator.h"
//
#include <mana/mana.h>
#define SOUNDIO_STATIC_LIBRARY
#include <sndfile.h>
#include <soundio.h>
#undef __cplusplus

#include "mana/audio/audioclip.h"

#define AUDIO_BUFFER 2 * 1024

struct AudioManager {
  struct SoundIoOutStream* outstream;
  struct SoundIoDevice* device;
  struct SoundIo* soundio;
  struct ArrayList audio_clips;
  struct ArrayList add_audio_clips;
  float master_volume;
  int alive;
};

// NOTE: CRASH OCCURS ON END RIGHT NOW
//Exception thrown at 0x00007FFD97811655 (VkLayer_khronos_validation.dll) in DadnMe.exe: 0xC0000005: Access violation reading location 0x0000000000000000.
static inline void write_callback(struct SoundIoOutStream* outstream, int frame_count_min, int frame_count_max) {
  // Needed to determine max output channels
  const struct SoundIoChannelLayout* layout = &outstream->layout;
  struct SoundIoChannelArea* areas;
  float float_sample_rate = (float)outstream->sample_rate;
  float seconds_per_frame = 1.0f / float_sample_rate;
  int err;

  struct AudioManager* audio_manager = outstream->userdata;
  struct ArrayList* audio_clips = &audio_manager->audio_clips;
  struct ArrayList* add_audio_clips = &audio_manager->add_audio_clips;

  // Master volume
  soundio_outstream_set_volume(outstream, audio_manager->master_volume);

  int added_clip = 0;
  for (int add_audio_clip_num = 0; add_audio_clip_num < array_list_size(add_audio_clips); add_audio_clip_num++) {
    array_list_add(audio_clips, array_list_get(add_audio_clips, add_audio_clip_num));
    added_clip++;
  }
  if (added_clip > 0)
    array_list_clear(add_audio_clips);

  int device_channels = layout->channel_count;
  int buffer_size = AUDIO_BUFFER;
  if ((err = soundio_outstream_begin_write(outstream, &areas, &buffer_size))) {
    fprintf(stderr, "%s\n", soundio_strerror(err));
    exit(1);
  }

  if (!buffer_size)
    return;

  for (int frame = 0; frame < AUDIO_BUFFER; frame++) {
    for (int channel = 0; channel < device_channels; channel++) {
      float* ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
      *ptr = 0.0f;
    }
  }

  for (int audio_clip_num = 0; audio_clip_num < array_list_size(audio_clips); audio_clip_num++) {
    struct AudioClip* audio_clip = array_list_get(audio_clips, audio_clip_num);
    SNDFILE* infile = audio_clip->audio_clip_cache->infile;
    SF_INFO sfinfo = audio_clip->audio_clip_cache->sfinfo;

    int channels = sfinfo.channels;

    int frames_left = AUDIO_BUFFER;
    int readcount = 0;
    while (frames_left > 0 && audio_clip->remove == 0) {
      // * 2 is not needed here but prevents int/float rounding errors?
      float* buff = calloc(channels * frames_left * 2, sizeof(float));
      size_t seek_offset = audio_clip->seconds_offset * (float_sample_rate * ((float)sfinfo.samplerate / float_sample_rate));
      sf_seek(infile, seek_offset, SEEK_SET);
      // buff causing crash?
      readcount += sf_readf_float(infile, buff + ((AUDIO_BUFFER - frames_left) * device_channels), frames_left);
      //printf("Frame count: %d Read count: %d\n", frames_left, readcount);

      for (int frame = 0; frame < readcount; frame++) {
        for (int channel = 0; channel < channels; channel++) {
          // Mono, only one speaker?
          //float sample = buff[(int)(seconds_offset + frame * float_sample_rate) + channel];
          // Stereo
          float sample = buff[(int)(audio_clip->seconds_offset + frame * ((float)sfinfo.samplerate / float_sample_rate)) * 2 + channel];
          float* ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
          *ptr += (sample * audio_clip->volume);
          if (*ptr > 1.0f)
            *ptr = 1.0f;
        }
      }

      free(buff);

      // This through memory exception error
      audio_clip->seconds_offset = audio_clip->seconds_offset + seconds_per_frame * readcount;
      if (readcount < frames_left && audio_clip->loop == 1)
        audio_clip->seconds_offset = 0.0f;
      // TEMP to fix overreading file
      else if (readcount < frames_left && audio_clip->loop == 0) {
        audio_clip->seconds_offset = 0.0f;
        audio_clip->remove = 1;
      } else
        break;

      frames_left -= readcount;
    }
  }

  if ((err = soundio_outstream_end_write(outstream))) {
    fprintf(stderr, "%s\n", soundio_strerror(err));
    exit(1);
  }

  for (int audio_clip_num = array_list_size(audio_clips) - 1; audio_clip_num >= 0; audio_clip_num--) {
    struct AudioClip* audio_clip = array_list_get(audio_clips, audio_clip_num);
    if (audio_clip->remove == 1) {
      audio_clip->remove = 0;
      audio_clip->seconds_offset = 0.0f;
      array_list_remove(audio_clips, audio_clip_num);
    }
  }
}

static inline int audio_manager_init(struct AudioManager* audio_manager) {
  audio_manager->alive = 1;
  audio_manager->master_volume = 1.0f;

  int err;
  audio_manager->soundio = soundio_create();
  if (!audio_manager->soundio) {
    fprintf(stderr, "out of memory\n");
    return 1;
  }

  if ((err = soundio_connect(audio_manager->soundio))) {
    fprintf(stderr, "error connecting: %s", soundio_strerror(err));
    return 1;
  }

  soundio_flush_events(audio_manager->soundio);

  int default_out_device_index = soundio_default_output_device_index(audio_manager->soundio);
  if (default_out_device_index < 0) {
    fprintf(stderr, "no output device found");
    return 1;
  }

  audio_manager->device = soundio_get_output_device(audio_manager->soundio, default_out_device_index);
  if (!audio_manager->device) {
    fprintf(stderr, "out of memory");
    return 1;
  }

  fprintf(stderr, "Output device: %s\n", audio_manager->device->name);

  audio_manager->outstream = soundio_outstream_create(audio_manager->device);
  if (!audio_manager->outstream) {
    fprintf(stderr, "out of memory\n");
    return 1;
  }

  array_list_init(&audio_manager->add_audio_clips);
  array_list_init(&audio_manager->audio_clips);

  audio_manager->outstream->userdata = audio_manager;
  audio_manager->outstream->format = SoundIoFormatFloat32NE;
  audio_manager->outstream->write_callback = write_callback;

  if ((err = soundio_outstream_open(audio_manager->outstream))) {
    fprintf(stderr, "unable to open device: %s", soundio_strerror(err));
    return 1;
  }

  if (audio_manager->outstream->layout_error)
    fprintf(stderr, "unable to set channel layout: %s\n", soundio_strerror(audio_manager->outstream->layout_error));

  if ((err = soundio_outstream_start(audio_manager->outstream))) {
    fprintf(stderr, "unable to start device: %s", soundio_strerror(err));
    return 1;
  }

  return 0;
}

static inline void audio_manager_delete(struct AudioManager* audio_manager) {
  audio_manager->alive = 0;

  soundio_outstream_destroy(audio_manager->outstream);
  soundio_device_unref(audio_manager->device);
  soundio_destroy(audio_manager->soundio);

  array_list_delete(&audio_manager->audio_clips);
  array_list_delete(&audio_manager->add_audio_clips);
}

static inline void audio_manager_start(struct AudioManager* audio_manager) {
  while (audio_manager->alive)
    soundio_wait_events(audio_manager->soundio);
}

static inline void audio_manager_play_audio_clip(struct AudioManager* audio_manager, struct AudioClip* audio_clip) {
  array_list_add(&audio_manager->add_audio_clips, audio_clip);
}

static inline void audio_manager_stop_audio_clip() {
}

#endif  // AUDIO_MANAGER_H
