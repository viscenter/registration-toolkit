# flags = -lGLEW -lGLU -lglut -shared -D_UNIX -fPIC
flags = -framework GLUT -framework OpenGL -lGLEW -shared -fPIC -g
#linuxJdk = /usr/java/j2sdk1.4.1_04

# flags = -lopengl32 -lglu32 -lglut32 -mno-cygwin
# jdk = /cygdrive/c/j2sdk1.4.2_04
jdk = /Library/Java/JavaVirtualMachines/jdk1.8.0_05.jdk/Contents/Home

educe: Interface Java

Java: educe.class EduceProject.class EduceMenuBar.class CanvasWindow.class ControlPanel.class SingleFileFilter.class HistogramLabel.class

Interface: JavaCInterface.class C_Side JavaCInterface.so

C_Side: JavaCInterface.h Control.o Callback.o Volume.o Miniture.o Selection.o


educe.class: educe.java
	javac educe.java

EduceProject.class: EduceProject.java
	javac EduceProject.java

EduceMenuBar.class: EduceMenuBar.java
	javac EduceMenuBar.java

CanvasWindow.class: CanvasWindow.java
	javac CanvasWindow.java

ControlPanel.class: ControlPanel.java
	javac ControlPanel.java

SingleFileFilter.class: SingleFileFilter.java
	javac SingleFileFilter.java

HistogramLabel.class: HistogramLabel.java
	javac HistogramLabel.java


JavaCInterface.class: JavaCInterface.java
	javac JavaCInterface.java

JavaCInterface.so: JavaCInterface.h JavaCInterface.cpp Control.o Callback.o Volume.o Miniture.o Selection.o
	gcc $(flags) -I$(jdk)/include -I$(jdk)/include/darwin -Wl, -shared -o JavaCInterface.so JavaCInterface.cpp Control.o Callback.o Volume.o Miniture.o Selection.o  -lstdc++

JavaCInterface.h: JavaCInterface.class
	javah JavaCInterface
	cat JavaCInterface.h JavaCInterfaceAdditional > JavaCInterface.temp
	mv JavaCInterface.temp JavaCInterface.h

Control.o: Control.h Control.cpp
	gcc -I$(jdk)/include -I$(jdk)/include/darwin -c Control.cpp

Callback.o: Callback.h Callback.cpp
	gcc -I$(jdk)/include -I$(jdk)/include/darwin -c Callback.cpp

Volume.o: Volume.h Volume.cpp
	gcc -I$(jdk)/include -I$(jdk)/include/darwin -c Volume.cpp

Miniture.o: Miniture.h Miniture.cpp
	gcc -I$(jdk)/include -I$(jdk)/include/darwin -c Miniture.cpp

Selection.o: Selection.h Selection.cpp
	gcc -I$(jdk)/include -I$(jdk)/include/darwin -c Selection.cpp

clean:
	rm -r *.class *.so *.o *.dSYM
