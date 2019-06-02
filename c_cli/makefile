CC=gcc
CFLAGS=-std=c99 -Wall

ODIR=obj
HEADERS=defs.h core.h io.h solver.h rearrange.h


saper: $(ODIR)/main.o $(ODIR)/io.o $(ODIR)/core.o $(ODIR)/solver.o $(ODIR)/rearrange.o $(HEADERS)
	$(CC) $(CFLAGS) -o saper $(ODIR)/main.o $(ODIR)/core.o $(ODIR)/io.o $(ODIR)/solver.o $(ODIR)/rearrange.o

$(ODIR)/%.o: %.c $(HEADERS) | $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR):
	mkdir $(ODIR)


clean:
	rm -r $(ODIR)

distclean: clean
	rm saper


##############################
#	$@ - before :
#	$< - first element after :

