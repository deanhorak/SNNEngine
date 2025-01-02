CXX      := g++
#CXXFLAGS := -O1 -g -Wall -Wno-reorder -Wno-write-strings -Wextra -fpermissive -DHAVE_CXX_STDHEADERS
# -Og
CXXFLAGS :=   -g -Og -Wall -Wno-reorder -Wno-write-strings -Wextra -fpermissive -DHAVE_CXX_STDHEADERS
BUILD    := ./build
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps

LDFLAGS  := -L/usr/lib -L$(OBJ_DIR) -L/usr/local/BerkeleyDB.18.1/lib  
LIBS     := -lstdc++ -lm -lboost_system -lboost_thread -lboost_filesystem -ldb -lcpprest -lcrypto -ldb_cxx -ldb_cxx-18 -lglut -lGLU -lGL 

# Add OpenCV libraries using pkg-config
LIBS     += `pkg-config --libs opencv4`

TARGET   := SNNEngine
INCLUDE  := -I/include -I/usr/local/BerkeleyDB.18.1/include -I/usr/include -I/usr/include/opencv4 

# Add OpenCV include flags using pkg-config
INCLUDE  += `pkg-config --cflags opencv4`
SRC      :=  $(wildcard *.cpp) 


OBJECTS      := $(SRC:%.cpp=$(OBJ_DIR)/%.o)  
DEPENDENCIES := $(OBJECTS:.o=.d) 

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

#$(APP_DIR)/$(TARGET): $(OBJECTS)
$(APP_DIR)/$(TARGET): $(OBJECTS) 
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS) $(LIBS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"
                       
