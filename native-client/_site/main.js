if (!wrp) var wrp = {};

//WRP_DEBUG_BEGIN
if (!wrp.etc) wrp.etc = {};
wrp.etc.log_off = true;
if (!wrp.log){
  if (!wrp.etc.log_off) {
    wrp.log = function(a, b){
      var t = wrp.log_stack[wrp.log_stack.length - 1];
      var i = '';
      for(var n = wrp.log_stack.length - 2; n; --n)
        i += '  ';
      console.log(i + '[' + new Date().getTime() + ':' + t.name + '] ', a, t.object);
    };
    wrp.log_push = function(n, o){
      wrp.log_stack.push( {name:n, object:o} );
      wrp.log('>>> push >>>');
    };
    wrp.log_pop = function(){
      wrp.log('<<< pop  <<<');
      wrp.log_stack.pop();
    };
    wrp.log_stack = [{name:void 0,object:void 0}];
  }else
    wrp.log = function(){};
    wrp.log_push=function(){};
    wrp.log_pop=function(){};
}
//WRP_DEBUG_END

wrp.nacl = (function(){
  var wrp_nacl = function(){
    this.etc = {
      dom: {
        nacl_container: 'nacl_container',
        nacl_target   : 'nacl_target',
      },
    };
    this.tmp = {
      proc  : wrp.nacl.proc.length,
      status: 0,
      dom   : {},
    };
    wrp.nacl.proc.push(this);
  };
  
  wrp_nacl.prototype = {
    initialize: function(){
      wrp.log_push('initialize', this);
      this.initialize_dom();
      this.initialize_event();
      this.initialize_status();
      wrp.log_pop();
    },
    initialize_dom: function(){
      wrp.log_push('initialize_dom', this);
      var ed = this.etc.dom;
      var td = this.tmp.dom;
      td.nacl_container = document.getElementById(ed.nacl_container);
      td.nacl_target    = document.getElementById(ed.nacl_target);
      wrp.log_pop();
    },
    initialize_event: function(){
      wrp.log_push('initialize_event', this);
      var c = this.tmp.dom.nacl_container;
      var t = this;
      c.addEventListener('load'     , function() { t.on_load();      } , true);
      c.addEventListener('message'  , function(a){ t.on_message(a);  } , true);
      c.addEventListener('loadstart', function() { t.on_loadstart(); } , true);
      c.addEventListener('loadend'  , function() { t.on_loadend();   } , true);
      c.addEventListener('progress' , function(a){ t.on_progress(a); } , true);
      c.addEventListener('error'    , function() { t.on_error();     } , true);
      c.addEventListener('abort'    , function() { t.on_abort();     } , true);
      c.addEventListener('crash'    , function() { t.on_crash();     } , true);
      wrp.log_pop();
    },
    initialize_status: function(){
      wrp.log_push('initialize_status', this);
      this.change_status(1);
      wrp.log_pop();
    },
    change_status: function(a){
      wrp.log_push('change_status', this);
      wrp.log(this.tmp.status + ' --> ' + a);
      this.tmp.status = a;
      wrp.log_pop();
    },
    on_load: function(){
      wrp.log_push('on_load', this);
      this.tmp.dom.nacl_target =
        document.getElementById(this.etc.dom.nacl_target);
      this.change_status(2);
      wrp.log_pop();
    },
    on_message: function(a){
      wrp.log_push('on_message',this);
      //wrp.log(a.data);
      if(this.message)
        this.message(a);
      wrp.log_pop();
    },
    post_message: function(a){
      wrp.log_push('post_message', this);
      wrp.log(a);
      this.tmp.dom.nacl_target.postMessage(a);
      wrp.log_pop();
    },
    on_loadstart: function(){
      wrp.log_push('on_loadstart', this);
      if(this.load_start)
        this.load_start();
      wrp.log_pop();
    },
    on_loadend: function(){
      wrp.log_push('on_loadend', this);
      if(this.load_end)
        this.load_end();
      wrp.log_pop();
    },
    on_progress: function(a){
      wrp.log_push('on_progress', this);
      var r = a.loaded / a.total;
      this.change_status(1.0 + r);
      if(this.loading)
        this.loading(r);
      wrp.log_pop();
    },
    on_error: function(){
      wrp.log_push('on_error', this);
      if(this.error)
        this.error();
      this.nacl_target_dump();
      wrp.log_pop();
    },
    on_abort: function(){
      wrp.log_push('on_abort', this);
      if(this.abort)
        this.abort();
      this.nacl_target_dump();
      wrp.log_pop();
    },
    on_crash: function(){
      wrp.log_push('on_crash', this);
      if(this.crash)
        this.crash();
      this.nacl_target_dump();
      wrp.log_pop();
    },
    nacl_target_dump: function(){
      wrp.log_push('nacl_target_dump', this);
      var t = this.tmp.dom.nacl_target;
      wrp.log('lastError : ' + t.lastError );
      wrp.log('readyState: ' + t.readyState);
      wrp.log_pop();
    },
  };
  
  wrp_nacl.proc = [];
  return wrp_nacl;
})();

// global tmp
var tmp = {};

var initialize = function(){
  var css_fix_size = function(){
    wrp.log_push('css_fix_size', this);
    var margin = 20;
    var s = document.styleSheets[document.styleSheets.length - 1];
    var m0 = Math.min(window.innerWidth, window.innerHeight);
    var m1 = Math.min(14, m0 * 0.02);
    var m2 = Math.min(14, m0 * 0.023);
    wrp.log_pop();
  };
  window.addEventListener('resize', css_fix_size, true);
  css_fix_size();
};

var main = function(){
  initialize();
  
  var c = tmp.canvas = document.createElement('canvas');
  document.body.appendChild(c);
  tmp.context = c.getContext('2d');
  c.setAttribute('width', 1920);
  c.setAttribute('height', 1080);
  
  var a = tmp.audio = document.createElement('audio');
  document.body.appendChild(a);
  a.src = "../../music/tam-x04.mp3";
  if(a.play)
    a.play();
  else
    console.log('[WARNING] Audio is n/a on your Environment. it cannot play BGM.');
  
  var indicate = function(k, v, va, o){
    wrp.log_push('indicate',this);
    var ei = document.getElementById('indicator');
    var ek = document.getElementById('indicator_key');
    var ev = document.getElementById('indicator_value');
    if(typeof o === typeof 0)
      ei.style.opacity = o;
    if(k)
      ek.innerHTML = k;
    if(v)
      ev.innerHTML = v;
    if(va)
      ev.style.textAlign = va;
    wrp.log_pop();
  };
  
  var n = new wrp.nacl();
  tmp.n = n;
  tmp.fps_data = [];
  for(var m = 256; m; --m)
    tmp.fps_data.push(0);
  tmp.last_ftp = 0;
  tmp.score = 0;
  
  n.message = function(a){
    var m = JSON.parse(a.data);
    if(m.type == 'updated'){
      var now = new Date();
      var dt  = now - tmp.main_loop_lastcall_time;
      tmp.main_loop_lastcall_time = now;
      var c = tmp.context;
      var rx = 160;
      var ry =  90;
      var w = 1920 / rx;
      var h = 1080 / ry;
      for(var i in m.data){
        var p = m.data[i];
        var y = (i % ry) * h;
        var x = (Math.floor(i / ry)) * w;
        c.fillStyle = 'rgb(' + p[0] + ',' + p[1] + ',' + p[2] + ')';
        c.fillRect(x, y, w, h);
      }
      tmp.info_text_buffer = [
        'C++ Advent Calendar 2012 / 4th day : Native-client vs. HTML5',
        ' by Usagi Ito, BGM by TAM Music Factory. Thanks.',
        '',
        '[Native-client version]'
      ];
      var t = 'SCORE: ' + tmp.score.toFixed(0) + ' ';
      if(tmp.score_fixed)
        t += '(RESULT FIXED!)';
      else{
        var last = Math.max(0, 128000 - (new Date() - tmp.begin_time));
        t += '(LAST ' + last + ' [ms])'
      }
      tmp.info_text_buffer.push(t);
      var data = tmp.fps_data;
      data.shift();
      data.push(1000/dt);
      var average = 0;
      for(var i in data)
        average += data[i];
      tmp.last_fps = average /= data.length;
      tmp.info_text_buffer.push('FPS: ' + average.toFixed(3));
      c.font        = '20px monospace';
      c.fillStyle   = 'gray';
      var b = tmp.info_text_buffer;
      var w = 1920 - 20;
      for(var i in b){
        var y = 40 + 20 * i;
        c.fillText  (b[i] + '\n', 20, y, w);
      }
      tmp.n.post_message("update");
    }
  };
  n.load_start = function(){
    indicate('LOADING', '', 'center', 1);
  };
  n.load_end = function(){
    indicate('LOADING', 'DONE', 'center', 0);
    tmp.begin_time = tmp.main_loop_lastcall_time = new Date();
    tmp.n.post_message("update");
    tmp.timer_score_id = setInterval(function(){
      tmp.score += tmp.last_fps;
      if(new Date() - tmp.begin_time > 128000){
        clearInterval(tmp.timer_score_id);
        tmp.score_fixed = true;
      }
    }, 1000);
  };
  n.loading = function(a){
    if(!this.loading_chars){
      this.loading_chars = ['-','\\','|','/'];
      this.loading_char_index = 0;
    }
    this.loading_char_index = ++this.loading_char_index % this.loading_chars.length;
    indicate('LOADING', this.loading_chars[this.loading_char_index], 'center', 1);
  };
  n.error = function(){ indicate('OOPS!','ERROR','center',1); };
  n.abort = function(){ indicate('OOPS!','ABORT','center',1); };
  n.crash = function(){ indicate('OOPS!','CRASH','center',1); };
  n.initialize();
  

  /*
  var nmf;
  var nmf_getter = new XMLHttpRequest();
  nmf_getter.open('GET', 'cppac2012_4th.nmf', true);
  nmf_getter.onreadystatechange = function(){
    wrp.log_push('onreadystatechange', this);
    if(this.readyState === 4){
      wrp.log('status: ' + this.status);
      if(this.status >= 200 && this.status < 300){
        nmf = eval('(' + this.responseText + ')');
        wrp.log(nmf);
        n.initialize();
      }else{
        wrp.log('oops!');
      }
    }
    wrp.log_pop();
  };
  nmf_getter.send(null);
  */
};

window.addEventListener('load', main, false);

