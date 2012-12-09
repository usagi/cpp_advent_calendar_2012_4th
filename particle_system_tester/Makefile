all: o3 o2 o1 o0

o3:
	g++ -std=c++11 main.cpp -o o3 -O3

o2:
	g++ -std=c++11 main.cpp -o o2 -O2

o1:
	g++ -std=c++11 main.cpp -o o1 -O1

o0:
	g++ -std=c++11 main.cpp -o o0 -O0

pg: o3-pg o2-pg o1-pg o0-pg

o3-pg:
	g++ -std=c++11 main.cpp -o o3-pg -pg -O3

o2-pg:
	g++ -std=c++11 main.cpp -o o2-pg -pg -O2

o1-pg:
	g++ -std=c++11 main.cpp -o o1-pg -pg -O1

o0-pg:
	g++ -std=c++11 main.cpp -o o0-pg -pg -O0

gprof: o3-gprof o2-gprof o1-gprof o0-gprof

o3-gprof: o3-pg
	./o3-pg 1000 && mv gmon.out o3-gmon.out && gprof ./o3-pg o3-gmon.out > o3-gprof.log

o2-gprof: o2-pg
	./o2-pg 1000 && mv gmon.out o2-gmon.out && gprof ./o2-pg o2-gmon.out > o2-gprof.log

o1-gprof: o1-pg
	./o1-pg 1000 && mv gmon.out o1-gmon.out && gprof ./o1-pg o1-gmon.out > o1-gprof.log

o0-gprof: o0-pg
	./o0-pg 100 && mv gmon.out o0-gmon.out && gprof ./o0-pg o0-gmon.out > o0-gprof.log

oprofile: o3-op o2-op o1-op o0-op

o3-g:
	g++ -std=c++11 main.cpp -o o3-g -g -O3

o2-g:
	g++ -std=c++11 main.cpp -o o2-g -g -O2

o1-g:
	g++ -std=c++11 main.cpp -o o1-g -g -O1

o0-g:
	g++ -std=c++11 main.cpp -o o0-g -g -O0

o3-op: o3-g
	sudo opcontrol --start && sudo opcontrol --reset
	./o3-g 1000
	sudo opcontrol --stop
	sudo opcontrol --dump
	sudo opcontrol --shutdown
	opreport --demangle smart ./o3-g > o3-op.profile
	opannotate -s ./o3-g > o3-op.annotate

o2-op: o2-g
	sudo opcontrol --start && sudo opcontrol --reset
	./o2-g 1000
	sudo opcontrol --stop
	sudo opcontrol --dump
	sudo opcontrol --shutdown
	opreport --demangle smart ./o2-g > o2-op.profile
	opannotate -s ./o2-g > o2-op.annotate

o1-op: o1-g
	sudo opcontrol --start && sudo opcontrol --reset
	./o1-g 1000
	sudo opcontrol --stop
	sudo opcontrol --dump
	sudo opcontrol --shutdown
	opreport --demangle smart ./o1-g > o1-op.profile
	opannotate -s ./o1-g > o1-op.annotate

o0-op: o0-g
	sudo opcontrol --start && sudo opcontrol --reset
	./o0-g 100
	sudo opcontrol --stop
	sudo opcontrol --dump
	sudo opcontrol --shutdown
	opreport --demangle smart ./o0-g > o0-op.profile
	opannotate -s ./o0-g > o0-op.annotate

clean:
	@rm -v o*

