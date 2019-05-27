CC=gcc
CFLAGS=-std=c99 -Wall

ODIR=obj
HEADERS=defs.h io.h


$(ODIR)/%.o: %.c $(HEADERS) $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

saper: $(ODIR)/main.o $(ODIR)/io.o io.h
	$(CC) $(CFLAGS) -o saper $(ODIR)/main.o $(ODIR)/io.o

$(ODIR):
	mkdir $(ODIR)


clean:
	rm -r $(ODIR)

distclean: clean
	rm saper


##############################
#	$@ - before :
#	$< - first element after :

