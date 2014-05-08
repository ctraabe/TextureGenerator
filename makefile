TARGET     := texture-generator

CXXFLAGS   := -std=c++11
LDLIBS     := -lm
LDFLAGS    := -g

CXX        := g++

# If the environment variable DEV_BUILD_PATH is set, then the build files will
# be placed there in a named sub-folder, otherwise a build directory will be
# created in the current directory
ifneq ($(DEV_BUILD_PATH),)
BUILD_PATH := $(DEV_BUILD_PATH)/$(TARGET)/build
BIN_PATH   := $(DEV_BUILD_PATH)/bin
else
BUILD_PATH := ./build
BIN_PATH   := ./bin
endif
INSTALL_PATH ?= /usr/local

SOURCES    = $(wildcard *.cc)
OBJECTS    = $(addprefix $(BUILD_PATH)/, $(SOURCES:.cc=.o))
DEPENDS    = $(addprefix $(BUILD_PATH)/, $(SOURCES:.cc=.d))


# Rule to make dependency "makefiles"
$(BUILD_PATH)/%.d: %.cc $(BUILD_PATH)
	$(CXX) -MM -MT '$(addprefix $(BUILD_PATH)/, $(<:.cc=.o)) $@' $< -MF $@

# Rule to make the compiled objects
$(BUILD_PATH)/%.o: %.cc $(BUILD_PATH)/%.d
	$(CXX) -c $(CXXFLAGS) -o $@ $<

# Declare targets that are not files
.PHONY: install clean uninstall


# Note that without an argument, make simply tries to build the first target
# (not rule), which in this case is $(TARGET)
$(BIN_PATH)/$(TARGET): $(OBJECTS) $(BIN_PATH)
	g++ $(LDFLAGS) -o $(BIN_PATH)/$(TARGET) $(OBJECTS) $(LDLIBS)

install: $(INSTALL_PATH)
	cp $(BIN_PATH)/$(TARGET) $(INSTALL_PATH)/.

clean:
	rm -f $(OBJECTS) $(DEPENDS) $(BIN_PATH)/$(TARGET)

uninstall:
	rm -rf $(INSTALL_PATH)/$(TARGET)

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

$(BIN_PATH):
	mkdir -p $(BIN_PATH)

$(INSTALL_PATH):
	mkdir -p $(INSTALL_PATH)/$(TARGET)

# Include the dependency "makefiles"
ifneq ($(MAKECMDGOALS), clean)
-include $(DEPENDS)
endif
