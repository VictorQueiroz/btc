cmake_minimum_required(VERSION 3.11)
project(btc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -Wextra -pedantic -Werror")

include_directories(src)

add_library(btc
        src/ast/alias.h
        src/ast/ast_item.c
        src/ast/ast_item.h
        src/ast/ast_list.c
        src/ast/ast_list.h
        src/ast/container_declaration.c
        src/ast/container_declaration.h
        src/ast/container_group.c
        src/ast/container_group.h
        src/ast/container_param.c
        src/ast/container_param.h
        src/ast/containers_list.c
        src/ast/containers_list.h
        src/ast/import_declaration.h
        src/ast/member_expression.c
        src/ast/member_expression.h
        src/ast/namespace.c
        src/ast/namespace.h
        src/ast/number.h
        src/ast/parser_identifier.h
        src/ast/string.h
        src/ast/template.c
        src/ast/template.h
        src/btc.h
        src/character.c
        src/character.h
        src/parser.c
        src/parser.h
        src/tokenizer.c
        src/tokenizer.h)

add_executable(btc_test
        test/test.c
        test/test_parser.c
        test/test_tokenizer.c)
target_link_libraries(btc_test btc)
target_include_directories(btc_test PRIVATE src)
