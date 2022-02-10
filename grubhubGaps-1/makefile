#detecta se o sistema é de 32 ou 64 bits
N_BITS = $(shell getconf LONG_BIT)
ifeq ($(N_BITS),32)
   SYSTEM  = x86_sles10_4.1
   BITS_OPTION = -m32
else
   #SYSTEM  = x86-64_sles10_4.1
   SYSTEM = x86-64_linux
   
   BITS_OPTION = -m64
endif


LIBFORMAT = static_pic

####diretorios com as libs do cplex
#CPLEXDIR      = /usr/ilog/cplex

#CPLEXDIR = /opt/ibm/ILOG/CPLEX_Studio127/cplex


#Use this on office pc
#CPLEXDIR = /home/herthel/opt/ibm/ILOG/CPLEX_Studio127/cplex
#HPLaptop
CPLEXDIR = /opt/ibm/ILOG/CPLEX_Studio1210/cplex
#vsc
#CPLEXDIR = /home/lv70752/kiefera8/cplex

#CONCERTDIR    = /usr/ilog/concert

#CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio127/concert

#Use this on office pc
#
#CONCERTDIR = /home/herthel/opt/ibm/ILOG/CPLEX_Studio127/concert


#HPLaptop
CONCERTDIR = /opt/ibm/ILOG/CPLEX_Studio1210/concert
#vsc
#CONCERTDIR = /home/lv70752/kiefera8/concert

CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

####diretorios com as libs do gurobi
#GUROBIDIR =/opt/gurobi550/linux64
#GUROBIINC =$(GUROBIDIR)/include/
#GUROBILIB =$(GUROBIDIR)/lib/

#### define o compilador
#office
#CPPC = g++-4.8
#home/laser
CPPC = g++

#############################

#### opcoes de compilacao e includes
CCOPT = $(BITS_OPTION) -O3 -g -fPIC -fexceptions -DNDEBUG -DIL_STD -std=c++0x -Wno-ignored-attributes -Wno-deprecated-declarations
CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include
CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR) #-I$(GUROBIINC)
#############################

#### flags do linker
#CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -L$(GUROBILIB) -lgurobi_c++ -lgurobi55 -lgmpxx -lgmp -lm -lpthread        #Without lgmpxx  
#CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -L$(GUROBILIB) -lgurobi_c++ -lgurobi55 -lm -lpthread        #Without lgmpxx        
CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -lpthread -ldl                                                         #Without Gurobi and lgmpxx
#############################

#### diretorios com os source files e com os objs files
SRCDIR = src
OBJDIR = obj
#############################

#### lista de todos os srcs e todos os objs
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCS))
#############################

#### regra principal, gera o executavel
exeSARP: $(OBJS)
	@echo  "\033[31m \nLinking all objects files: \033[0m"
	$(CPPC) $(BITS_OPTION) $(OBJS) -o $@ $(CCLNFLAGS)
############################

#inclui os arquivos de dependencias
-include $(OBJS:.o=.d)

#regra para cada arquivo objeto: compila e gera o arquivo de dependencias do arquivo objeto
#cada arquivo objeto depende do .c e dos headers (informacao dos header esta no arquivo de dependencias gerado pelo compiler)
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo  "\033[31m \nCompiling $<: \033[0m"
	$(CPPC) $(CCFLAGS) -c $< -o $@
	@echo  "\033[32m \ncreating $< dependency file: \033[0m"
	$(CPPC)  -MM $< > $(basename $@).d
	@mv -f $(basename $@).d $(basename $@).d.tmp #proximas tres linhas colocam o diretorio no arquivo de dependencias (g++ nao coloca, surprisingly!)
	@sed -e 's|.*:|$(basename $@).o:|' < $(basename $@).d.tmp > $(basename $@).d
	@rm -f $(basename $@).d.tmp

#delete objetos e arquivos de dependencia
clean:
	@echo "\033[31mcleaning obj directory \033[0m"
	@rm -f $(OBJDIR)/*.o $(OBJDIR)/*.d

rebuild: clean exeSARP

