#ifndef TEST_PARSER_H_
#define TEST_PARSER_H_

#include "parser.h"

void test_parser();
void expect_ast_identifier(btc_ast_item* item, const char* name);

#endif