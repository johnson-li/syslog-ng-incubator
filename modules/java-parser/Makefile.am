export top_srcdir

AUTOMAKE_OPTIONS = subdir-objects

if ENABLE_JAVA

module_LTLIBRARIES += libjava-parser.la

libjava_parser_la_CFLAGS	 = \
	$(JNI_CFLAGS)\
	$(INCUBATOR_CFLAGS)			   \
	-I$(top_srcdir)/modules/java-parser		   \
	-I$(top_builddir)/modules/java-parser    \
	-I$(top_srcdir)/modules/java-parser/native		   \
	-I$(top_builddir)/modules/java-parser/native

libjava_parser_la_SOURCES	 = \
	modules/java-parser/native/java-parser-grammar.y	   \
	modules/java-parser/native/java-parser.c		   \
	modules/java-parser/native/java-parser.h		   \
	modules/java-parser/native/java-parser-parser.c	   \
	modules/java-parser/native/java-parser-parser.h	   \
	modules/java-parser/native/java-parser-plugin.c \
	modules/java-parser/native/java-class-loader.h \
	modules/java-parser/native/java-class-loader.c \
	modules/java-parser/native/java_machine.h \
	modules/java-parser/native/java_machine.c \
	modules/java-parser/proxies/java-parser-proxy.h \
	modules/java-parser/proxies/java-parser-proxy.c

libjava_parser_la_LIBADD	 = \
	$(JNI_LIBS) $(INCUBATOR_LIBS)

libjava_parser_la_LDFLAGS	 = \
	-avoid-version -module -no-undefined

libjava_parser_la_DEPENDENCIES = \
  $(MODULE_DEPS_LIBS)

JAVA_PKG_NAME = org.syslog_ng

JAVA_CLASS_DIR = modules/java-parser/org/syslog_ng

JAVA_CLASS_PATH = $(JAVA_CLASS_DIR)
JAVA_SRC_DIR = modules/java-parser

JAVA_CLASS_FILES = \
									 $(JAVA_CLASS_PATH)/InternalMessageSender.class \
									 $(JAVA_CLASS_PATH)/LogPipe.class \
									 $(JAVA_CLASS_PATH)/LogParser.class

JAVA_FILES = \
						 $(JAVA_SRC_DIR)/InternalMessageSender.java \
						 $(JAVA_SRC_DIR)/LogPipe.java \
						 $(JAVA_SRC_DIR)/LogParser.java

JAVA_HEADER_FILES = \
										modules/java-parser/LogParser.h \
										modules/java-parser/InternalMessageSender.h

modules/java-parser/SyslogNg.jar: $(JAVA_CLASS_FILES) $(JAVA_HEADER_FILES)
	$(AM_V_GEN) $(JAR) -cvf $@ -C ./ org

$(JAVA_HEADER_FILES): $(JAVA_CLASS_FILES)
	$(AM_V_GEN) $(JAVAH) -cp ./ -o $@ $(JAVA_PKG_NAME).$(notdir $(basename $@))

$(JAVA_CLASS_PATH)/%.class: $(top_srcdir)/modules/java-parser/%.java $(JAVA_FILES)
	$(AM_V_GEN) $(JAVAC) -d ./ -cp ./ $<

install-exec-local:
	$(mkinstalldirs) $(DESTDIR)$(moduledir)
	cp modules/java-parser/SyslogNg.jar $(DESTDIR)$(moduledir)/

uninstall-local:
	rm -rf $(DESTDIR)$(moduledir)/SyslogNg.jar

clean-local:
	rm -rf org
endif

BUILT_SOURCES						+= \
	modules/java-parser/native/java-parser-grammar.y	   \
	modules/java-parser/native/java-parser-grammar.c	   \
	modules/java-parser/native/java-parser-grammar.h

if ENABLE_JAVA
BUILT_SOURCES += $(JAVA_HEADER_FILES) \
								 modules/java-parser/SyslogNg.jar

CLEANFILES = $(JAVA_CLASS_FILES) \
						 $(JAVA_HEADER_FILES) \
						 modules/java-parser/SyslogNg.jar
endif

EXTRA_DIST += \
	modules/java-parser/native/java-parser-grammar.ym
	$(JAVA_FILES)

