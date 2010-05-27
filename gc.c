#include <stdlib.h>
#include <string.h>
#include "gc.h"

struct _CNL_OBJ* cnl_make_obj(CNL_GC *gc){
	if(gc->index >= CNL_GC_BUFSIZ){
		gc->no++;
		gc->index = 0;
		if(gc->no >= gc->size){
			struct _CNL_OBJ** newbuf = (struct _CNL_OBJ **)malloc(sizeof(struct _CNL_OBJ *) * (gc->size + 1));
			if(gc->size > 0){
				memcpy(newbuf,gc->buf,sizeof(struct _CNL_OBJ *) * gc->size);
				free(gc->buf);
			}
			gc->buf = newbuf;
			*(gc->buf + gc->size) = (struct _CNL_OBJ *)malloc(sizeof(struct _CNL_OBJ) * CNL_GC_BUFSIZ);
			gc->size++;
		}
	}
	return *(gc->buf + gc->no) + gc->index++;
}

CNL_GC* cnl_make_gc(){
	CNL_GC *gc = (CNL_GC*)malloc(sizeof(CNL_GC));
	gc->no = 0;
	gc->index = 0;
	gc->size = 1;
	gc->buf = (struct _CNL_OBJ **)malloc(sizeof(struct _CNL_OBJ *));
	*gc->buf = (struct _CNL_OBJ *)malloc(sizeof(struct _CNL_OBJ) * CNL_GC_BUFSIZ);
	return gc;
}

void cnl_gc_sweep(CNL_GC *gc,struct _CNL_OBJ *env){
}

void cnl_free_gc(CNL_GC *gc){
	int i,j;
	for(i = 0; i < gc->size; i++){
		for(j = 0; j < gc->index; j++){
			CNL_OBJ *obj = *(gc->buf + i) + j;
			switch(obj->type){
			case CNL_TYPE_SYMBOL : free(obj->o.str); break;
			case CNL_TYPE_STRING : free(obj->o.str); break;
			}
		}
		free(*(gc->buf + i));
	}
	free(gc->buf);
	free(gc);
}
