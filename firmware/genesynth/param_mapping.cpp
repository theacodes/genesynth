#include "param_mapping.h"
#include <cstdio>

namespace thea {
namespace params {

const char *param_names[] = {
    /* Global params */
    "Algorithm\0",
    "Feedback\0",
    "LFO Enable\0"
    "LFO Freq\0",
    "LFO FMS\0",
    "LFO AMS\0",
    /* Operator params */
    "OP1 DT\0",
    "OP1 MUL\0",
    "OP1 TL\0",
    "OP1 AR\0",
    "OP1 D1R\0",
    "OP1 D2R\0",
    "OP1 D1L\0",
    "OP1 RR\0",
    "OP1 RS\0",
    "OP1 AM\0",
    "OP2 DT\0",
    "OP2 MUL\0",
    "OP2 TL\0",
    "OP2 AR\0",
    "OP2 D1R\0",
    "OP2 D2R\0",
    "OP2 D1L\0",
    "OP2 RR\0",
    "OP2 RS\0",
    "OP2 AM\0",
    "OP3 DT\0",
    "OP3 MUL\0",
    "OP3 TL\0",
    "OP3 AR\0",
    "OP3 D1R\0",
    "OP3 D2R\0",
    "OP3 D1L\0",
    "OP3 RR\0",
    "OP3 RS\0",
    "OP3 AM\0",
    "OP4 DT\0",
    "OP4 MUL\0",
    "OP4 TL\0",
    "OP4 AR\0",
    "OP4 D1R\0",
    "OP4 D2R\0",
    "OP4 D1L\0",
    "OP4 RR\0",
    "OP4 RS\0",
    "OP4 AM\0",
};

ParamMapping mappings[100];

size_t get_num_params() { return sizeof(param_names) / sizeof(char *); }

ParamMapping get_mapping_for_param(uint8_t param) {
  mappings[param].param = param;
  return mappings[param];
}

ParamMapping get_mapping_for_cc(uint8_t midi_cc) {
  for (auto &&mapping : mappings) {
    if (mapping.midi_cc == midi_cc) {
      return mapping;
    }
  }

  return ParamMapping();
}
void set_mapping_for_param(ParamMapping mapping) { mappings[mapping.param] = ParamMapping(mapping); }

float map_value(Curves curve, float normalized) {
  switch (curve) {
  case Curves::LINEAR:
    // Nothing to do for the linear case.
    return normalized;
    break;

  case Curves::QUAD_IN:
    return ease_in_quad(normalized);
    break;
  case Curves::QUAD_OUT:
    return ease_out_quad(normalized);
    break;
  case Curves::CUBIC_IN:
    return ease_in_cubic(normalized);
    break;
  case Curves::CUBIC_OUT:
    return ease_out_cubic(normalized);
    break;
  case Curves::QUINT_IN:
    return ease_in_quint(normalized);
    break;
  case Curves::QUINT_OUT:
    return ease_out_quint(normalized);
    break;
  case Curves::EXPO_IN:
    return ease_in_expo(normalized);
    break;
  case Curves::EXPO_OUT:
    return ease_out_expo(normalized);
    break;
  default:
    return normalized;
    break;
  }
}

float map_range(ParamMapping mapping, float normalized) {
  float range_max = fmax(mapping.range_one, mapping.range_two) / 10.0f;
  float range_min = fmin(mapping.range_one, mapping.range_two) / 10.0f;
  float range = range_max - range_min;
  float mapped = normalized * range;
  return range_min + mapped;
}

} // namespace params
} // namespace thea
