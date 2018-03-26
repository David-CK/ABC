CC = gcc 
LDFLAGS = -lpthread -ldl -D_REENTRANT

LIB_PATH = /usr/local/lib
INC_PATH = /usr/include/libnl3
 
CFLAGS += -DCW_NO_DTLS -DCW_NO_DTLSCWParseConfigurationUpdateRequest

CFLAGS += -DCW_DEBUGGING 
CFLAGS += -DOPENSSL_NO_KRB5

RM = /bin/rm -f 

AC_OBJS = AC.o ACConfigFile.o ACMainLoop.o ACDiscoveryState.o ACJoinState.o \
	ACConfigureState.o ACDataCheckState.o ACRunState.o ACProtocol_User.o \
	CWCommon.o CWList.o \
	CWLog.o ACMultiHomedSocket.o ACProtocol.o CWNetwork.o CWProtocol.o \
	CWStevens.o CWThread.o CWBinding.o\
	ACBinding.o ACInterface.o

WTP_OBJS = WTP.o WTPFrameReceive.o WTPFreqStatsReceive.o WTPStatsReceive.o WTPConfigFile.o WTPProtocol.o WTPProtocol_User.o \
	WTPDiscoveryState.o WTPJoinState.o WTPConfigureState.o WTPDataCheckState.o WTPRunState.o WTPRunStateCheck.o \
	WTPRetransmission.o WTPSulkingState.o CWCommon.o CWList.o CWNetwork.o \
	CWProtocol.o CWStevens.o CWThread.o CWBinding.o WTPBinding.o \
	WTPDriverInteraction.o \
	WTPRadio.o CWTunnel.o 
	
AC_SRCS = $(AC_OBJS:.o=.c) 
WTP_SRCS = $(WTP_OBJS:.o=.c)
WUA_SRCS = $(WUA:OBJS:.o=.c)

AC_DEPS := $(AC_OBJS:.o=.d)
WTP_DEPS := $(WTP_OBJS:.o=.d)
WUA_DEPS := $(WUA_OBJS:.o=.d)

# program executables. 
AC_NAME = AC 
WTP_NAME = WTP 

.PHONY: deps clean clean_libs libs

# top-level rule, to compile everything. 
all: $(AC_NAME) $(WTP_NAME) $(WUA_NAME)

$(AC_NAME): $(AC_OBJS) 
	$(CC) $(AC_OBJS) $(CC_FLAGS) $(OPENSSL_INCLUDE) $(LDFLAGS) -o $(AC_NAME) 
#-DSOFTMAC
$(WTP_NAME): $(WTP_OBJS) 
	$(CC) -DWRITE_STD_OUTPUT  $(WTP_OBJS) $(CC_FLAGS) -L$(LIB_PATH)  $(LDFLAGS) -o $(WTP_NAME) 

$(WUA_NAME): $(WUA_OBJS) 
	$(CC) $(WUA_OBJS) $(CC_FLAGS)  $(LDFLAGS) -o $(WUA_NAME) 

clean: 
	$(RM) $(AC_NAME) $(WTP_NAME) $(WUA_NAME) $(AC_OBJS) $(WTP_OBJS) $(WUA_OBJS) $(AC_DEPS) $(WTP_DEPS)

clean_deps:
	$(AC_DEPS) $(WTP_DEPS)
	
deps: $(AC_SRC) $(WTP_SRC)
	$(CC) -MD -E $(AC_SRCS) $(CFLAGS) >/dev/null
	$(CC) -MD -E -DWRITE_STD_OUTPUT $(WTP_SRCS) $(CFLAGS) >/dev/null

-include $(AC_DEPS)
-include $(WTP_DEPS)
