#ifndef THEA_PARAM_MAPPING_H
#define THEA_PARAM_MAPPING_H

#include <math.h>
#include <stddef.h>
#include <stdint.h>

namespace thea {
namespace params {

enum struct Curves : uint8_t {
  LINEAR,
  QUAD_IN,
  QUAD_OUT,
  CUBIC_IN,
  CUBIC_OUT,
  QUINT_IN,
  QUINT_OUT,
  EXPO_IN,
  EXPO_OUT,
  NUM_CURVES,
};

struct ParamMapping {
  uint8_t param = 0;
  uint8_t midi_cc = 0;
  Curves curve = Curves::LINEAR;
};

extern const char *param_names[];
size_t get_num_params();
ParamMapping get_mapping_for_param(uint8_t param);
ParamMapping get_mapping_for_cc(uint8_t midi_cc);
void set_mapping_for_param(ParamMapping mapping);
float map_value(Curves curve, float normalized);

inline float ease_in_quad(float x) { return x * x; }
inline float ease_out_quad(float x) { return 1.0f - (1.0f - x) * (1.0f - x); }
inline float ease_in_cubic(float x) { return x * x * x; }
inline float ease_out_cubic(float x) { return 1.0f - pow(1.0f - x, 3.0f); }
inline float ease_in_quint(float x) { return x * x * x * x * x; }
inline float ease_out_quint(float x) { return 1.0f - pow(1.0f - x, 5.0f); }
inline float ease_in_expo(float x) { return x == 0.0f ? 0.0f : pow(2.0f, 10.0f * x - 10.0f); }
inline float ease_out_expo(float x) { return x == 1.0f ? 1.0f : 1.0f - pow(2.0f, -10.0f * x); }

} // namespace params
} // namespace thea

#endif // THEA_PARAM_MAPPING_H
