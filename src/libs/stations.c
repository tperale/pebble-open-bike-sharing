#include "./stations.h"

static void Stations_free (Stations* self) {
  for (uint32_t i = 0; i < self->current; ++i) {
    self->stations[i]->free(self->stations[i]);
  }
  free(self->stations);

  free(self);
}

static void Stations_resize (Stations* self, uint32_t length) {
  if (length < self->current) {
    for (uint32_t i = length; i < self->current; ++i) {
      self->stations[i]->free(self->stations[i]);
    }
    self->current = length;
  } else if (length > self->current) {
    self->stations = realloc(self->stations, sizeof(Station*) * length);
    self->length = length;
  }
}

//! Decal the element of the list on the right.
static inline void Station_decal (Stations* self, uint32_t fromIndex) {
    uint32_t startFrom = self->current == self->length ? self->current - 1 : self->current;
    for (uint32_t i = startFrom; i >= fromIndex; --i) {
        self->stations[i + 1] = self->stations[i];
    }
}

static void Stations_add (Stations* self, Station* new) {
  if (self->current == self->length) {
    self->resize(self, self->length * 2);
  }

  for (uint32_t i = 0; i < self->current; ++i) {
    Station* tmp = self->stations[i];
    printf("i : %ld, distance : %ld.", i, tmp->distance);
    if (new->distance < tmp->distance) {
      Station_decal(self, i);
      self->current += 1;
      self->stations[i] = new;
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
  WARN("NO EQUALS : freeing the new");
  new->free(new);
}

static void Stations_insert (Stations* self, Station* new) {
  for (uint32_t i = 0; i < self->current; ++i) {
    Station* tmp = self->stations[i];
    if (new->distance < tmp->distance) {
      Station_decal(self, i);
      self->stations[i] = new;
      return;
    }
  }
}

Stations* Stations_new (uint32_t length) {
  Stations* s = malloc(sizeof(Stations));

  *s = (Stations) {
    .free = Stations_free,
    .resize = Stations_resize,
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
