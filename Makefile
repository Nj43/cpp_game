RM = rm -f
EXE =
OPT = -O3
#
#	Compilation on Windows for Windows
#
ifeq '$(OS)' "Windows_NT"
FLTK_VERSION = fltk-1.4-windows
EXE = .exe
LD = ld
RM = del
CXX = g++
CXXFLAGS =		-Wall $(OPT)

CONSOLE =		-Wl,--subsystem,console
LINKFLTK_IMG =	$(CONSOLE) -mwindows -L$(FLTK_VERSION) \
				-lfltk_images -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl -lfltk \
				-lwinmm -lglu32 -lopengl32 -lole32 -luuid \
				-lcomctl32 -lws2_32 -lwinpthread -static

else
#
#	Compilation on MacOSX for MacOSX
#
ifeq '$(shell uname -s)' "Darwin"
ifeq '$(shell uname -p)' "arm"
FLTK_VERSION = fltk-1.4-macosx-arm
else
FLTK_VERSION = fltk-1.4-macosx
endif

OS = Darwin
CXX = g++
LD = ld
CXXFLAGS =		-Wall -Wno-invalid-source-encoding -Wno-deprecated-declarations \
				-I /usr/include/freetype2 -I /usr/X11/include $(OPT)

LINKFLTK_IMG =	-L$(FLTK_VERSION) -lfltk_png -lfltk_z -lfltk_jpeg -lfltk_gl \
				-lfltk -framework Cocoa -framework AGL -framework OpenGL \
				-framework ApplicationServices
else
ifeq '$(OS)' "Ubuntu"
#
#	Compilation on Linux for Linux (Ubuntu)

FLTK_VERSION = fltk-1.4-ubuntu
CXX = g++
LD = ld
CXXFLAGS =		-Wall -DUBUNTU -I /usr/include/freetype2 -I /usr/X11/include $(OPT)

LINKFLTK_IMG =	-L$(FLTK_VERSION) -lfltk_jpeg \
				-lfltk_png -lfltk_z -lfltk_gl -lfltk -lGLU -lGL \
				-lXext -lXft -lXrender -lfontconfig -ldl -lm -lX11 -lXfixes
else
#
#	Compilation on Linux for Linux (Fedora et autres)
#
FLTK_VERSION = fltk-1.4-linux
FMOD = fmod/4.3
CXX = g++
LD = ld
CXXFLAGS =		-Wall -I $(FMOD) -I /usr/include/freetype2 -I /usr/X11/include $(OPT)

LINKFLTK_IMG =	$(FMOD)/libfmodex64.so -L$(FLTK_VERSION) -lfltk_jpeg \
				-lfltk_png -lfltk_z -lfltk_gl -lfltk -lGLU -lGL \
				-lXext -lXft -lXrender -lfontconfig -ldl -lm -lX11 -lXfixes
endif
endif
endif

.SUFFIXES: .h .o .cc $(EXE)

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

#
#	D�but du Makefile propre au projet
#

SRC	=	Labyrinthe.cc Chasseur.cc Gardien.cc
H	=	Labyrinthe.h Chasseur.h FireBall.h Sound.h Environnement.h \
		Gardien.h Mover.h
O	=	Labyrinthe.o Chasseur.o Gardien.cc

ifeq '$(OS)' "Darwin"
ifeq '$(shell uname -p)' "arm"
OPENGL_O = OpenGL-macosx-arm.o
else
OPENGL_O = OpenGL-macosx.o
endif
else
ifeq '$(OS)' "Windows_NT"
OPENGL_O = OpenGL-windows.o
else
ifeq '$(OS)' "Ubuntu"
OPENGL_O = OpenGL-ubuntu.o
else
OPENGL_O = OpenGL-linux.o
endif
endif
endif

labh$(EXE):	$(O)
ifeq '$(OS)' "Darwin"
	$(CXX) -o $@ $(O) $(OPENGL_O) $(LINKFLTK_IMG)
else
	$(CXX) -no-pie -o $@ $(O) $(OPENGL_O) $(LINKFLTK_IMG)
endif

clean:
	$(RM) labh labh.exe $(O)

Labyrinthe.o:	Labyrinthe.h Environnement.h Chasseur.h Mover.h Gardien.h
Chasseur.o:		Chasseur.cc Chasseur.h Mover.h Sound.h
Gardien.cc:		Chasseur.cc Chasseur.h Mover.h Sound.h
