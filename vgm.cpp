#include <arduino.h>
#include <SdFat.h>

#include "vgm.h"
#include "ym2612.h"
#include "psg.h"

namespace thea {
namespace vgm {

#define SAMPLE_RATE 44100
// 1/44100 = 22.7 uS, will round to 22
#define SINGLE_SAMPLE_WAIT_FLOAT 22.7
#define SINGLE_SAMPLE_WAIT 22
#define ONE_SIXTIETH_WAIT 735 * SINGLE_SAMPLE_WAIT
#define ONE_FIFTIETH_WAIT 882 * SINGLE_SAMPLE_WAIT
#define PCM_BUFFER_SIZE 100 * 1024 //100kB

SdFatSdio vgm_sd;
File vgm_file;
byte pcm_buffer[PCM_BUFFER_SIZE];
uint32_t pcm_buffer_pos = 0;
uint32_t loop_offset = 0;

static bool char_array_equal(const char a[], const char b[], int len){
  for(int i = 0; i < len; i++){
    if(a[i] != b[i]) return false;
  }
  return true;
}

static void print_char_array(const char arr[], int len){
  for(int i = 0; i < len; i++) Serial.print(arr[i]);
  Serial.println();
}

static inline uint32_t read_uint32(File* file){
  /* Read a 32-bit value from the given file */
  byte v0 = file->read();
  byte v1 = file->read();
  byte v2 = file->read();
  byte v3 = file->read();
  return uint32_t(v0 + (v1 << 8) + (v2 << 16) + (v3 << 24));
}

static inline uint16_t read_uint16(File* file){
  /* Read a 16-bit value from the given file */
  byte v0 = file->read();
  byte v1 = file->read();
  return uint32_t(v0 + (v1 << 8));
}

static int read_vgm_header(File* file) {
  char marker[4];
  int err = file->read(marker, 4);

  if(err == -1) {
    Serial.println("Error reading header.");
    return -1;
  }

  if(!char_array_equal(marker, "Vgm ", 4)) {
    Serial.println("Invalid marker! Got:");
    print_char_array(marker, 4);
    return -1;
  }

  file->seekSet(0x34); // VGM Offset location
  uint32_t vgm_offset = read_uint32(file);

  Serial.println("VGM Offset:");
  Serial.println(vgm_offset);

  if(vgm_offset == 0x00 || vgm_offset == 0x0C) {
    // Standard offset 0x0C which means VGM data starts at 0x40.
    vgm_offset = 0x0C;
  }

  // Get the loop offset
  file->seekSet(0x1C);
  loop_offset = read_uint32(file);

  // Seek the file to the start of the VGM data.
  file->seekSet(0x34 + vgm_offset);

  return 0;
}

void init() {
  Serial.print("Initializing SD card...");

  if (!vgm_sd.begin()) {
    Serial.println("SD initialization failed!");
    return;
  }
  Serial.println("SD initialization done.");

  vgm_file = vgm_sd.open("song.vgm");

  if (!vgm_file) {
    Serial.println("No song.vgm found!");
    return;
  }

  if (read_vgm_header(&vgm_file) == -1) {
    Serial.println("Failed to read header!");
    vgm_file.close();
    return;
  }
}


void loop() {
  static unsigned long sleep_time_us = 0;
  static unsigned long last_time_us = 0;
  unsigned long loop_start_us = micros();

  if(!vgm_file) return;

  if(loop_start_us - last_time_us <= sleep_time_us) {
    return;
  }

  int cmd = vgm_file.read();

  switch(cmd) {
    // Game Gear PSG, skip
    case 0x4F: {
      vgm_file.read(); // skip argument
      last_time_us = loop_start_us;
      sleep_time_us = SINGLE_SAMPLE_WAIT;

      break;
    }

    // Send a byte to the SN.
    case 0x50: {
      byte data = vgm_file.read();
      thea::psg::send_byte(data);
      last_time_us = loop_start_us;
      sleep_time_us = SINGLE_SAMPLE_WAIT;

      break;
    }

    // Set YM registers
    case 0x52: {
      byte reg = vgm_file.read();
      byte data = vgm_file.read();
      thea::ym2612::set_reg(reg, data, 0);
      last_time_us = loop_start_us;
      sleep_time_us = SINGLE_SAMPLE_WAIT;

      break;
    }

    case 0x53: {
      byte reg = vgm_file.read();
      byte data = vgm_file.read();
      thea::ym2612::set_reg(reg, data, 1);
      last_time_us = loop_start_us;
      sleep_time_us = SINGLE_SAMPLE_WAIT;

      break;
    }

    // Custom wait
    case 0x61: {
      uint16_t wait = read_uint16(&vgm_file);
      last_time_us = loop_start_us;
      sleep_time_us = SINGLE_SAMPLE_WAIT * wait;

      break;
    }

    // Wait 1/60th of a second.
    case 0x62: {
      last_time_us = loop_start_us;
      sleep_time_us = ONE_SIXTIETH_WAIT;

      break;
    }

    // Wait 1/50th of a second.
    case 0x63: {
      last_time_us = loop_start_us;
      sleep_time_us = ONE_FIFTIETH_WAIT;

      break;
    }

    // PCM data.
    case 0x67: {
      // throw away 0x66 (STOP) byte and type bytes.
      vgm_file.read();
      vgm_file.read();
      uint32_t size = read_uint32(&vgm_file);

      if (size > PCM_BUFFER_SIZE) {
        Serial.println("PCM data too large!");
        Serial.println(size);
        // Skip the data.
        vgm_file.seekCur(size);
        break;
      }

      vgm_file.read(pcm_buffer, size);

      break;
    }

    // Wait x + 1 samples.
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x7D:
    case 0x7E:
    case 0x7F:{
      uint32_t wait_samples = cmd & 0x0F;

      last_time_us = loop_start_us;
      sleep_time_us = SINGLE_SAMPLE_WAIT * (wait_samples + 1);

      break;
    }

    // Write YM port 0 address 2A from the PCM data bank and wait.
    case 0x80:
    case 0x81:
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
    case 0x88:
    case 0x89:
    case 0x8A:
    case 0x8B:
    case 0x8C:
    case 0x8D:
    case 0x8E:
    case 0x8F: {
      uint32_t wait_samples = cmd & 0x0F;
      byte data = pcm_buffer[pcm_buffer_pos];
      pcm_buffer_pos++;
      thea::ym2612::set_reg(0x2A, data, 1);

      last_time_us = loop_start_us;
      sleep_time_us = SINGLE_SAMPLE_WAIT * wait_samples;

      break;
    }

    // Seek in PCM data bank
    case 0xE0: {
      pcm_buffer_pos = read_uint32(&vgm_file);

      break;
    }

    case 0x66: {
      if (loop_offset) {
        vgm_file.seekSet(loop_offset + 0x1C);
      } else {
        vgm_file.seekSet(0x40);
      }

      break;
    }

    case -1: {
      Serial.println("End of file.");
      vgm_file.close();
      return;
      break;
    }

    default: {
      Serial.println(cmd);
      break;
    }
  }
}

}; //namespace thea
}; //namespace vgm
