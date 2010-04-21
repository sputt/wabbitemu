///////////////////////////////////////////////////
#include "wbded.h"
///////////////////////////////////////////////
// MEMORY HANDLING (GLIB) gmem.c
///////////////////////////////////////////////

static gpointer
standard_malloc (gsize n_bytes)
{
  return malloc (n_bytes);
}

static gpointer
standard_realloc (gpointer mem,
		  gsize    n_bytes)
{
  return realloc (mem, n_bytes);
}

static void
standard_free (gpointer mem)
{
  free (mem);
}

static gpointer
standard_calloc (gsize n_blocks,
		 gsize n_bytes)
{
  return calloc (n_blocks, n_bytes);
}

#define	standard_try_malloc		standard_malloc
#define	standard_try_realloc	standard_realloc

struct _GMemVTable
{
  gpointer (*malloc)      (gsize    n_bytes);
  gpointer (*realloc)     (gpointer mem,
			   gsize    n_bytes);
  void     (*free)        (gpointer mem);
  /* optional; set to NULL if not used ! */
  gpointer (*calloc)      (gsize    n_blocks,
			   gsize    n_block_bytes);
  gpointer (*try_malloc)  (gsize    n_bytes);
  gpointer (*try_realloc) (gpointer mem,
			   gsize    n_bytes);
};

static GMemVTable glib_mem_vtable = {
  standard_malloc,
  standard_realloc,
  standard_free,
  standard_calloc,
  standard_try_malloc,
  standard_try_realloc,
};

gpointer g_malloc0 (gulong n_bytes)
{
	gpointer mem;

	mem = glib_mem_vtable.calloc (1, n_bytes);
	if (mem)
		return mem;

  return NULL;
}

gpointer g_malloc (gulong n_bytes)
{
 	gpointer mem;

	mem = glib_mem_vtable.malloc (n_bytes);
      if (mem)
	return mem;

  return NULL;
}


void g_free (gpointer mem)
{
//  if (G_UNLIKELY (!g_mem_initialized))
//    g_mem_init_nomessage();
//  if (G_LIKELY (mem))
    glib_mem_vtable.free (mem);
}
