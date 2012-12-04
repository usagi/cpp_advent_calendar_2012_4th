APP_NAME     :=cppac2012_4th

THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
NACL_SDK_ROOT?=$(abspath $(dir $(THIS_MAKEFILE))../..)
OSNAME       :=$(shell uname --kernel-name | tr "[:upper:]" "[:lower:]")
TC_PATH      :=$(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_x86_glibc)

CXXPPAPI    := -lppapi -lppapi_cpp -lppapi_gles2
CXXLIBPATH  := 
CXXLIBS     := 
CXXINCLUDES := -Iinclude
CXXWARNINGS := -Wall
CXXSTD      := -std=c++0x
CXXOPTIMIZE := -O2
CXXTHREAD   := -pthread

CXXFLAGS:=$(CXXTHREAD) $(CXXSTD) $(CXXOPTIMIZE) $(CXXWARNINGS) $(CXXPPAPI) $(CXXLIBPATH) $(CXXLIBS) $(CXXINCLUDES)

CXX:=$(TC_PATH)/bin/i686-nacl-g++

DESTINATION   :=_site

REMOTE_DEPLOY_COMMAND:=rsync -av --delete
REMOTE_DEPLOY_TEST   :=$(APP_NAME).WonderRabbitProject.net:/srv/http/WonderRabbitProject.net/$(APP_NAME)/test
REMOTE_DEPLOY_CURRENT:=$(APP_NAME).WonderRabbitProject.net:/srv/http/WonderRabbitProject.net/$(APP_NAME)/current

LOCAL_TEST_SERVER      :=darkhttpd
LOCAL_TEST_SERVER_OPTS :=--index main.html --port 12345
LOCAL_TEST_BROWSER     :=google-chrome
LOCAL_TEST_BROWSER_OPTS:=--incognito http://localhost:12345
LOCAL_TEST_LOG         :=test-client.log

all: $(APP_NAME)_i686.nexe $(APP_NAME)_x86_64.nexe

$(APP_NAME)_i686.nexe: src.cxx/$(APP_NAME)-client-nacl.cxx $(THIS_MAKE)
	$(CXX) -o $@ $< -m32 $(CXXFLAGS)

$(APP_NAME)_x86_64.nexe: src.cxx/$(APP_NAME)-client-nacl.cxx $(THIS_MAKE)
	$(CXX) -o $@ $< -m64 $(CXXFLAGS)

clean:
	@-if [ -d $(DESTINATION) ]; then rm -rfv $(DESTINATION)/*; fi;
	@-rmdir -v $(DESTINATION)
	@-rm -v *.o
	@-rm -v *.nexe
	@-rm -v *.nmf

.PHONY: _site_dir
_site_dir:
	@if [ -d $(DESTINATION) ]; then rm -rf $(DESTINATION)/*; else mkdir $(DESTINATION); fi;

.PHONY: glibc_so_64
glibc_so_64: _site_dir $(APP_NAME)_x86_64.nexe
	@if [ -d $(DESTINATION)/lib64 ]; then rm -rf $(DESTINATION)/lib64/*; else mkdir $(DESTINATION)/lib64; fi;
	@ln -v $(TC_PATH)/x86_64-nacl/lib/runnable-ld.so $(DESTINATION)/lib64/
	@for a in `$(TC_PATH)/bin/x86_64-nacl-objdump -p $(APP_NAME)_x86_64.nexe | grep NEEDED | tr NEEDED " " | sed "s/^[ ]*//" | tr "\n" " "`; do ln -v $(TC_PATH)/x86_64-nacl/lib/$${a} $(DESTINATION)/lib64/$${a}; done;
	@chmod 644 $(DESTINATION)/lib64/*

.PHONY: glibc_so_32
glibc_so_32: _site_dir $(APP_NAME)_i686.nexe
	@if [ -d $(DESTINATION)/lib32 ]; then rm -rf $(DESTINATION)/lib32/*; else mkdir $(DESTINATION)/lib32; fi;
	@ln -v $(TC_PATH)/x86_64-nacl/lib32/runnable-ld.so $(DESTINATION)/lib32/
	@for a in `$(TC_PATH)/bin/i686-nacl-objdump -p $(APP_NAME)_i686.nexe | grep NEEDED | tr NEEDED " " | sed "s/^[ ]*//" | tr "\n" " "`; do ln -v $(TC_PATH)/x86_64-nacl/lib32/$${a} $(DESTINATION)/lib32/$${a}; done;
	@chmod 644 $(DESTINATION)/lib32/*

.PHONY: $(APP_NAME).nmf
$(APP_NAME).nmf: _site_dir $(APP_NAME)_x86_64.nexe
	@echo '{' >> $(APP_NAME).nmf
	@echo '  "program": {' >> $(APP_NAME).nmf
	@echo '     "x86-64": { "url": "lib64/runnable-ld.so" },' >> $(APP_NAME).nmf
	@echo '     "x86-32": { "url": "lib32/runnable-ld.so" }' >> $(APP_NAME).nmf
	@echo '  },' >> $(APP_NAME).nmf
	@echo '  "files": {' >> $(APP_NAME).nmf
	@for a in `$(TC_PATH)/bin/i686-nacl-objdump -p $(APP_NAME)_i686.nexe | grep NEEDED | tr NEEDED " " | sed "s/^[ ]*//" | tr "\n" " "`;\
		do\
		echo "    \"$${a}\": {" >> $(APP_NAME).nmf;\
		echo "      \"x86-64\": { \"url\": \"lib64/$${a}\"}," >> $(APP_NAME).nmf;\
		echo "      \"x86-32\": { \"url\": \"lib32/$${a}\"}" >> $(APP_NAME).nmf;\
		echo '    },' >> $(APP_NAME).nmf;\
		done;
	@echo '    "main.nexe": {' >> $(APP_NAME).nmf
	@echo '      "x86-64": { "url": "$(APP_NAME)_x86_64.nexe" },' >> $(APP_NAME).nmf
	@echo '      "x86-32": { "url": "$(APP_NAME)_i686.nexe" }' >> $(APP_NAME).nmf
	@echo '    }' >> $(APP_NAME).nmf
	@echo '  }' >> $(APP_NAME).nmf
	@echo "}" >> $(APP_NAME).nmf

.PHONY: _site
_site: _site_dir $(APP_NAME).nmf glibc_so_64 glibc_so_32 $(APP_NAME)_x86_64.nexe $(APP_NAME)_i686.nexe
	@-ln -v *.nexe $(DESTINATION)
	@-ln -v *.html $(DESTINATION)
	@-ln -v *.nmf  $(DESTINATION)
	@-ln -v *.js   $(DESTINATION)
	@-ln -v *.css  $(DESTINATION)
	@-ln -v favicon.ico $(DESTINATION)

.PHONY: test-server
test-server: all _site
	$(LOCAL_TEST_SERVER) $(DESTINATION) $(LOCAL_TEST_SERVER_OPTS)

.PHONY: test-client
test-client: 
	@ if [ -f test-client.log ]; then rm -v test-client.log; fi;
	$(LOCAL_TEST_BROWSER) $(LOCAL_TEST_BROWSER_OPTS) | tee $(LOCAL_TEST_LOG)

.PHONY: deploy-test
deploy-test: all _site
	$(REMOTE_DEPLOY_COMMAND) $(DESTINATION)/ $(REMOTE_DEPLOY_TEST)

.PHONY: deploy-current
deploy-current: all _site
	$(REMOTE_DEPLOY_COMMAND) $(DESTINATION)/ $(REMOTE_DEPLOY_CURRENT)

