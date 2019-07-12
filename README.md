# TritonCTS LEF/DEF Helper

- This helper reads lef/def with clock pin names and generates \*.cts (clockpin info file) and \*.blk (macro info file). 

### Required
* GCC compiler and libstdc++ static library >= 4.8.5
* Recommended OS: Centos6 or Centos7 or Ubuntu 16.04

## How To Compile
    $ git clone --recursive https://github.com/mgwoo/CtsHelper.git
    $ cd CtsHelper
    $ mkdir build
    $ cd build 
    $ cmake ..             // you may specify -DCMAKE_INSTALL_PREFIX to avoid installing in /usr/local/*
    $ make
    $ sudo make install    // or make install if you specified -DCMAKE_INSTALL_PREFIX

### How To Execute
    $ ./lefdef2cts -lef tech.lef -lef macro.lef [-lef ... ] -def input.def -cts design.cts -blk -design.blk
    
### Parent Repository
  - https://github.com/The-OpenROAD-Project/TritonCTS
