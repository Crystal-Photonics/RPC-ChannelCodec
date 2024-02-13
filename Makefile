#Set this to @ to keep the makefile quiet


#UNIXTOOLS = "C:\Program Files (x86)\Git\bin\"
#UNIXTOOLS = "C:/Progra~1/Git/usr/bin/"
RUNTEST=false
RUNPROD=true

ifeq ($(MAKECMDGOALS),start)
	RUNTEST=true
	RUNPROD=false
endif

ifeq ($(MAKECMDGOALS),test)
	RUNTEST=true
	RUNPROD=false
endif

ifeq ($(MAKECMDGOALS),clean)
	RUNTEST=true
	RUNPROD=false
endif

ifeq ($(RUNTEST),true)
	SILENCE = @

	TEST_SRC_DIRS = tests
	TEST_SRC_DIRS += tests/channel_codec
		

	SRC_DIRS += src/channel_codec
	SRC_DIRS += tests/rpc/rpc/client
	SRC_DIRS += tests/rpc/rpc/server
	
	
	INCLUDE_DIRS = include/
	INCLUDE_DIRS += $(CPPUTEST_HOME)/include/
	INCLUDE_DIRS += tests/
	INCLUDE_DIRS += tests/include
	INCLUDE_DIRS += include/errorlogger_dummy
	
		
	LD_LIBRARIES = -L$(CPPUTEST_HOME)/lib -lCppUTest  -lCppUTestExt -lpthread.dll
	
	ifndef CPPUTEST_HOME
		CPPUTEST_HOME=C:\Users\ark\entwicklung\eclipse_workspace\cpputest-3.6
	endif

	CPPUTEST_OBJS_DIR=buildtest/obj
	CPPUTEST_LIB_DIR=buildtest/lib
	#---- Outputs ----#
	COMPONENT_NAME = channel_codec
	#--- Inputs ----#
	PROJECT_HOME_DIR = .

	CPP_PLATFORM = Gcc
	CPPUTEST_EXE_FLAGS =
	CPPUTEST_WARNINGFLAGS += -Wall 
	CPPUTEST_WARNINGFLAGS += -Werror 
	CPPUTEST_WARNINGFLAGS += -Wswitch-default 
	CPPUTEST_WARNINGFLAGS += -Wswitch-enum  -Wno-unknown-pragmas -Wno-implicit-fallthrough 
	CPPUTEST_CFLAGS += -std=c99
	CPPUTEST_CFLAGS += -Wextra 
	#CPPUTEST_CFLAGS += -pedantic 
	#CPPUTEST_CFLAGS += -Wstrict-prototypes




 

	ifeq ($(CPPUTEST_HOME),)
		$(info CPPUTEST_HOME not set! See README.txt)
	else  
		include $(CPPUTEST_HOME)/build/MakefileWorker.mk
	endif
endif
	
ifeq ($(RUNPROD),true)

endif


