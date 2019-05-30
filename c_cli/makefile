CC=gcc
CFLAGS=-std=c99 -Wall

ODIR=obj
HEADERS=defs.h core.h io.h


$(ODIR)/%.o: %.c $(HEADERS) $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

saper: $(ODIR)/main.o $(ODIR)/io.o $(ODIR)/core.o $(HEADERS)
	$(CC) $(CFLAGS) -o saper $(ODIR)/main.o $(ODIR)/core.o $(ODIR)/io.o

$(ODIR):
	mkdir $(ODIR)


clean:
	rm -r $(ODIR)

distclean: clean
	rm saper


##############################
#	$@ - before :
#	$< - first element after :

