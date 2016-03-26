IDIR =./
CC=gcc
LIBS=-lm 
ODIR=obj

_DEPS = pp.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_ObjDraw = dia1P_draw.o  dia1P_drawInitialSetup.o  dia1P_markBond.o  dia1P_markSquare.o dia1P_getData.o dia1P_isEndOfRun.o dia1P_isEndOfAva.o dia1P_initializeInputFile.o dia1P_compareDoubles.o

ObjDraw = $(patsubst %,$(ODIR)/%,$(_ObjDraw))

_ObjDrawSeq = dia1P_drawSeq.o  dia1P_drawInitialSetup.o  dia1P_markBond.o  dia1P_markSquare.o dia1P_getData.o dia1P_isEndOfRun.o dia1P_isEndOfAva.o dia1P_initializeInputFile.o dia1P_compareDoubles.o

ObjDrawSeq = $(patsubst %,$(ODIR)/%,$(_ObjDrawSeq))

_OBJDISORD = dia1P_drawDisorder.o  dia1P_drawInitialSetup.o  dia1P_markBond.o  dia1P_markSquare.o dia1P_getData.o dia1P_initializeInputFile.o dia1P_compareDoubles.o

OBJDISORD = $(patsubst %,$(ODIR)/%,$(_OBJDISORD))

_OBJASD = dia1P_avalancheSizeDist.o  dia1P_initializeInputFile.o dia1P_compareDoubles.o dia1P_getData.o dia1P_isEndOfAva.o dia1P_isEndOfRun.o 

OBJASD = $(patsubst %,$(ODIR)/%,$(_OBJASD))

_OBJASDDUP = dia1PDup_avalancheSizeDist.o  dia1P_initializeInputFile.o dia1P_compareDoubles.o dia1P_getData.o dia1P_isEndOfAva.o dia1P_isEndOfRun.o 

OBJASDDUP = $(patsubst %,$(ODIR)/%,$(_OBJASDDUP))

_OBJLSD = dia1P_largestAva.o  dia1P_initializeInputFile.o dia1P_compareDoubles.o dia1P_getData.o dia1P_isEndOfAva.o dia1P_isEndOfRun.o 

OBJLSD = $(patsubst %,$(ODIR)/%,$(_OBJLSD))

_OBJAVGDMG = dia1P_averageDamage.o  dia1P_initializeInputFile.o dia1P_compareDoubles.o dia1P_getData.o dia1P_isEndOfAva.o dia1P_isEndOfRun.o 

OBJAVGDMG = $(patsubst %,$(ODIR)/%,$(_OBJAVGDMG))

_OBJSTR = dia1P_survivalStress.o  dia1P_initializeInputFile.o dia1P_compareDoubles.o dia1P_getData.o dia1P_isEndOfAva.o dia1P_isEndOfRun.o 

OBJSTR = $(patsubst %,$(ODIR)/%,$(_OBJSTR))

_OBJSP = dia1P_survivalProb.o  dia1P_initializeInputFile.o dia1P_compareDoubles.o dia1P_getData.o dia1P_isEndOfAva.o dia1P_isEndOfRun.o 

OBJSP = $(patsubst %,$(ODIR)/%,$(_OBJSP))

_OBJLASTASD = dia1P_avalancheSizeDistLast.o  dia1P_initializeInputFile.o dia1P_compareDoubles.o dia1P_getData.o dia1P_isEndOfAva.o dia1P_isEndOfRun.o 

OBJLASTASD = $(patsubst %,$(ODIR)/%,$(_OBJLASTASD))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -g -o $@ $< $(CFLAGS)

draw: $(ObjDraw)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

drawSeq: $(ObjDrawSeq)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

drawDisOrd: $(OBJDISORD)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

asdLast: $(OBJLASTASD)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

asd: $(OBJASD)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

asddup: $(OBJASDDUP)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

lsd: $(OBJLSD)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

avgdmg: $(OBJAVGDMG)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

sp: $(OBJSP)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

str: $(OBJSTR)
	$(CC) -g -o  $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ *.dat *.stat *.log *.ava *.gpi *.png *.gif nohup.out
