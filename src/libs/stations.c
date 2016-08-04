#include "./stations.h"

static void Stations_free (Stations* self) {
  for (uint32_t i = 0; i < self->current; ++i) {
    self->stations[i]->free(self->stations[i]);
  }
  free(self->stations);

  free(self);
}

//! Decal the element of the list on the right.
static inline void Station_decal (Station** array, uint32_t fromIndex, uint32_t arrayLength) {
    for (uint32_t i = arrayLength; i >= fromIndex; --i) {
        array[i + 1] = array[i];
    }
}

static void Stations_add (Stations* self, Station* new) {
  if (self->current == self->length) {
    self->length *= 2;
    Station** tmp = self->stations;
    self->stations = realloc(self->stations, self->length);
    free(tmp);
  }

  for (uint32_t i = 0; i < self->current; ++i) {
    Station* tmp = self->stations[i];
    if (new->distance < tmp->distance) {
      Station_decal(self->stations, i, self->current);
      return;
    }
  }

  self->stations[self->current] = new;
  self->current += 1;
}

static void Stations_update (Stations* self, Station* new) {
  for (uint32_t i = 0; i < self->current; ++i) {
    Station* tmp = self->stations[i];
    if (Stations_are_equals(tmp, new)) {
      tmp->free(tmp);
      self->stations[i] = new;
      return;
    }
  }
}

static void Stations_insert (Stations* self, Station* new) {
  for (uint32_t i = 0; i < self->current; ++i) {
    Station* tmp = self->stations[i];
    if (new->distance < tmp->distance) {
      Station_decal(self->stations, i, self->current);
      return;
    }
  }
}

Stations* Stations_new (uint32_t length) {
  Stations* s = malloc(sizeof(Stations));

  *s = (Stations) {
    .free = Stations_free,
    .add = Stations_add,
    .update = Stations_update,
    .insert = Stations_insert,
    .length = length,
    .current = 0,
    .stations = malloc(sizeof(Station) * length),
  };

  return s;
}


Station* Stations_next (Stations* s, Station* current) {
  if (s->current == 0) {
    return NULL;
  }

  Station* result = s->stations[0];

  for (uint32_t i = 0; i < s->current; ++i) {
    if (s->stations[i]->distance <= current->distance ) {
      // Next should a greater distance than the current one.
      continue;
    }

    printf("res is %ld", i);
    result = s->stations[i];
    break;
  }

  return result;
}

Station* Stations_previous (Stations* s, Station* current) {
  if (s->current == 0) {
    return NULL;
  }

  uint32_t last_index = s->current - 1;
  Station* result = s->stations[last_index];
  for (uint32_t i = last_index; i > 0; --i) {
    if (current->distance <= s->stations[i]->distance) {
      // Previous value should a shorter distance than the current one.
      continue;
    }

    result = s->stations[i];
    return result;
  }

  if (current->distance > s->stations[0]->distance) {
    result = s->stations[0];
  }

  return result;
}
