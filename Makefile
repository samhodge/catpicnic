
PLATFORM := $(shell uname)

ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglut32
	EXT = .exe
    DEFS =-DWIN32
endif

ifneq (, $(findstring Linux, $(PLATFORM)))
    GL_LIBS = -lGL -lglut -lGLEW 
    GL_LIBS += -L/usr/lib/nvidia-304 # necessary for some NVidia drivers;
	EXT = 
    DEFS =
endif

ifneq (, $(findstring Darwin, $(PLATFORM)))
    DEFS   = -DMacOSX -D__APPLE__ -m32 -arch x86_64 -stdlib=libc++
    DEFS += -Wno-deprecated # if you get sick of deprecated warnings..
#   DEFS += -DDEBUG
    GL_LIBS = -framework GLUT -framework OpenGL -framework CoreFoundation -lGLEW
	EXT = 
endif


ifneq (, $(findstring chile, $(HOSTNAME)))
# Workplace specific make arguments
    GLEW_LOC = /asset/common/software/thirdparty/glew/1.5.1-build3/arch/linux-centos5_2/x86_64
    GLUT_LOC = /asset/common/software/thirdparty/glut/3.7.0-build3
    GLUT_LIB_LOC = $(GLUT_LOC)/arch/linux-centos5_2/x86_64/
    GLUT_INC_LOC = $(GLUT_LOC)/include
    GL_LIBS = -lGL -lglut -lGLEW 
    GL_LIBS += -L/usr/lib/nvidia-304 # necessary for some NVidia drivers;
	EXT = 
    DEFS =-L$(GLEW_LOC)/lib -I$(GLEW_LOC)/include -L$(GLUT_LIB_LOC)/lib -I$(GLUT_INC_LOC)/include -I"../glm "
#    DEFS += " -std=gnu++0x "
#    DEFS +=-DDEBUG
endif


CAL3D = -I${PWD}/usr/local/include -L${PWD}/usr/local/lib 
CAL3D += -L${PWD}/usr/local/include/cal3d -lcal3d 

AUDIO_INCLUDE = -I${PWD}/usr/local/include
AUDIO_LIBS = -L${PWD}/usr/local/include -lao -lmpg123
    
OPENSTEER = -I${PWD}/opensteer-code/include 
LINK +=   sh_matrix.o shader.o tga.o viewer.o tick.o wander.o

.PHONY:  clean
LINK2 = shader.o skymap.o skybox.o sh_matrix.o Camera.o cubemap.o

LINK3 = shader.o skybox.o sh_matrix.o Camera.o Ocean.o cubemap.o skymap_ocean.o
LINK4 = shader.o skybox.o sh_matrix.o Camera.o Ocean.o cubemap.o cat.o skymap_ocean_cat.o tick.o tga.o
LINK5 = shader.o skybox.o sh_matrix.o Camera.o Ocean.o cubemap.o cat.o skymap_ocean_cat_wander.o wander.o tick.o tga.o
LINK6 = shader.o skybox.o sh_matrix.o Camera.o Ocean.o cubemap.o cat.o skymap_ocean_cat_wander_island.o wander.o tick.o tga.o Island.o Audio.o

all : assign3$(EXT) skymap$(EXT) skymap_ocean$(EXT) skymap_ocean_cat$(EXT) skymap_ocean_cat_wander$(EXT) viewer$(EXT)

skymap$(EXT) : $(LINK2) usr/local/lib/libcal3d.la
	g++ $(DEFS) $(CPPFLAGS) -o skymap $(LINK2) $(GL_LIBS)

skymap_ocean$(EXT) : $(LINK3) usr/local/lib/libcal3d.la
	g++ $(DEFS) $(CPPFLAGS) -o skymap_ocean $(LINK3) $(GL_LIBS)

skymap_ocean_cat$(EXT) : $(LINK4) usr/local/lib/libcal3d.la
	g++ $(DEFS) $(CPPFLAGS) -o skymap_ocean_cat $(LINK4) $(GL_LIBS)	 $(CAL3D)

skymap_ocean_cat_wander$(EXT) : $(LINK5) usr/local/lib/libcal3d.la
	g++ $(DEFS) $(CPPFLAGS) -o skymap_ocean_cat_wander $(LINK5) $(GL_LIBS)	 $(CAL3D)

assign3$(EXT) : $(LINK6) usr/local/lib/libcal3d.la usr/local/lib/libao.la usr/local/lib/libmpg123.la
	g++ $(DEFS) $(CPPFLAGS) -o assign3 $(LINK6) $(GL_LIBS) $(CAL3D)	$(AUDIO_LIBS) -pthread	

wander_test$(EXT) : wander.o cat_test.cpp usr/local/lib/libcal3d.la
	g++ $(DEFS) -o wander_test wander.o cat_test.cpp
	
viewer$(EXT) : $(LINK) usr/local/lib/libcal3d.la
	g++ $(DEFS) -o viewer main.cpp $(LINK) $(GL_LIBS) $(CAL3D)

tga.o : tga.cpp tga.h usr/local/lib/libcal3d.la
	g++ $(DEFS) $(CAL3D) -c tga.cpp
	
tick.o : tick.cpp tick.h usr/local/lib/libcal3d.la
	g++ $(DEFS) $(CAL3D) -c tick.cpp 

viewer.o : viewer.cpp viewer.h global.h usr/local/lib/libcal3d.la sh_matrix.h
	g++ $(DEFS)  $(CAL3D) $(GL_LIBS) -c viewer.cpp

shader.o : shader.cpp shader.h
	g++ $(DEFS) $(GL_LIBS) -c shader.cpp

sh_matrix.o : sh_matrix.h sh_matrix.cpp
	g++ $(DEFS) $(GL_LIBS) -c sh_matrix.cpp
	
wander.o : wander.cpp wander.h
	g++ $(DEFS)$(GL_LIBS)  -c wander.cpp
	
Simplex.o : Simplex.cpp Simplex.hpp
	g++ $(DEFS) $(GL_LIBS) -c Simplex.cpp

skymap.o: skymap.cpp shader.h 
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS)  -c skymap.cpp

skybox.o: skybox.h skybox.cpp cubemap.h
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS)  -c skybox.cpp

Camera.o: Camera.h Camera.cpp
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS)  -c Camera.cpp 

Ocean.o: Ocean.h Ocean.cpp cubemap.h
	g++ $(DEFS) $(CPPFLAGS)  $(GL_LIBS) -c Ocean.cpp

skymap_ocean.o: skymap_ocean.cpp shader.h
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS) -c skymap_ocean.cpp

skymap_ocean_cat.o: skymap_ocean_cat.cpp shader.h
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS) $(CAL3D) -c skymap_ocean_cat.cpp	

skymap_ocean_cat_wander.o: skymap_ocean_cat_wander.cpp shader.h
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS) $(CAL3D) -c skymap_ocean_cat_wander.cpp	

skymap_ocean_cat_wander_island.o: skymap_ocean_cat_wander_island.cpp usr/local/lib/libao.la usr/local/lib/libmpg123.la
	g++ $(DEFS) $(CPPFLAGS) $(AUDIO_INCLUDE) -c skymap_ocean_cat_wander_island.cpp			

cubemap.o: cubemap.cpp cubemap.h
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS) -c cubemap.cpp

cat.o: cat.h cat.cpp
	g++ $(DEFS) $(CPPFLAGS) $(CAL3D) $(GL_LIBS) -c cat.cpp

Island.o: Island.h Island.cpp
	g++ $(DEFS) $(CPPFLAGS) $(GL_LIBS) -c Island.cpp	

Audio.o: Audio.h Audio.cpp usr/local/lib/libao.la usr/local/lib/libmpg123.la
	g++ $(DEFS) $(CPPFLAGS) $(AUDIO_INCLUDE) -c Audio.cpp

usr/local/lib/libcal3d.la : ${PWD}/usr/local
	mkdir -p usr/local && tar -zvxf cal3d-0.11.0.tar.gz && cd cal3d-0.11.0 && ./configure --prefix ${PWD}/usr/local && echo "#include <cstring>" >> config.h && echo "#include <memory>" >> config.h && make clean && make && make install && touch cal3d

usr/local/lib/libao.la : ${PWD}/usr/local
	tar -zxvf libao-1.2.0.tar.gz && cd libao-1.2.0 && ./configure --prefix ${PWD}/usr/local && make clean && make && make install

usr/local/lib/libmpg123.la : ${PWD}/usr/local
	tar -xvf mpg123-1.22.2.tar && cd mpg123-1.22.2 && ./configure --prefix ${PWD}/usr/local && make clean && make && make install	


${PWD}/usr/local : cal3d-0.11.0.tar.gz mpg123-1.22.2.tar libao-1.2.0.tar.gz
	echo "Tar Ball Ready"

opensteer-demo:
	g++ opensteer-code/src/*.cpp opensteer-code/src/*.c opensteer-code/plugins/*.cpp -o opensteer-demo ${OPENSTEER} ${DEFS} ${GL_LIBS}
	
clean:
	rm -rf *.o assign3$(EXT) usr  cal3d-0.11.0
