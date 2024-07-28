SRCDIR   = ./src
BUILDDIR = ./build
TARGET   = main

SYSCONF_LINK = gcc
LIBS         = -lm -L${HOME}/.local/lib -lbench -Wl,-rpath,${HOME}/.local/lib
INCLUDES     = -I$(SRCDIR)/include -I${HOME}/.local/include

CPPFLAGS     = -Wall -Wextra -Wshadow -g -fsanitize=address 
LDFLAGS      = -Wall -fsanitize=address 

ifeq ($(MODE),fast)
	CPPFLAGS     := -Wall -Wextra -Wshadow -O3
	LDFLAGS      := -Wall 
else ifeq ($(MODE),bench)
	CPPFLAGS     := -Wall -Wextra -Wshadow -O3 -DBENCHMARK
	LDFLAGS      := -Wall 
endif 

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

