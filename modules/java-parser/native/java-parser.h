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

#ifndef SNG_TRIGGER_SOURCE_H_INCLUDED
#define SNG_TRIGGER_SOURCE_H_INCLUDED

#include <jni.h>
#include <iv.h>
#include <iv_event.h>
#include "driver.h"
#include "mainloop.h"
#include "mainloop-io-worker.h"
#include "java_machine.h"
#include "parser/parser-expr.h"
#include "proxies/java-parser-proxy.h"

typedef struct 
{
	JavaParserProxy *proxy;
	GString *class_path;
	gchar *class_name;
} JavaParserDriver;

LogParser *java_parser_new(GlobalConfig *cfg);

void java_parser_set_offset (LogParser *s, goffset offset);
void java_parser_set_format (LogParser *s, gchar *format);
void java_parser_set_timezone (LogParser *s, gchar *tz);

static gboolean java_parser_init (LogPipe *parser);
static gboolean java_parser_process (LogParser *s, 
		LogMessage **pmsg, const LogPathOptions *path_options,
		const gchar *input, gsize input_len);
static LogPipe *java_parser_clone (LogPipe *s);
static void java_parser_free (LogPipe *s);
LogParser *java_parser_new (GlobalConfig *cfg);
#endif
