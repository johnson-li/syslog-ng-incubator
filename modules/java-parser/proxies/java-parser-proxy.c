#include "java-parser-proxy.h"
#include "java-class-loader.h"

typedef struct _JavaParserImpl
{
	jobject parser_object;
	jmethodID mi_init;
	jmethodID mi_deinit;
	jmethodID mi_constructor;
} JavaParserImpl;

struct _JavaParserProxy
{
	JavaVMSingleton *java_machine;
	jclass loaded_class;
	JavaParserImpl parser_impl;
};

static gboolean
__load_parser_object(JavaParserProxy *self, const gchar *class_name, const gchar *class_path, gpointer handle)
{
	JNIEnv *java_env = NULL;
	java_env = java_machine_get_env(self->java_machine, &java_env);
	self->loaded_class = java_machine_load_class(self->java_machine, class_name, class_path);
	if (!self->loaded_class)
	{
		msg_error("Can't find class",
				evt_tag_str("class_name", class_name),
				NULL);
		return FALSE;
	}

	self->parser_impl.mi_constructor = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "<init>", "(J)V");
	if (!self->parser_impl.mi_constructor) {
		msg_error("Can't find default contructor for class",
				evt_tag_str("class_name", class_name), NULL);
		return FALSE;
	}

	self->parser_impl.mi_init = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "initProxy", "()Z");
	if (!self->parser_impl.mi_init) {
		msg_error("Can't find method in class",
				evt_tag_str("class_name", class_name),
				evt_tag_str("method", "boolean init(SyslogNg)"), NULL);
		return FALSE;
	}

  return TRUE;
}

gboolean
java_parser_proxy_init(JavaParserProxy *self)
{
	jboolean result;
	JNIEnv *env = java_machine_get_env(self->java_machine, &env);

	result = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->parser_impl.parser_object, self->parser_impl.mi_init);

	return result;
}

void 
java_parser_proxy_deinit(JavaParserProxy *self)
{
	JNIEnv *env = java_machine_get_env(self->java_machine, &env);

	jboolean res = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->parser_impl.parser_object, self->parser_impl.mi_deinit);

}

