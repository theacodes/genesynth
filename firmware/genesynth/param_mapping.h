#ifndef THEA_PARAM_MAPPING_H
#define THEA_PARAM_MAPPING_H

#include <stddef.h>
#include <stdint.h>

namespace thea {
namespace params {

enum struct Curves : uint8_t {
  LINEAR,
  EXPO_IN,
  EXPO_OUT,
  CUBIC_IN,
  CUBIC_OUT,
  NUM_CURVES,
};

struct ParamMapping {
  uint8_t param;
  uint8_t midi_cc;
  uint8_t curve;
};

extern const char *param_names[];
size_t get_num_params();
ParamMapping get_mapping_for_param(uint8_t param);
ParamMapping get_mapping_for_cc(uint8_t midi_cc);
void set_mapping_for_param(ParamMapping mapping);

} // namespace params
} // namespace thea

#endif // THEA_PARAM_MAPPING_H