#--- Environment check ---
#-------------------------

#--- Server environment ---
#--------------------------

 C_BASE=
 L_BASE=

TMP=tmp
OUT=out
TEST=test
WORK=/opt/smev

WARNINGS= -Wformat \
          -Wunused \
          -Wuninitialized \
          -Wshadow \
          -Wno-write-strings
OPTIMISE= -O0

DEFINES=-DUNIX -DLINUX $(OPTIMISE) $(WARNINGS) $(C_BASE)

#--- ABTP environment ---

ABTP_H=-I$(COMMON_LIB_PATH)/grabtp
ABTP_L=$(COMMON_LIB_PATH)/grabtp/grabtp.a

#--- SQL environment ---

#--- Project components ---
#--------------------------

default : init install

init : $(TMP) $(OUT)

$(TMP) :
	mkdir $(TMP)

$(OUT) :
	mkdir $(OUT)

all      : clean $(OUT)/smev

$(OUT)/smev : $(TMP)/smev_unix.o $(TMP)/smev_rkn_gov_ru.o
	g++ -o $(OUT)/smev $(TMP)/smev_unix.o $(TMP)/smev_rkn_gov_ru.o \
		$(ABTP_L)

$(TMP)/smev_unix.o : smev_unix.cpp
	g++ -c -o $(TMP)/smev_unix.o $(DEFINES) $(ABTP_H) smev_unix.cpp

$(TMP)/smev_rkn_gov_ru.o : SMEV_rkn_gov_ru.cpp
	g++ -c -o $(TMP)/smev_rkn_gov_ru.o $(DEFINES) $(ABTP_H) SMEV_rkn_gov_ru.cpp

#--- Cleaning ---
#----------------

clean:
	rm -f $(TMP)/*.* $(OUT)/*.*

#--- Copy to installing folder ---
#---------------------------------

install: init all
	cp $(OUT)/smev /opt/smev/
