#include <stdlib.h>
#include <string.h>
#include "gc.h"

struct _CNL_OBJ* cnl_make_obj(CNL_GC *gc){
	int i;
	for(i = 0; i < gc->size; i++){
		if(gc->buf[i]){
			CNL_OBJ *obj = gc->buf[i];
			if((obj->type & 0x10) == 0x10){
				return obj;
			}
		}else{
			CNL_OBJ *obj = (struct _CNL_OBJ *)malloc(sizeof(struct _CNL_OBJ));
			gc->buf[i] = obj;
			return obj;
		}
	}
	int oldsize = gc->size;
	int newsize = oldsize + CNL_GC_BUFSIZ;
	CNL_OBJ **newbuf = (struct _CNL_OBJ **)malloc(sizeof(struct _CNL_OBJ *) * newsize);
	memcpy(newbuf,gc->buf,sizeof(struct _CNL_OBJ *) * oldsize);
	for(i = oldsize; i < newsize; i++){
		newbuf[i] = NULL;
	}
	free(gc->buf);
	gc->size = newsize;
	gc->buf = newbuf;
	gc->buf[oldsize] = (struct _CNL_OBJ *)malloc(sizeof(struct _CNL_OBJ));
	return gc->buf[oldsize];
}

CNL_GC* cnl_make_gc(){
	int i;
	CNL_GC *gc = (CNL_GC*)malloc(sizeof(CNL_GC));
	gc->size = CNL_GC_BUFSIZ;
	gc->buf = (struct _CNL_OBJ **)malloc(sizeof(struct _CNL_OBJ *) * gc->size);
	for(i = 0; i < gc->size; i++){
		gc->buf[i] = NULL;
	}
	return gc;
}

void cnl_gc_sweep(CNL_GC *gc,struct _CNL_OBJ *env){
}

void cnl_free_gc(CNL_GC *gc){
	int i;
	for(i = 0; i < gc->size; i++){
		if(gc->buf[i]){
			CNL_OBJ *obj = gc->buf[i];
			switch(CNL_TYPE(obj)){
			case CNL_TYPE_SYMBOL : free(obj->o.str); break;
			case CNL_TYPE_STRING : free(obj->o.str); break;
			}
			free(obj);
			gc->buf[i] = NULL;
		}
	}
	free(gc->buf);
	free(gc);
	gc = NULL;
}
