# Cross compile support - create a Makefile which defines these three variables and then includes this Makefile...
CFLAGS  ?= -Wall -fPIC -O2
LDADD   ?= -lasound -lpthread -lm -lrt
EXECUTABLE ?= squeezelite

# passing one or more of these in $(OPTS) enables optional feature inclusion
OPT_DSD     = -DDSD
OPT_FF      = -DFFMPEG
OPT_LINKALL = -DLINKALL
OPT_RESAMPLE= -DRESAMPLE
OPT_VIS     = -DVISEXPORT

SOURCES = \
	main.c slimproto.c buffer.c stream.c utils.c \
	output.c output_alsa.c output_pa.c output_stdout.c output_pack.c decode.c \
	flac.c pcm.c mad.c vorbis.c faad.c mpg.c

SOURCES_DSD      = dsd.c dop.c dsd2pcm/dsd2pcm.c
SOURCES_FF       = ffmpeg.c
SOURCES_RESAMPLE = process.c resample.c
SOURCES_VIS      = output_vis.c

LINK_LINUX       = -ldl

LINKALL          = -lFLAC -lmad -lvorbisfile -lfaad -lmpg123
LINKALL_FF       = -lavcodec -lavformat -lavutil
LINKALL_RESAMPLE = -lsoxr

DEPS             = squeezelite.h slimproto.h

UNAME            = $(shell uname -s)

# add optional sources
ifneq (,$(findstring $(OPT_DSD), $(OPTS)))
	SOURCES += $(SOURCES_DSD)
endif
ifneq (,$(findstring $(OPT_FF), $(OPTS)))
	SOURCES += $(SOURCES_FF)
endif
ifneq (,$(findstring $(OPT_RESAMPLE), $(OPTS)))
	SOURCES += $(SOURCES_RESAMPLE)
endif
ifneq (,$(findstring $(OPT_VIS), $(OPTS)))
	SOURCES += $(SOURCES_VIS)
endif

# add optional link options
ifneq (,$(findstring $(OPT_LINKALL), $(OPTS)))
	LDADD += $(LINKALL)
ifneq (,$(findstring $(OPT_FF), $(OPTS)))
	LDADD += $(LINKALL_FF)
endif
ifneq (,$(findstring $(OPT_RESAMPLE), $(OPTS)))
	LDADD += $(LINKALL_RESAMPLE)
endif
else
# if not LINKALL and linux add LINK_LINUX
ifeq ($(UNAME), Linux)
	LDADD += $(LINK_LINUX)
endif
endif

OBJECTS = $(SOURCES:.c=.o)

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LDADD) -o $@

$(OBJECTS): $(DEPS)

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(OPTS) $< -c -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
