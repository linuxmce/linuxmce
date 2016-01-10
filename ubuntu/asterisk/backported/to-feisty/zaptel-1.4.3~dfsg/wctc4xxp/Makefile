ifneq ($(KBUILD_EXTMOD),)

include $(obj)/Makefile.kernel26

endif

tests: codec_test

codec_test: codec_test.c ../zaptel.h
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm -rf codec_test
