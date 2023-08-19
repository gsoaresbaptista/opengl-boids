# Compiler and flags
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Isrc -Ithird_party
LDFLAGS := -lglfw -lGL

# Directories
SRC_DIR := src
BUILD_DIR := build
TARGET := boids

# List of modules
MODULES := core
THIRD_PARTY := glad
CPP_FILES := $(foreach module,$(MODULES),$(wildcard $(SRC_DIR)/$(module)/*.cpp))
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(CPP_FILES))
FOLDER_PATHS := $(addprefix $(BUILD_DIR)/, $(MODULES))
THIRD_PARTY_OBJECTS := $(addprefix $(BUILD_DIR)/, $(addsuffix .o, $(THIRD_PARTY)))

all: folders $(TARGET)

# add a rule to compile each object file
define compile_rule
$(BUILD_DIR)/$(1)/%.o: $(SRC_DIR)/$(1)/%.cpp $(SRC_DIR)/$(1)/%.hpp
	$$(CXX) $$(CXXFLAGS) -c $$< -o $$@
endef

$(foreach module,$(MODULES),$(eval $(call compile_rule,$(module))))

# other rules
$(TARGET): $(OBJ_FILES) $(THIRD_PARTY_OBJECTS) build/main.o
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: third_party/%/*.cpp third_party/%/*.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: third_party/%/*.c third_party/%/*.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/main.o: src/main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

folders:
	@mkdir -p build
	@mkdir -p $(FOLDER_PATHS)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TARGET)
