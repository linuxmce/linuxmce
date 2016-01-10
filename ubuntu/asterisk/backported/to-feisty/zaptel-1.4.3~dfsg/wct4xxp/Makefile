ifneq ($(KBUILD_EXTMOD),)

include $(obj)/Makefile.kernel26

else

OCTASIC_OBJS:=$(shell ../oct612x/octasic-helper objects ../oct612x)
OCTASIC_CFLAGS:=$(shell ../oct612x/octasic-helper cflags ../oct612x) -Wno-undef

all: wct4xxp.o

%.o: %.c
	$(CC) $(KFLAGS) $(OCTASIC_CFLAGS) -o $@ -c $<

base.o: ../zaptel.h vpm450m.h wct4xxp.h

vpm450m.o: vpm450m.h ../oct612x/include/oct6100api/oct6100_api.h

../firmware/zaptel-fw-oct6114-064.o: base.o
	$(MAKE) -C ../firmware zaptel-fw-oct6114-064.o

../firmware/zaptel-fw-oct6114-128.o: base.o
	$(MAKE) -C ../firmware zaptel-fw-oct6114-128.o

wct4xxp.o: base.o vpm450m.o $(OCTASIC_OBJS) ../firmware/zaptel-fw-oct6114-064.o ../firmware/zaptel-fw-oct6114-128.o
	$(LD) -r -o $@ $^

clean:
	rm -f *.o
	rm -f $(OCTASIC_OBJS)

endif
