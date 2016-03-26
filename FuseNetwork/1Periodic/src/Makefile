IDIR =../include
CC=gcc
CFLAGS=-I../include -I/usr/include/suitesparse -I/c/ashivni/local/include -L/c/ashivni/local/lib -L/usr/lib64
LIBS=-lm -lcholmod -lrngstreams -lcolamd -lccolamd -lamd -lcamd -lbtf -lcxsparse -lumfpack -lklu -lldl -lblas -llapack
ODIR=obj

_DEPS = dia1P.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = dia1P.o dia1P_isFatal.o dia1P_errHandler.o dia1P_initialize.o dia1P_printPD.o dia1P_stiffnessMatrix.o dia1P_strengthVector.o dia1P_loadVector.o dia1P_bondToSnap.o dia1P_initializeBrokenBonds.o dia1P_updateBrokenBonds.o dia1P_isSnapped.o dia1P_stiffnessUpdateVector.o dia1P_loadUpdateVector.o dia1P_isBroken.o dia1P_freeBrokenBonds.o dia1P_finish.o dia1P_voltageToCurrentMatrix.o dia1P_updateVoltageToCurrentMatrix.o dia1P_getBondNumber.o dia1P_writeAnim.o trim.o dia1P_getNodeNumbers.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

dia1P: $(OBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LIBS)

_testOBJ = dia1P_testCases.o dia1P_getBondNumber.o dia1P_getNodeNumbers.o dia1P_errHandler.o dia1P_isFatal.o
testOBJ = $(patsubst %,$(ODIR)/%,$(_testOBJ))

testCase: $(testOBJ)
	$(CC) -o  $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ *.dat *.stat *.log *.ava *.gpi *.png *.gif nohup.out
