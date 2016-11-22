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

void vibes_veryshort_number(int count, int length, int delay) {
  if (count > 5) {
    count = 5;
  }
  static uint32_t segments[] = { 40, 90, 40, 90, 40, 90, 40, 90, 40};
  for(int i = 0; i < count; i++) {
    segments[i * 2] = length;
    if (i > 0) {
      segments[i * 2 - 1] = delay;
    }
  }

  VibePattern pat = {
    .durations = segments,
    .num_segments = (count - 1) * 2 + 1,
  };
  vibes_enqueue_custom_pattern(pat);
}
