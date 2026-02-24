#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define START 0

static inline unsigned set_add(unsigned set, int state) {
    return set | (1u << state);
}

static inline unsigned set_has(unsigned set, int state) {
    return (set >> state) & 1u;
}

static inline unsigned set_upper(unsigned set, char c) {
    return (c >= 'A' && c <= 'Z');
}

static inline unsigned set_lower(unsigned set, char c) {
    return ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

static unsigned move(unsigned state, int c) {
    unsigned next = 0;

    if (set_has(state, 0)) {
        if (c == '+') next = set_add(next, 1);
        if (set_upper(state, c)) next = set_add(next, 3);
    }

    if (set_has(state, 1)) {
        if (c == '+') next = set_add(next, 2);
    }

    if (set_has(state, 3)) {
        if (set_lower(state, c)) next = set_add(next, 4);
    }

    if (set_has(state, 4)) {
        if (set_lower(state, c)) next = set_add(next, 4);
    }

    return next;
}

static const char *best_token(unsigned state) {
    if (set_has(state, 2)) return "INCR";
    if (set_has(state, 1)) return "SUMA";
    if (set_has(state, 3) || set_has(state, 4)) return "ID";
    return NULL;
}

static char* read_file(const char *path, long *out_len) {
    FILE *file = fopen(path, "rb");
    if (!file) return NULL;

    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return NULL; }
    long len = ftell(file);
    if (len < 0) { fclose(file); return NULL; }
    rewind(file);

    char *buf = (char *)malloc((size_t)len + 1);
    if (!buf) { fclose(file); return NULL; }

    size_t read = fread(buf, 1, (size_t)len, file);
    fclose(file);

    if (out_len) *out_len = (long)read;
    buf[read] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.txt>\n", argv[0]);
        return 1;
    }

    long n;
    char *text = read_file(argv[1], &n);
    if (!text) { 
        fprintf(stderr, "Error al leer el archivo\n"); 
        return 1;
    }

    long i = 0;
    while (i < n) {
        unsigned char ch = (unsigned char)text[i];
        if (isspace(ch)) { i++; continue; }

        unsigned state = 0;
        state = set_add(state, START);

        long j = i;
        const char *best = NULL;
        long best_end = i;

        while (j < n && !isspace((unsigned char)text[j])) {
            state = move(state, (unsigned char)text[j]);
            if (state == 0) break;

            const char *tok = best_token(state);
            if (tok) {
                best = tok;
                best_end = j + 1;
            }

            j += 1;
        }

        if (!best) {
            printf("NO ACEPTA\n");
            free(text);
            return 0;
        }

        printf("%s\t", best);
        for (long k = i; k < best_end; k++) putchar(text[k]);
        putchar('\n');

        i = best_end;

    }

    free(text);
    return 0;
}


