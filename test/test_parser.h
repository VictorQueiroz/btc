#ifndef TEST_PARSER_H_
#define TEST_PARSER_H_

#include "parser.h"

void test_parser();
void expect_ast_identifier(btc_ast_item*, const char*);
void expect_comment(btc_comments_list*, size_t, const char*);

#endif