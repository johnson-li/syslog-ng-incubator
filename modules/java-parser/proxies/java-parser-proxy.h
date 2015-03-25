#ifndef JAVA_DESTINATION_PROXY_H_
#define JAVA_DESTINATION_PROXY_H_ 

#include <jni.h>
#include <syslog-ng.h>
#include <java_machine.h>
#include <template/templates.h>

typedef struct _JavaParserProxy JavaParserProxy;

JavaParserProxy *java_parser_proxy_new(const gchar *class_name, const gchar *class_path, gpointer handle);

#endif
