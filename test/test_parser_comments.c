#include "test_parser.h"

#include <btc.h>
#include <assert.h>
#include <stdio.h>

void test_parser_comments() {
    const char comments_schema[] = {
        "import \"extra.txt\";\n\n"
        "alias PostsList = Vector<Post>;\n"
        "type Post {\n"
        "   /**\n"
        "    * Post created by an user"
        "    */\n"
        "   post -> user.User author"
        "}\n\n"
        "namespace user {\n"
        "    type UserStatus {\n"
        "       userStatusNormal\n"
        "       userStatusDeleted\n"
        "    }\n\n"
        "    type User {\n"
        "        /**\n"
        "         * Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla dolor ligula, porttitor\n"
        "         * non semper nec, ornare eget mauris. Cras posuere purus elit, non ultricies nisi convallis\n"
        "         * quis. Ut vitae nunc magna. Cras in tortor non ex tincidunt vestibulum. Nullam sit amet rutrum\n"
        "         * velit, nec dictum enim. Proin commodo sollicitudin sollicitudin. Integer ultrices nisl turpis,\n"
        "         * ut vehicula nunc ultricies ac. Ut ultrices blandit est et euismod. Donec eget placerat nisi,\n"
        "         * ut varius nibh. Nulla ut velit quis dui suscipit tempus. Praesent eleifend luctus augue, id\n"
        "         * volutpat lorem aliquet vel.\n"
        "         *\n"
        "         * Phasellus egestas auctor massa eget viverra. Nulla consectetur elementum nulla et condimentum.\n"
        "         * Curabitur eget maximus mauris, vel aliquet nisi. Pellentesque neque tortor, eleifend ac viverra\n"
        "         * vitae, faucibus sit amet risus. Fusce et feugiat justo. Nam iaculis vulputate mi non interdum.\n"
        "         * Vivamus sed nulla tellus. Suspendisse neque odio, pharetra eget suscipit nec, placerat at elit.\n"
        "         * Maecenas lobortis erat ac tortor sodales, et cursus est venenatis. Vivamus sed orci ac diam\n"
        "         * convallis accumsan. Mauris pretium leo eu lorem consequat dapibus. Duis quam ante, pretium vel\n"
        "         * erat ut, porta commodo nibh. Sed accumsan neque mi, eu scelerisque nulla ornare in. Sed pretium\n"
        "         * augue tortor, ut auctor arcu gravida eget. Aliquam urna nunc, ultrices eget interdum sed,\n"
        "         * venenatis feugiat magna.\n"
        "         */\n"
        "        user -> UserStatus status\n"
        "    }\n"
        "}"
    };
    btc_tokenizer* t;
    btc_tokenizer_init(&t);
    assert(btc_tokenizer_scan(t, comments_schema) == BTC_OK);

    btc_parser* parser;
    btc_parser_init(&parser, t);
    assert(btc_parse(parser) == BTC_OK);

    btc_ast_item* node = btc_ast_list_get(parser->result, 3);
    assert(node->type == BTC_NAMESPACE);

    node = btc_ast_list_get(node->namespace_item->body, 1);
    assert(node->type == BTC_CONTAINER_GROUP);

    node = btc_ast_list_get(node->container_group->body, 0);
    assert(node->type == BTC_CONTAINER_DECLARATION);

    const char expected_comment[] = {
        "*\n"
        "         * Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla dolor ligula, porttitor\n"
        "         * non semper nec, ornare eget mauris. Cras posuere purus elit, non ultricies nisi convallis\n"
        "         * quis. Ut vitae nunc magna. Cras in tortor non ex tincidunt vestibulum. Nullam sit amet rutrum\n"
        "         * velit, nec dictum enim. Proin commodo sollicitudin sollicitudin. Integer ultrices nisl turpis,\n"
        "         * ut vehicula nunc ultricies ac. Ut ultrices blandit est et euismod. Donec eget placerat nisi,\n"
        "         * ut varius nibh. Nulla ut velit quis dui suscipit tempus. Praesent eleifend luctus augue, id\n"
        "         * volutpat lorem aliquet vel.\n"
        "         *\n"
        "         * Phasellus egestas auctor massa eget viverra. Nulla consectetur elementum nulla et condimentum.\n"
        "         * Curabitur eget maximus mauris, vel aliquet nisi. Pellentesque neque tortor, eleifend ac viverra\n"
        "         * vitae, faucibus sit amet risus. Fusce et feugiat justo. Nam iaculis vulputate mi non interdum.\n"
        "         * Vivamus sed nulla tellus. Suspendisse neque odio, pharetra eget suscipit nec, placerat at elit.\n"
        "         * Maecenas lobortis erat ac tortor sodales, et cursus est venenatis. Vivamus sed orci ac diam\n"
        "         * convallis accumsan. Mauris pretium leo eu lorem consequat dapibus. Duis quam ante, pretium vel\n"
        "         * erat ut, porta commodo nibh. Sed accumsan neque mi, eu scelerisque nulla ornare in. Sed pretium\n"
        "         * augue tortor, ut auctor arcu gravida eget. Aliquam urna nunc, ultrices eget interdum sed,\n"
        "         * venenatis feugiat magna.\n"
        "         "
    };
    expect_comment(node->leading_comments, 0, expected_comment);

    btc_parser_destroy(parser);
    btc_tokenizer_destroy(t);
}