# Construire manuală a proiectului Pacman (Linux sau terminal MSYS2).
#
#   make          - construiește bibliotecile statice și jocul (build/pacman)
#   make libs     - doar bibliotecile statice (build/lib/*.a)
#   make run      - construiește și pornește jocul
#   make test     - construiește și rulează testele
#   make clean    - șterge folderul build
#
# Biblioteca externă SQLite3 trebuie instalată:
#   MSYS2 (terminal UCRT64):  pacman -S mingw-w64-ucrt-x86_64-sqlite3
#   Debian / Ubuntu:          sudo apt install libsqlite3-dev
# Dacă este într-un loc neobișnuit:  make SQLITE_CFLAGS=-I/cale/include SQLITE_LIBS="-L/cale/lib -lsqlite3"

CXX      ?= g++
AR       ?= ar
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
CPPFLAGS := -Iinclude $(SQLITE_CFLAGS) -MMD -MP

SQLITE_CFLAGS ?=
SQLITE_LIBS   ?= -lsqlite3

ifeq ($(OS),Windows_NT)
EXE := .exe
endif

BUILD  := build
OBJDIR := $(BUILD)/obj
LIBDIR := $(BUILD)/lib
TARGET := $(BUILD)/pacman$(EXE)
TEST   := $(BUILD)/smoke_test$(EXE)

# ---- Bibliotecile statice: câte una pentru fiecare clasă de date ----
#   nume bibliotecă -> fișierele sursă din care se construiește (fără extensie)
LIBS := enums position pacman ghost board score config highscores sqlite

SRC_enums      := Enums
SRC_position   := Position
SRC_pacman     := Pacman
SRC_ghost      := Ghost
SRC_board      := Board
SRC_score      := Score
SRC_config     := GameConfig
SRC_highscores := HighScores ScoreStorage
SRC_sqlite     := SqliteScoreStorage          # folosește biblioteca externă SQLite3

LIB_FILES := $(foreach l,$(LIBS),$(LIBDIR)/libpacman_$(l).a)
LIB_OBJS  := $(foreach l,$(LIBS),$(addprefix $(OBJDIR)/,$(addsuffix .o,$(SRC_$(l)))))

# ---- Aplicația (nu face parte din biblioteci) ----
APP_SRC  := GameEngine Renderer Listener Game Demo
APP_OBJS := $(addprefix $(OBJDIR)/,$(addsuffix .o,$(APP_SRC)))
MAIN_OBJ := $(OBJDIR)/main.o
TEST_OBJ := $(OBJDIR)/smoke_test.o
DEPS     := $(LIB_OBJS:.o=.d) $(APP_OBJS:.o=.d) $(MAIN_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

# Bibliotecile se leagă într-un grup, ca ordinea lor să nu conteze; SQLite3 la sfârșit.
LINK_LIBS := -L$(LIBDIR) -Wl,--start-group $(addprefix -lpacman_,$(LIBS)) -Wl,--end-group $(SQLITE_LIBS)

.PHONY: all libs run test clean

all: $(TARGET)

libs: $(LIB_FILES)

$(TARGET): $(APP_OBJS) $(MAIN_OBJ) $(LIB_FILES)
	$(CXX) $(APP_OBJS) $(MAIN_OBJ) $(LINK_LIBS) -o $@

$(TEST): $(APP_OBJS) $(TEST_OBJ) $(LIB_FILES)
	$(CXX) $(APP_OBJS) $(TEST_OBJ) $(LINK_LIBS) -o $@

# Regula de arhivare: libpacman_<nume>.a se construiește din obiectele SRC_<nume>
define make_lib
$(LIBDIR)/libpacman_$(1).a: $$(addprefix $(OBJDIR)/,$$(addsuffix .o,$$(SRC_$(1)))) | $(LIBDIR)
	$$(AR) rcs $$@ $$^
endef
$(foreach l,$(LIBS),$(eval $(call make_lib,$(l))))

$(OBJDIR)/%.o: src/%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR)/smoke_test.o: tests/smoke_test.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJDIR) $(LIBDIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST)
	./$(TEST)

clean:
	rm -rf $(BUILD)

-include $(DEPS)
