/*
 * Copyright (c) 2010-2014 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 2010-2014 Viktor Juhasz <viktor.juhasz@balabit.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */


#include "java-destination-proxy.h"
#include "java-logmsg-proxy.h"
#include "java-class-loader.h"
#include "messages.h"
#include "logpipe.h"
#include "logmsg.h"
#include <string.h>


typedef struct _JavaDestinationImpl
{
  jobject dest_object;
  jmethodID mi_constructor;
  jmethodID mi_init;
  jmethodID mi_deinit;
  jmethodID mi_send;
  jmethodID mi_send_msg;
  jmethodID mi_open;
  jmethodID mi_close;
  jmethodID mi_is_opened;
} JavaDestinationImpl;

struct _JavaDestinationProxy
{
  JavaVMSingleton *java_machine;
  jclass loaded_class;
  JavaDestinationImpl dest_impl;
  LogTemplate *template;
  GString *formatted_message;
};

static gboolean
__load_destination_object(JavaDestinationProxy *self, const gchar *class_name, const gchar *class_path, gpointer handle)
{
  JNIEnv *java_env = NULL;
  java_env = java_machine_get_env(self->java_machine, &java_env);
  self->loaded_class = java_machine_load_class(self->java_machine, class_name, class_path);
  if (!self->loaded_class) {
      msg_error("Can't find class",
                evt_tag_str("class_name", class_name),
                NULL);
      return FALSE;
  }

  self->dest_impl.mi_constructor = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "<init>", "(J)V");
  if (!self->dest_impl.mi_constructor) {
      msg_error("Can't find default constructor for class",
                evt_tag_str("class_name", class_name), NULL);
      return FALSE;
  }

  self->dest_impl.mi_init = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "initProxy", "()Z");
  if (!self->dest_impl.mi_init) {
      msg_error("Can't find method in class",
                evt_tag_str("class_name", class_name),
                evt_tag_str("method", "boolean init(SyslogNg)"), NULL);
      return FALSE;
  }

  self->dest_impl.mi_deinit = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "deinitProxy", "()V");
  if (!self->dest_impl.mi_deinit) {
      msg_error("Can't find method in class",
                evt_tag_str("class_name", class_name),
                evt_tag_str("method", "void deinit()"), NULL);
      return FALSE;
  }

  self->dest_impl.mi_send = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "sendProxy", "(Ljava/lang/String;)Z");
  self->dest_impl.mi_send_msg = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "sendProxy", "(Lorg/syslog_ng/LogMessage;)Z");

  if (!self->dest_impl.mi_send_msg && !self->dest_impl.mi_send)
    {
      msg_error("Can't find any queue method in class",
                evt_tag_str("class_name", class_name),
                evt_tag_str("method", "boolean send(String) or boolean send(LogMessage)"),
                NULL);
    }

  self->dest_impl.mi_open = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "openProxy", "()Z");
  if (!self->dest_impl.mi_open)
    {
      msg_error("Can't find method in class",
                evt_tag_str("class_name", class_name),
                evt_tag_str("method", "boolean open()"),
                NULL);
    }

  self->dest_impl.mi_close = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "closeProxy", "()V");
  if (!self->dest_impl.mi_close)
    {
      msg_error("Can't find method in class",
                evt_tag_str("class_name", class_name),
                evt_tag_str("method", "void close()"),
                NULL);
    }

  self->dest_impl.mi_is_opened = CALL_JAVA_FUNCTION(java_env, GetMethodID, self->loaded_class, "isOpenedProxy", "()Z");
  if (!self->dest_impl.mi_is_opened)
    {
      msg_error("Can't find method in class", evt_tag_str("class_name", class_name),
          evt_tag_str("method", "boolean isOpened()"), NULL);
    }

  self->dest_impl.dest_object = CALL_JAVA_FUNCTION(java_env, NewObject, self->loaded_class, self->dest_impl.mi_constructor, handle);
  if (!self->dest_impl.dest_object)
    {
      msg_error("Can't create object",
                evt_tag_str("class_name", class_name),
                NULL);
      return FALSE;
    }
  return TRUE;
}


void
java_destination_proxy_free(JavaDestinationProxy *self)
{
  JNIEnv *env = NULL;
  env = java_machine_get_env(self->java_machine, &env);
  if (self->dest_impl.dest_object)
    {
      CALL_JAVA_FUNCTION(env, DeleteLocalRef, self->dest_impl.dest_object);
    }

  if (self->loaded_class)
    {
      CALL_JAVA_FUNCTION(env, DeleteLocalRef, self->loaded_class);
    }
  java_machine_unref(self->java_machine);
  g_string_free(self->formatted_message, TRUE);
  log_template_unref(self->template);
  g_free(self);
}

JavaDestinationProxy *
java_destination_proxy_new(const gchar *class_name, const gchar *class_path, gpointer handle, LogTemplate *template)
{
  JavaDestinationProxy *self = g_new0(JavaDestinationProxy, 1);
  self->java_machine = java_machine_ref();
  self->formatted_message = g_string_sized_new(1024);
  self->template = log_template_ref(template);

  if (!java_machine_start(self->java_machine))
      goto error;

  if (!__load_destination_object(self, class_name, class_path, handle))
    {
      goto error;
    }

  return self;
error:
  java_destination_proxy_free(self);
  return NULL;
}

static gboolean
__queue_native_message(JavaDestinationProxy *self, JNIEnv *env, LogMessage *msg)
{
  JavaLogMessageProxy *jmsg = java_log_message_proxy_new(msg);
  if (!jmsg)
    {
      return FALSE;
    }

  jboolean res = CALL_JAVA_FUNCTION(env,
                                    CallBooleanMethod,
                                    self->dest_impl.dest_object,
                                    self->dest_impl.mi_send_msg,
                                    java_log_message_proxy_get_java_object(jmsg));

  java_log_message_proxy_free(jmsg);

  return !!(res);
}

static gboolean
__queue_formatted_message(JavaDestinationProxy *self, JNIEnv *env, LogMessage *msg)
{
  log_template_format(self->template, msg, NULL, LTZ_LOCAL, 0, NULL, self->formatted_message);
  jstring message = CALL_JAVA_FUNCTION(env, NewStringUTF, self->formatted_message->str);
  jboolean res = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->dest_impl.dest_object, self->dest_impl.mi_send, message);
  CALL_JAVA_FUNCTION(env, DeleteLocalRef, message);
  return !!(res);
}

gboolean
java_destination_proxy_send(JavaDestinationProxy *self, LogMessage *msg)
{
  JNIEnv *env = java_machine_get_env(self->java_machine, &env);
  gboolean result = FALSE;
  LogPathOptions path_options = LOG_PATH_OPTIONS_INIT;
  msg = log_msg_make_writable(&msg, &path_options);
  if (self->dest_impl.mi_send_msg != 0)
    {
      result =  __queue_native_message(self, env, msg);
    }
  else
    {
      result =  __queue_formatted_message(self, env, msg);
    }
  return result;
}

gboolean
java_destination_proxy_init(JavaDestinationProxy *self)
{
  jboolean result;
  JNIEnv *env = java_machine_get_env(self->java_machine, &env);

  result = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->dest_impl.dest_object, self->dest_impl.mi_init);

  return !!(result);
}

void
java_destination_proxy_deinit(JavaDestinationProxy *self)
{
  JNIEnv *env = java_machine_get_env(self->java_machine, &env);

  CALL_JAVA_FUNCTION(env, CallVoidMethod, self->dest_impl.dest_object, self->dest_impl.mi_deinit);

}

gboolean
java_destination_proxy_open(JavaDestinationProxy *self)
{
  JNIEnv *env = java_machine_get_env(self->java_machine, &env);

  jboolean res = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->dest_impl.dest_object, self->dest_impl.mi_open);

  return !!(res);
}

gboolean
java_destination_proxy_is_opened(JavaDestinationProxy *self)
{
  JNIEnv *env = java_machine_get_env(self->java_machine, &env);

  jboolean res = CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->dest_impl.dest_object, self->dest_impl.mi_is_opened);

  return !!(res);
}


void
java_destination_proxy_close(JavaDestinationProxy *self)
{
  JNIEnv *env = java_machine_get_env(self->java_machine, &env);

  CALL_JAVA_FUNCTION(env, CallBooleanMethod, self->dest_impl.dest_object, self->dest_impl.mi_close);

}
