#include "java-parser-proxy.h"
#include "java-logmsg-proxy.h"
#include "java-class-loader.h"
#include "java-logmsg-proxy.h"

typedef struct _JavaParserImpl
{
	jobject parser_object;
	jmethodID mi_init;
	jmethodID mi_deinit;
	jmethodID mi_constructor;
	jmethodID mi_process;
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

	self->parser_impl.mi_process = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "process", "(Ljava/lang/String;)Z");
	if (!self->parser_impl.mi_process) {
    msg_error("Can't find method in class",
				evt_tag_str("class_name", class_name),
				evt_tag_str("method", "boolean process(String)"), 
				NULL);
	}

  return TRUE;
}

JavaParserProxy *
java_parser_proxy_new(const gchar *class_name, const gchar *class_path, gpointer handle)
{
	JavaParserProxy *self = g_new0(JavaParserProxy, 1);
	self->java_machine = java_machine_ref();
	
	if (!java_machine_start(self->java_machine))
		goto error;

	if (!__load_parser_object(self, class_name, class_path, handle))
		goto error;
	return self;

error:
	java_parser_proxy_free(self);
	return NULL;
}

void 
java_parser_proxy_free(JavaParserProxy *self)
{
	JNIEnv *env = NULL;
	env = java_machine_get_env(self->java_machine, &env);
	if (self->parser_impl.parser_object)
		CALL_JAVA_FUNCTION(env, DeleteLocalRef, self->parser_impl.parser_object);
	if (self->loaded_class)
	  CALL_JAVA_FUNCTION(env, DeleteLocalRef, self->loaded_class);
	java_machine_unref(self->java_machine);
	g_free(self);
}

gboolean
java_parser_proxy_init(JavaParserProxy *self)
{
	jboolean result;
	JNIEnv *env = java_machine_get_env(self->java_machine, &env);

	result = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->parser_impl.parser_object, self->parser_impl.mi_init);

	return !!(result);
}

void 
java_parser_proxy_deinit(JavaParserProxy *self)
{
	JNIEnv *env = java_machine_get_env(self->java_machine, &env);

	jboolean res = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->parser_impl.parser_object, self->parser_impl.mi_deinit);

}

gboolean
java_parser_proxy_process(JavaParserProxy *self,  
		LogMessage **pmsg, const LogPathOptions *path_options,
		const gchar *input, gsize input_len) 
{
	JNIEnv *env = java_machine_get_env(self->java_machine, &env);
	JavaLogMessageProxy *jmsg = java_log_message_proxy_new(*pmsg);
  
	jboolean res = CALL_JAVA_FUNCTION(env, CallBooleanMethod, 
			self->parser_impl.parser_object, self->parser_impl.mi_process, 
			java_log_parser_get_java_object(jmsg));

	java_log_message_proxy_free(jmsg);
	return !!(res);
}
