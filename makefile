# Note that without an argument, make simply tries to build the first target
# (not rule), which in this case is $(TARGET)

TARGET   = texture_generator

CXX      = g++

CXXFLAGS = -std=c++11
LDLIBS   = -lm
LDFLAGS  = -g

SOURCES  = $(wildcard *.cc)
OBJECTS  = $(SOURCES:.cc=.o)
DEPENDS  = $(SOURCES:.cc=.d)


# Rule to make dependency "makefiles"
%.d: %.cc
	$(CXX) -MM -MT '$(<:.cc=.o) $@' $< -MF $@

# Rule to make the compiled objects
%.o: %.cc %.d
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# Declare targets that are not files
.PHONY: clean

$(TARGET): $(OBJECTS)
	g++ $(LDFLAGS) -o $(TARGET) $(OBJECTS) $(LDLIBS)

# Include the dependency "makefiles"
ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDS)
endif

clean:
	$(RM) -f $(TARGET) $(OBJECTS) $(DEPENDS)
