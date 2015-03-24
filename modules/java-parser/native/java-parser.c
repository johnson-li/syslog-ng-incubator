/*
 * Copyright (c) 2015 BalaBit IT Ltd, Budapest, Hungary
 * Copyright (c) 2015 Vincent Bernat <Vincent.Bernat@exoscale.ch>
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

#include "java-parser.h"
#include "scratch-buffers.h"
#include <stdio.h>

#define KEY_BUFFER_LENGTH 1024

typedef struct _JavaParser
{
  LogParser super;
  goffset date_offset;
  gchar *date_format;
  gchar *date_tz;
	gchar *class_path;
	gchar *class_name;
  TimeZoneInfo *date_tz_info;
	JavaParserProxy *proxy;
} JavaParser;

void
java_parser_set_offset (LogParser *s, goffset offset)
{
  JavaParser *self = (JavaParser *)s;
  self->date_offset = offset;
}

void java_parser_set_format (LogParser *s, gchar *format)
{
	printf("format: %s\n", format);
  JavaParser *self = (JavaParser *)s;
  if (self->date_format)
    g_free (self->date_format);

  self->date_format = g_strdup (format);
	printf("done\n");
}

void java_parser_set_timezone (LogParser *s, gchar *tz)
{
	printf("timezone: %s\n", tz);
  JavaParser *self = (JavaParser *)s;
  if (self->date_tz)
    g_free (self->date_tz);

  self->date_tz = g_strdup (tz);
  if (self->date_tz_info)
    time_zone_info_free (self->date_tz_info);
  self->date_tz_info = time_zone_info_new (self->date_tz);
	printf("done\n");
}

void java_parser_set_class_path (LogParser *s, const gchar *class_path)
{
  printf("class_path: %s\n", class_path);
  JavaParser *self = (JavaParser *)s;
	if (self->class_path) 
		g_free (self->class_path);
	self->class_path = g_strdup(class_path);
	printf("done\n");
}

void java_parser_set_class_name (LogParser *s, const gchar *class_name)
{
  printf("class_name: %s\n", class_name);
  JavaParser *self = (JavaParser *)s;
	g_free(self->class_name);
	self->class_name = g_strdup(class_name);
	printf("done\n");
}

static gboolean
java_parser_init (LogPipe *parser)
{
  JavaParser *self = (JavaParser *)parser;
	printf("java parser init: %s, %s\n", self->class_path, self->class_name);
	GError *error = NULL;

  self->proxy = java_parser_proxy_new(self->class_name, self->class_path, self);
	if (!self->proxy) 
		return FALSE;
	if (!java_parser_proxy_init(self->proxy)) 
		return FALSE;

  //GlobalConfig *cfg = log_pipe_get_config (&self->super.super);

  return TRUE;
};

static gboolean
java_parser_process (LogParser *s,
                     LogMessage **pmsg,
                     const LogPathOptions *path_options,
                     const gchar *input,
                     gsize input_len)
{
	return java_parser_proxy_process (s, pmsg, path_options, input, input_len);
	printf("java parser process by c\n");
  const gchar *src = input;
  char *cloned_input;
  char *remaining;
  JavaParser *self = (JavaParser *)s;
  LogMessage *msg = log_msg_make_writable (pmsg, path_options);
  struct tm tm;
  memset(&tm, 0, sizeof(struct tm));

  if (self->date_offset > input_len) return FALSE;
  input += self->date_offset;
  input_len -= self->date_offset;

  /* Parse date */
  cloned_input = g_strndup(input, input_len);
  if (!cloned_input) return FALSE;
  remaining = strptime(cloned_input, self->date_format, &tm);
  g_free(cloned_input);

  if (remaining == NULL) return FALSE;

  /* mktime handles timezones horribly. It considers the time to be
     local and also alter the parsed timezone. Try to fix all that. */
  msg->timestamps[LM_TS_STAMP].tv_usec = 0;
  if (!self->date_tz_info)
    {
      msg->timestamps[LM_TS_STAMP].zone_offset = tm.tm_gmtoff;
      msg->timestamps[LM_TS_STAMP].tv_sec = mktime (&tm) - msg->timestamps[LM_TS_STAMP].zone_offset - timezone;
    }
  else
    {
      msg->timestamps[LM_TS_STAMP].tv_sec = mktime (&tm) - timezone;
      msg->timestamps[LM_TS_STAMP].zone_offset =
        time_zone_info_get_offset(self->date_tz_info,
                                  msg->timestamps[LM_TS_STAMP].tv_sec);
      if (msg->timestamps[LM_TS_STAMP].zone_offset == -1)
        {
          msg->timestamps[LM_TS_STAMP].zone_offset =
            get_local_timezone_ofs(msg->timestamps[LM_TS_STAMP].tv_sec);
        }
      msg->timestamps[LM_TS_STAMP].tv_sec -= msg->timestamps[LM_TS_STAMP].zone_offset;
    }

  return TRUE;
};

static LogPipe *
java_parser_clone (LogPipe *s)
{
	printf("java parser clone\n");
  JavaParser *self = (JavaParser *) s;

  JavaParser *cloned = (JavaParser *) java_parser_new (log_pipe_get_config (&self->super.super));
  g_free (cloned->date_format);
  g_free (self->date_tz);
  if (self->date_tz_info)
    time_zone_info_free (self->date_tz_info);
  cloned->date_offset = self->date_offset;
  cloned->date_format = g_strdup (self->date_format);
  cloned->date_tz = g_strdup (self->date_tz);
  cloned->date_tz_info = time_zone_info_new (cloned->date_tz);
  cloned->super.template = log_template_ref (self->super.template);

  return &cloned->super.super;
};

static void
java_parser_free (LogPipe *s)
{
	printf("java parser free\n");
  JavaParser *self = (JavaParser *)s;

  g_free (self->date_format);
  g_free (self->date_tz);
  g_free (self->class_path);
  g_free (self->class_name);
  if (self->date_tz_info)
    time_zone_info_free (self->date_tz_info);

  log_parser_free_method (s);
};

LogParser *java_parser_new (GlobalConfig *cfg)
{
	printf("java parser new\n");
  JavaParser *self = g_new0 (JavaParser, 1);
  log_parser_init_instance (&self->super, cfg);
  self->super.super.init = java_parser_init;
  self->super.process = java_parser_process;
  self->super.super.clone = java_parser_clone;
  self->super.super.free_fn = java_parser_free;

  self->date_format = g_strdup ("%FT%T%z");
  self->class_path = g_strdup ("/usr/local/lib/syslog-ng/SyslogNg.jar");
  self->class_name = g_strdup ("org.syslog_ng.LogParser");

  return (LogParser *)self;
};

