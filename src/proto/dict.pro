/* dict.c */
dict_T *dict_alloc(void);
dict_T *dict_alloc_id(alloc_id_T id);
dict_T *dict_alloc_lock(int lock);
int rettv_dict_alloc(typval_T *rettv);
void rettv_dict_set(typval_T *rettv, dict_T *d);
void dict_free_contents(dict_T *d);
void hashtab_free_contents(hashtab_T *ht);
void dict_unref(dict_T *d);
int dict_free_nonref(int copyID);
void dict_free_items(int copyID);
dictitem_T *dictitem_alloc(char_u *key);
void dictitem_remove(dict_T *dict, dictitem_T *item);
void dictitem_free(dictitem_T *item);
dict_T *dict_copy(dict_T *orig, int deep, int copyID);
int dict_add(dict_T *d, dictitem_T *item);
int dict_add_number(dict_T *d, char *key, varnumber_T nr);
int dict_add_bool(dict_T *d, char *key, varnumber_T nr);
int dict_add_string(dict_T *d, char *key, char_u *str);
int dict_add_string_len(dict_T *d, char *key, char_u *str, int len);
int dict_add_list(dict_T *d, char *key, list_T *list);
int dict_add_tv(dict_T *d, char *key, typval_T *tv);
int dict_add_callback(dict_T *d, char *key, callback_T *cb);
void dict_iterate_start(typval_T *var, dict_iterator_T *iter);
char_u *dict_iterate_next(dict_iterator_T *iter, typval_T **tv_result);
int dict_add_dict(dict_T *d, char *key, dict_T *dict);
long dict_len(dict_T *d);
dictitem_T *dict_find(dict_T *d, char_u *key, int len);
int dict_get_tv(dict_T *d, char_u *key, typval_T *rettv);
char_u *dict_get_string(dict_T *d, char_u *key, int save);
varnumber_T dict_get_number(dict_T *d, char_u *key);
varnumber_T dict_get_number_def(dict_T *d, char_u *key, int def);
varnumber_T dict_get_number_check(dict_T *d, char_u *key);
char_u *dict2string(typval_T *tv, int copyID, int restore_copyID);
int eval_dict(char_u **arg, typval_T *rettv, evalarg_T *evalarg, int literal);
void dict_extend(dict_T *d1, dict_T *d2, char_u *action);
dictitem_T *dict_lookup(hashitem_T *hi);
int dict_equal(dict_T *d1, dict_T *d2, int ic, int recursive);
void f_items(typval_T *argvars, typval_T *rettv);
void f_keys(typval_T *argvars, typval_T *rettv);
void f_values(typval_T *argvars, typval_T *rettv);
void dict_set_items_ro(dict_T *di);
void f_has_key(typval_T *argvars, typval_T *rettv);
void dict_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg);
/* vim: set ft=c : */
