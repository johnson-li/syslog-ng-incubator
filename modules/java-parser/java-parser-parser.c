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
#include "cfg-parser.h"
#include "java-parser-grammar.h"
#include "java-parser-parser.h"

extern int date_parser_debug;
int date_parse(CfgLexer *lexer, LogParser **instance, gpointer arg);

static CfgLexerKeyword date_keywords[] = {
  { "java_parser", KW_DATE },
  { "offset",      KW_DATE_OFFSET },
  { "format",      KW_DATE_FORMAT },
  { NULL }
};

CfgParser date_parser =
{
#if ENABLE_DEBUG
  .debug_flag = &date_parser_debug,
#endif
  .name = "java-parser",
  .keywords = date_keywords,
  .parse = (int (*)(CfgLexer *, gpointer *, gpointer)) date_parse,
  .cleanup = (void (*)(gpointer)) log_pipe_unref,
};

CFG_PARSER_IMPLEMENT_LEXER_BINDING(date_, LogParser **);
