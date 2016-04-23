ROOT = ..
CXXFLAGS += -Wall -g 
CPPFLAGS += -I$(ROOT)/ -I$(ROOT)/DCE -I$(ROOT)/VIPShared -D_REENTRANT -D_GNU_SOURCE -D_DEVEL_X_DEFINES -DGENERATOR
LDFLAGS += -L$(ROOT)/lib 
LDLIBS += -lRACommon -lDCECommon -lPlutoUtils -lSerializeClass -lpthread -lattr
CXX ?= g++

MYSQL_FLAGS := $(shell pkg-config --cflags mysqlclient 2>/dev/null)
MYSQL_LIBS := $(shell pkg-config --libs mysqlclient 2>/dev/null)
ifneq (($(MYSQL_FLAGS)),)
	CPPFLAGS += $(MYSQL_FLAGS)
	LDLIBS += $(MYSQL_LIBS)
else
	CPPFLAGS += -I/usr/include/mysql
	LDLIBS += -lmysqlclient_r
endif

sources = A_UpdateRow.cpp ChangedRow.cpp CommonFunctions.cpp Database.cpp Field.cpp ../sql2cpp/FieldInfo.cpp \
	R_CommitChanges.cpp R_CommitRow.cpp R_CommitTable.cpp R_GetAll_psc_id.cpp R_UpdateRepository.cpp \
	R_UpdateTable.cpp sqlCVSrequests.cpp Table.cpp Repository.cpp ValidateUser.cpp sqlCVSprocessor.cpp \
	R_CloseTransaction.cpp R_ApproveBatch.cpp A_UpdateSchema.cpp R_GetRow.cpp R_GetHashedTableStats.cpp \
	R_GetConditional_psc_id.cpp
sqlCVS = $(sources) sqlCVS.cpp
new_thing = $(sources) TableInfo_SCVS.cpp

all: sqlCVS
#all: newthing

sqlCVS: $(sqlCVS:.cpp=.o)
	$(CXX) -o $@ $(call cpath,$+) $(LDFLAGS) $(LDLIBS)
	cp $@ $(ROOT)/bin

newthing: $(new_thing:.cpp=.o)
	@echo "*** Something to do here ***"

distclean: clean

clean: clean-recursive
	rm -f $(call cpath,$(sqlCVS:.cpp=.o) $(sqlCVS:.cpp=.d)) sqlCVS >/dev/null

include $(ROOT)/PlutoUtils/make.rules
-include $(sources:.cpp=.d)

install: 
	mkdir -p /usr/pluto/bin
	cp sqlCVS $(DESTPATH)/usr/pluto/bin
	cp sqlcvs_db_fixer.php $(DESTPATH)/usr/pluto/bin
	cp sqlcvs_db_fixer.sh $(DESTPATH)/usr/pluto/bin
