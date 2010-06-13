#include <stdlib.h>
#include <string.h>
#include "gc.h"

void cnl_gc_debug_print(CNL_GC *gc){
	int i;
	for(i = 0; i < gc->size; i++){
		if(gc->buf[i]){
			if((gc->buf[i]->type & 0x10) == 0x10){
				putchar('X');
			}else{
				putchar('E');
			}
		}else{
			putchar('N');
		}
	}
	putchar('\n');
}

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
	cnl_gc_debug_print(gc);
	int i;
	for(i = 0; i < gc->size; i++){
		if(gc->buf[i]){
			gc->buf[i]->type |= 0x10;
		}
	}
	int size = CNL_GC_BUFSIZ;
	int count = 0;
	CNL_OBJ **buf = (struct _CNL_OBJ **)malloc(sizeof(struct _CNL_OBJ *) * size);
	CNL_OBJ *obj = env;
	while(! CNL_NIL_P(obj)){
		if((obj->type & 0x10) == 0x10){
			obj->type &= 0xEF;
			if(CNL_PAIR_P(obj) || CNL_PROC_P(obj)){
				CNL_OBJ *car = CNL_CAR(obj);
				CNL_OBJ *cdr = CNL_CDR(obj);
				if(CNL_NIL_P(car)){
					if(CNL_NIL_P(cdr)){
						obj = count ? buf[--count] : CNL_NIL;
					}else{
						obj = cdr;
					}
				}else{
					if(! CNL_NIL_P(cdr)){
						if(count < size){
							buf[count++] = cdr;
						}else{
							int newsize = size + CNL_GC_BUFSIZ;
							CNL_OBJ **newbuf = (struct _CNL_OBJ **)malloc(sizeof(struct _CNL_OBJ *) * newsize);
							memcpy(newbuf,buf,sizeof(struct _CNL_OBJ *) * size);
							free(buf);
							size = newsize;
							buf = newbuf;
							buf[count++] = cdr;
						}
					}
					obj = car;
				}
			}
		}else{
			obj = count ? buf[--count] : CNL_NIL;
		}
	}
	free(buf);
	for(i = 0; i < gc->size; i++){
		if(gc->buf[i]){
			if((gc->buf[i]->type & 0x10) == 0x10){
				switch(CNL_TYPE(gc->buf[i])){
				case CNL_TYPE_SYMBOL : free(gc->buf[i]->o.str); break;
				case CNL_TYPE_STRING : free(gc->buf[i]->o.str); break;
				}
			}
		}
	}
	cnl_gc_debug_print(gc);
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
