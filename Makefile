SRCDIR   = ./src
BUILDDIR = ./build
TARGET   = main

SYSCONF_LINK = gcc
CPPFLAGS     = -Wall -Wextra -g
LDFLAGS      = -Wall -fsanitize=address 
LIBS         = -lm
INCLUDES     = -I$(SRCDIR)/include


SRCS := $(wildcard $(SRCDIR)/*.c)
OBJECTS := $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(SRCS))

all: $(BUILDDIR)/$(TARGET)

$(BUILDDIR)/$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) $(LDFLAGS) $(INCLUDES) -o $(BUILDDIR)/$(TARGET) $(OBJECTS) $(LIBS)

$(OBJECTS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	$(SYSCONF_LINK) $(CPPFLAGS) $(INCLUDES) -c $(CFLAGS) $< -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga

