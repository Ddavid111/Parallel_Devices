#include "kernel_loader.h"

#include <stdio.h>
#include <stdlib.h>

char* load_kernel_source(const char* const path, int* error_code) {
    FILE* source_file;
    char* source_code;
    long file_size;

    source_file = fopen(path, "rb");
    if (source_file == NULL) {
        *error_code = -1; // Hiba: nem sikerült megnyitni a fájlt
        return NULL;
    }

    fseek(source_file, 0, SEEK_END);
    file_size = ftell(source_file);
    rewind(source_file);
    
    source_code = (char*)malloc(file_size + 1);
    if (source_code == NULL) {
        fclose(source_file);
        *error_code = -2; // Hiba: nem sikerült foglalni a memóriát
        return NULL;
    }

    if (fread(source_code, sizeof(char), file_size, source_file) != (size_t)file_size) {
        fclose(source_file);
        free(source_code);
        *error_code = -3; // Hiba: nem sikerült a fájl tartalmának olvasása
        return NULL;
    }
    source_code[file_size] = '\0';

    fclose(source_file);
    *error_code = 0; // Sikeres betöltés
    return source_code;
}
