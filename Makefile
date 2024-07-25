SYSCONF_LINK = gcc
CPPFLAGS     = -Wall -Wextra
LDFLAGS      = -Wall
LIBS         = -lm

SRCDIR   = ./src
BUILDDIR = ./build
TARGET   = main

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

all: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(SYSCONF_LINK) $(CPPFLAGS) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga

