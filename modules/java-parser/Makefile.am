if ENABLE_DATE
module_LTLIBRARIES += modules/java-parser/libjava-parser.la

modules_java_parser_libjava_parser_la_CFLAGS	 = \
	$(INCUBATOR_CFLAGS)			   \
	-I$(top_srcdir)/modules/java-parser		   \
	-I$(top_builddir)/modules/java-parser

modules_java_parser_libjava_parser_la_SOURCES	 = \
	modules/java-parser/java-parser-grammar.y	   \
	modules/java-parser/java-parser.c		   \
	modules/java-parser/java-parser.h		   \
	modules/java-parser/java-parser-parser.c	   \
	modules/java-parser/java-parser-parser.h	   \
	modules/java-parser/java-parser-plugin.c

modules_java_parser_libjava_parser_la_LIBADD	 = \
	$(INCUBATOR_LIBS)

modules_java_parser_libjava_parser_la_LDFLAGS	 = \
	-avoid-version -module -no-undefined

BUILT_SOURCES						+= \
	modules/java-parser/java-parser-grammar.y	   \
	modules/java-parser/java-parser-grammar.c	   \
	modules/java-parser/java-parser-grammar.h

modules/java-parser mod-java-parser: modules/java-parser/libjava-parser.la
else
modules/java-parser mod-java-parser:
endif


EXTRA_DIST += \
	modules/java-parser/java-parser-grammar.ym

.PHONY: modules/java-parser mod-java-parser
