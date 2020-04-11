#include "comments_list.h"
#include "comment.h"

#include <stdlib.h>

CVECTOR_NEW(btc_comments_list, btc_comment*)

void btc_comments_list_free_items(btc_comments_list* list) {
    vector_foreach(list, i) {
        btc_comment_free(btc_comments_list_get(list, i));
    }
}
