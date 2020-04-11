#include "test_parser.h"
#include "test_tokenizer.h"

#include <stdio.h>

int main() {
    test_tokenizer();
    test_parser();
    printf("success\n");
}