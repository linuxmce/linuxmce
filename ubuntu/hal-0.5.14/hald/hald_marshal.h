
#ifndef __hald_marshal_MARSHAL_H__
#define __hald_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:OBJECT,STRING,STRING (hald_marshal.list:1) */
extern void hald_marshal_VOID__OBJECT_STRING_STRING (GClosure     *closure,
                                                     GValue       *return_value,
                                                     guint         n_param_values,
                                                     const GValue *param_values,
                                                     gpointer      invocation_hint,
                                                     gpointer      marshal_data);

/* VOID:STRING,STRING (hald_marshal.list:2) */
extern void hald_marshal_VOID__STRING_STRING (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* VOID:STRING,BOOL,BOOL (hald_marshal.list:3) */
extern void hald_marshal_VOID__STRING_BOOLEAN_BOOLEAN (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);
#define hald_marshal_VOID__STRING_BOOL_BOOL	hald_marshal_VOID__STRING_BOOLEAN_BOOLEAN

/* VOID:STRING,BOOL (hald_marshal.list:4) */
extern void hald_marshal_VOID__STRING_BOOLEAN (GClosure     *closure,
                                               GValue       *return_value,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint,
                                               gpointer      marshal_data);
#define hald_marshal_VOID__STRING_BOOL	hald_marshal_VOID__STRING_BOOLEAN

/* VOID:STRING (hald_marshal.list:5) */
#define hald_marshal_VOID__STRING	g_cclosure_marshal_VOID__STRING

/* VOID:OBJECT,BOOL (hald_marshal.list:6) */
extern void hald_marshal_VOID__OBJECT_BOOLEAN (GClosure     *closure,
                                               GValue       *return_value,
                                               guint         n_param_values,
                                               const GValue *param_values,
                                               gpointer      invocation_hint,
                                               gpointer      marshal_data);
#define hald_marshal_VOID__OBJECT_BOOL	hald_marshal_VOID__OBJECT_BOOLEAN

/* VOID:OBJECT,STRING,BOOL,BOOL (hald_marshal.list:7) */
extern void hald_marshal_VOID__OBJECT_STRING_BOOLEAN_BOOLEAN (GClosure     *closure,
                                                              GValue       *return_value,
                                                              guint         n_param_values,
                                                              const GValue *param_values,
                                                              gpointer      invocation_hint,
                                                              gpointer      marshal_data);
#define hald_marshal_VOID__OBJECT_STRING_BOOL_BOOL	hald_marshal_VOID__OBJECT_STRING_BOOLEAN_BOOLEAN

/* VOID:OBJECT,STRING (hald_marshal.list:8) */
extern void hald_marshal_VOID__OBJECT_STRING (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

/* VOID:VOID (hald_marshal.list:9) */
#define hald_marshal_VOID__VOID	g_cclosure_marshal_VOID__VOID

G_END_DECLS

#endif /* __hald_marshal_MARSHAL_H__ */

