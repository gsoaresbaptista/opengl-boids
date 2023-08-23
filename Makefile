# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Isrc -Ithird_party
LDFLAGS := -lglfw -lGL

# Directories
SRC_DIR := src
BUILD_DIR := build
LIBS_DIR := $(BUILD_DIR)/third_party
TARGET := boids

# List of modules
MODULES := core camera shapes
THIRD_PARTY := glad imgui
FOLDER_PATHS = $(addprefix $(BUILD_DIR)/, $(MODULES))
FOLDER_PATHS += $(addprefix $(LIBS_DIR)/, $(THIRD_PARTY))

# get all source files
CPP_FILES = $(foreach module,$(MODULES),$(wildcard $(SRC_DIR)/$(module)/*.cpp))
CPP_LIB_FILES = $(foreach lib,$(THIRD_PARTY),$(wildcard third_party/$(lib)/*.cpp))
C_LIB_FILES = $(foreach lib,$(THIRD_PARTY),$(wildcard third_party/$(lib)/*.c))

# get all object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_FILES))
OBJ_FILES += $(patsubst third_party/%.cpp, $(LIBS_DIR)/%.o, $(CPP_LIB_FILES))
OBJ_FILES += $(patsubst third_party/%.c, $(LIBS_DIR)/%.o, $(C_LIB_FILES))

# recipes
all: folders $(TARGET)

print:
	@echo $(CPP_LIB_FILES)
	@echo ""
	@echo $(C_LIB_FILES)
	@echo ""
	@echo $(OBJ_FILES)

# add a rule to compile each object file
define compile_src
$(BUILD_DIR)/$(1)/%.o: $(SRC_DIR)/$(1)/%.cpp $(SRC_DIR)/$(1)/%.hpp
	$$(CXX) $$(CXXFLAGS) -c $$< -o $$@
endef

$(foreach module,$(MODULES),$(eval $(call compile_src,$(module))))

define compile_lib
$(LIBS_DIR)/$(1)/%.o: third_party/$(1)/%.cpp
	$$(CXX) $$(CXXFLAGS) -c $$< -o $$@
$(LIBS_DIR)/$(1)/%.o: third_party/$(1)/%.cpp third_party/$(1)/%.h
	$$(CXX) $$(CXXFLAGS) -c $$< -o $$@
$(LIBS_DIR)/$(1)/%.o: third_party/$(1)/%.c third_party/$(1)/%.h
	$$(CXX) $$(CXXFLAGS) -c $$< -o $$@
endef

$(foreach lib,$(THIRD_PARTY),$(eval $(call compile_lib,$(lib))))

# other rules
$(BUILD_DIR)/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES) build/main.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

folders:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIBS_DIR)
	@mkdir -p $(FOLDER_PATHS)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(LIBS_DIR)
	rm -f $(TARGET)
