#include <pebble.h>
void set_text_layer_d(TextLayer *layer, char *s, int sizeofs, char *format, int d) {
  snprintf(s, sizeofs, format, d);
  text_layer_set_text(layer, s);
}

void vibes_veryshort_pulse() {
  static const uint32_t const segments[] = { 40 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  vibes_enqueue_custom_pattern(pat);
}

void vibes_veryshort_number(int count) {
  static const uint32_t const segments[] = { 40, 50 };
  VibePattern pat = {
    .durations = segments,
    .num_segments = ARRAY_LENGTH(segments),
  };
  for(int i = 0; i < count; i++) {
    vibes_enqueue_custom_pattern(pat);
  }
}
