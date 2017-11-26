/* popupmnu.c */
void pum_display(pumitem_T *array, int size, int selected);
void pum_redraw(void);
void pum_undisplay(void);
void pum_clear(void);
int pum_visible(void);
int pum_get_height(void);
int split_message(char_u *mesg, pumitem_T **array);
void ui_remove_balloon(void);
void ui_post_balloon(char_u *mesg, list_T *list);
void ui_may_remove_balloon(void);
/* vim: set ft=c : */
