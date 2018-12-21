project(btc)
cmake_minimum_required(VERSION 3.12)

set(CMAKE_C_STANDARD 11)

add_library(btc src/parser.c
                src/character.c
                src/tokenizer.c
                src/ast/ast_item.c
                src/ast/ast_list.c
                src/ast/container_declaration.c
                src/ast/container_group.c
                src/ast/container_param.c
                src/ast/containers_list.c
                src/ast/member_expression.c
                src/ast/namespace.c
                src/ast/template.c)
target_include_directories(btc PRIVATE src)

add_executable(btc_test test/test.c test/test_parser.c test/test_tokenizer.c)
target_link_libraries(btc_test PRIVATE btc)
target_include_directories(btc_test PRIVATE src)

