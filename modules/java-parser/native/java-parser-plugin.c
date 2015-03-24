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

#include <jni.h>

#include "java-parser-parser.h"
#include "plugin.h"
#include "driver.h"
#include "java-parser.h"
#include "plugin-types.h"
#include <stdio.h>

extern CfgParser java_parser;

static Plugin java_plugin =
{
  .type = LL_CONTEXT_PARSER,
  .name = "java-parser",
  .parser = &java_parser,
};

gboolean
java_parser_module_init(GlobalConfig *cfg, CfgArgs *args G_GNUC_UNUSED)
{
	printf("register java parser\n");
  plugin_register(cfg, &java_plugin, 1);
  return TRUE;
}

const ModuleInfo module_info =
{
  .canonical_name = "java-parser",
  .version = VERSION,
  .description = "Experimental java parser.",
  .core_revision = VERSION_CURRENT_VER_ONLY,
  .plugins = &java_plugin,
  .plugins_len = 1,
};
