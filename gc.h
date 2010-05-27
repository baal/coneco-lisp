#ifndef _CONECO_LISP_GC_
#define _CONECO_LISP_GC_ 1

#include "coneco.h"

#define CNL_GC_BUFSIZ 1024

struct _CNL_OBJ* cnl_make_obj(CNL_GC *gc);
CNL_GC* cnl_make_gc();
void cnl_gc_sweep(CNL_GC *gc,struct _CNL_OBJ *env);
void cnl_free_gc(CNL_GC *gc);

#endif /* _CONECO_LISP_GC_ */
