#include <stdio.h>

int pow(int base, int exp)
{
    if(exp < 0)
    return -1;

    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

int main() {
    int input = (int) -150501;
	unsigned int number;

    if (input < 0) {
        putchar('-');
        number = (unsigned int)(-(long)input);
    } else {
        number = (unsigned int) input;
    }

	int divider = 1;
	char cell = 0;

    // find out the biggest digit on our number
    while (number / divider >= 10) {
        divider *= 10;
	}

    while (divider) {
        cell = number / divider;
        number %= divider;
        divider /= 10;
        putchar('0' + cell);
    }


    return 0;
}