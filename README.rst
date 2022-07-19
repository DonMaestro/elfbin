Elfbin
======

elfbin is converter from elf(Executable and Linkable Format)
to ASCII file with values represented in hex.

.. code-block:: bash

   $ ./elfbin --help

Example
-------

.. code-block::

   $ riscv32-elf-objdump -d test

   test:     file format elf32-littleriscv


   Disassembly of section .text:

   00000000 <_start>:
      0:00002137          luisp,0x2
      4:00500793          lia5,5
      8:00100813          lia6,1
      c:010787b3          adda5,a5,a6
     10:410787b3          suba5,a5,a6
     14:00f02023          swa5,0(zero) # 0 <_start>
     18:fe9ff06f          j0 <_start>

Result

.. code-block::

   $ ./elfbin -f ./test
   $ cat a.out
   // 0x00000000: _start
   00002137
   00500793
   00100813
   010787b3
   410787b3
   00f02023
   fe9ff06f


Build
-----

.. code-block:: bash

   $ git clone https://github.com/DonMaestro/elfbin.git
   $ cd elfbin
   $ make build

