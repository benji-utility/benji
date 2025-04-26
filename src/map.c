#include "include/map.h"

map_t* map_init() {
    map_t* map = malloc(sizeof(map_t));

    if (!map) {
        return NULL;
    }

    map->keys = malloc(BENJI_CAPACITY(BENJI_MAP_INITIAL_CAPACITY, char*));
    map->values = malloc(BENJI_CAPACITY(BENJI_MAP_INITIAL_CAPACITY, char*));

    if (!map->keys || !map->values) {
        return NULL;
    }

    map->size = 0;
    map->capacity = BENJI_MAP_INITIAL_CAPACITY;

    return map;
}

void map_insert(map_t* map, const char* key, const char* value) {
    int index = map_get_index_of(map, key);

    if (index != BENJI_MAP_KEY_NOT_FOUND) {
        free(map->values[index]);

        map->values[index] = strdup(value);
    }
    else {
        if(map->size == map->capacity) {
            map_resize(map, map->capacity * 2); // key + value = 2 new elements to be allocated for
        }

        map->keys[map->size] = strdup(key);
        map->values[map->size] = strdup(value);

        map->size++;
    }
}

const char* map_get(map_t* map, const char* key) {
    int index = map_get_index_of(map, key);

    if (index == BENJI_MAP_KEY_NOT_FOUND) {
        return NULL;
    }

    return map->values[index];
}

int map_get_index_of(map_t* map, const char* key) {
    for (size_t i = 0; i < map->size; i++) {
        if (BENJI_STRING_EQUALS(map->keys[i], key)) {
            return i;
        }
    }

    return BENJI_MAP_KEY_NOT_FOUND;
}

char* map_serialize(map_t* map, const char* header) {
    char* block = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH * map->capacity, char));

    if (!block) {
        return "";
    }

    block[0] = '\0';

    char* buffer = malloc(BENJI_CAPACITY(BENJI_BASIC_STRING_LENGTH, char));

    if (!buffer) {
        return "";
    }

    buffer[0] = '\0';

    sprintf(buffer, "\"%s\": {", header);
    strcat(block, buffer);

    size_t i = 0;
    for (i; i < map->size - 1; i++) {
        sprintf(buffer, "\"%s\": \"%s\",", map->keys[i], map->values[i]);
        strcat(block, buffer);
    }

    sprintf(buffer, "\"%s\": \"%s\"", map->keys[i], map->values[i]);
    strcat(block, buffer);

    strcat(block, "}");

    return block;
}

void map_resize(map_t* map, size_t new_capacity) {
    map->keys = realloc(map->keys, BENJI_CAPACITY(new_capacity, char*));
    map->values = realloc(map->values, BENJI_CAPACITY(new_capacity, char*));

    map->capacity = new_capacity;
}

void map_free(map_t* map) {
    for (size_t i = 0; i < map->size; i++) {
        free(map->keys[i]);
        free(map->values[i]);
    }

    free(map->keys);
    free(map->values);

    free(map);
}