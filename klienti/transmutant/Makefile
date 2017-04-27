SERVERDIR = ../../server
CXXFLAGS = -O2 -MMD -MP -Wall -I$(SERVERDIR) -std=gnu++11

CLEAN = *.d *.o hrac klient.tar.gz *_edm.* blossom5-v2.05.src/blossom5

all: blossom5-v2.05.src/blossom5 hrac klient.tar.gz 
naserveri: blossom5-v2.05.src/blossom5 hrac

blossom5-v2.05.src/blossom5:
	(cd blossom5-v2.05.src; make; cd ..;)

.PHONY: klient.tar.gz
klient.tar.gz:
	tar czf $@ $(addprefix --exclude=,$(CLEAN)) *

hrac: main.o server_marshal.o server_common.o
	$(LINK.cpp) $^ $(LDLIBS) -o $@

server_%.o: $(SERVERDIR)/%.cpp
	$(COMPILE.cpp) -o $@ $<

clean:
	-rm -f $(CLEAN)

notedm:
	rm -f *_edm.*

-include *.d
