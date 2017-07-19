#ifndef __C_JSON_H_
#define __C_JSON_H_

#include "all.h"

/*
http://twobit.blog.chinaunix.net/uid-23412956-id-3784975.html

*/


typedef struct _key_obj{
    char *key;
    struct json_object *obj;
    struct json_object *parent_obj;

}key_obj;



#define __json_object_object_foreach(obj,key,val) \
 char *key; struct json_object *val; struct lh_entry *entry; \
 for(entry = json_object_get_object(obj)->head; (entry ? (key = (char*)entry->k, val = (struct json_object*)entry->v, entry) : 0); entry = entry->next)

extern void printf_t(int i);
extern void printf_json(const char *json_str);
extern void write_t_to_flie(FILE *fp, int i);
extern int save_json_to_file(const char *json_str, char *path);
extern void __str(char *dest, char *src);

extern int __json_tokener_parse(char *str, struct json_object *new_obj);
extern int __json_key_to_string(struct json_object *new_obj, char *key, char *str, int len);
//extern int __json_key_to_int(struct json_object *new_obj, char *key, int *val);
extern int __json_key_to_int(struct json_object *new_obj, char *key);


extern int get_top_node(const char *str, char *top_node_buf, int buf_len);
extern int json_get_obj(struct json_object *obj, const char *str, key_obj* output_obj);
extern int json_set_value(struct json_object *root_obj, const char *str, const char *val);
extern const char * json_get_value(struct json_object *root_obj, const char *str);



#endif


