#include <assert.h>

#include "misc/rstr.h"

#include "ecmascript.h"
#include "metadata/metadata.h"
#include "metadata/metadata_str.h"
#include "prop/prop.h"


typedef struct es_mlv {
  es_resource_t super;
  metadata_lazy_video_t *mlv;
} es_mlv_t;




/**
 *
 */
static void
es_mlv_destroy(es_resource_t *eres)
{
  es_mlv_t *em = (es_mlv_t *)eres;
  mlv_unbind(em->mlv, 0);
  printf("mlv destroyed\n");
}



/**
 *
 */
static const es_resource_class_t es_resource_mlv = {
  .erc_name = "mlv",
  .erc_size = sizeof(es_mlv_t),
  .erc_destroy = es_mlv_destroy,
};


/**
 *
 */
static int
es_video_metadata_bind_duk(duk_context *ctx)
{
  prop_t *root = es_stprop_get(ctx, 0);
  const char *urlstr = duk_safe_to_string(ctx, 1);
  es_context_t *ec = es_get(ctx);
  es_mlv_t *em = es_resource_create(ec, &es_resource_mlv);

  rstr_t *url = rstr_alloc(urlstr);
  rstr_t *title;
  rstr_t *filename = es_prop_to_rstr(ctx, 2, "filename");
  int year         = es_prop_to_int(ctx, 2, "year", 0);

  if(filename != NULL) {
    // Raw filename case
    title = metadata_remove_postfix_rstr(filename);
    rstr_release(filename);
    year = -1;
  } else {
    title = es_prop_to_rstr(ctx, 2, "title");
  }

  int season    = es_prop_to_int(ctx,  2, "season", -1);
  int episode   = es_prop_to_int(ctx,  2, "episode", -1);
  rstr_t *imdb  = es_prop_to_rstr(ctx, 2, "imdb");
  int duration  = es_prop_to_int(ctx,  2, "duration", 0);

  em->mlv =
    metadata_bind_video_info(url, title, imdb, duration, root, NULL, 0, 0,
                             year, season, episode, 0);
  rstr_release(title);
  rstr_release(url);
  printf("mlv created\n");
  es_resource_push(ctx, &em->super);
  return 1;
}


/**
 * Showtime object exposed functions
 */
const duk_function_list_entry fnlist_Showtime_metadata[] = {
  { "videoMetadataBind",     es_video_metadata_bind_duk,       3 },
  { NULL, NULL, 0}
};
