#include "station.h"
#include "./pebble-assist.h"
#include "../appinfo.h"
#include <string.h>

static void Station_free (Station* self) {
  WARN("Freeing the 'stations' : %s.", self->name);
  free(self->name);
  free(self);
}

static Station* Station_copy(Station* self) {
  return Station_new (self->name,
      self->empty_slots,
      self->free_bike,
      self->distance,
      TRIGANGLE_TO_DEG(self->angle)
  );
}

Station* Station_new (char* name,
    uint32_t empty_slots,
    uint32_t free_bike,
    uint32_t distance,
    int32_t angle)
{
  Station* s = malloc(sizeof(Station));

  size_t name_length = strlen(name);

  *s = (Station) {
    .free = Station_free,
    .copy = Station_copy,
    .name = malloc(sizeof(char) * name_length),
    .empty_slots = empty_slots,
    .free_bike = free_bike,
    .distance = distance,
    .angle = DEG_TO_TRIGANGLE(angle),
  };
  memcpy(s->name, name, name_length);

  return s;
}

bool Stations_are_equals(Station* first, Station* second) {
  return strcmp(first->name, second->name) == 0;
}
