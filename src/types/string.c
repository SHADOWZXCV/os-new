#include "types/string.h"

byte strcmp(string s1, string s2) {
    while(*s1) {
        if (!*s2) {
            return false;
        }
        if (*s1 != *s2) {
            return false;
        }
        s1++;
        s2++;
    }

    if (*s2) {
        return false;
    }

    return true;
}
