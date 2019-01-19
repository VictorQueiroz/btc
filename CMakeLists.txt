project(btc)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_C_STANDARD 11)
add_subdirectory(deps/cvector)

add_library(btc src/parser.c
                src/tokenizer/character.c
                src/tokenizer/tokenizer.c
                src/tokenizer/token.c
                src/tokenizer/tokens_list.c
                src/ast/ast_item.c
                src/ast/ast_list.c
                src/ast/comments_list.c
                src/ast/comments_list.h
                src/ast/container_declaration.c
                src/ast/container_group.c
                src/ast/container_param.c
                src/ast/containers_list.c
                src/ast/member_expression.c
                src/ast/namespace.c
                src/ast/template.c src/range.h src/ast/comment.c src/ast/template_declaration.h)
target_include_directories(btc PRIVATE src)
target_link_libraries(btc PUBLIC cvector)

add_executable(btc_test test/test.c test/test_parser.c test/test_tokenizer.c)
target_link_libraries(btc_test PRIVATE btc)
target_include_directories(btc_test PRIVATE src)

