#macro Macro1()
  1
#endmacro

#macro RecursiveMacro1(x)
  #if x > 1
    1 + RecursiveMacro1(x - 1)
  #else
    1
  #endif
#endmacro

#define InfiniteRecursiveDefine InfiniteRecursiveDefine + 1

#macro FibonacciMacro(x)
  #if x = 0
    0
  #else
    #if x = 1
      1
    #else
      FibonacciMacro(x - 1) + FibonacciMacro(x - 2)
    #endif
  #endif
#endmacro

#macro EchoMacro1()
  .echo >> EchoMacro1.txt "Hello World"
#endmacro

#macro EchoMacro2(x)
  .echo >> EchoMacro2.txt x
#endmacro

#macro EchoMacro3(x1, x2)
  .echo >> EchoMacro3.txt "My prefix (", FibonacciMacro(x1), "): ", x2
#endmacro

#macro EchoMacro4(x1, x2)
  .echo >> EchoMacro4.txt "My prefix (", RecursiveMacro1(x1), "): ", x2
#endmacro

#define .echomacro4 EchoMacro4(

#macro ConcatMacro1(x1)
  #define x2 concat("\"",x1,"\"")
  .echo >> ConcatMacro1.txt x2
#endmacro

#macro ConcatMacro2(x1)
  #define x2 concat("\"",x1,"\"")
  .echo >> ConcatMacro2.txt x2
#endmacro


#macro InternalConcatMacro3(string, times)
  #if times = 0
    .echo >> ConcatMacro3.txt string
    #undef gfirst
  #else
    #define string concat(string, ",\"X\"")
    InternalConcatMacro3(string, times - 1)
  #endif
#endmacro

#macro ConcatMacro3(x1)
  InternalConcatMacro3("\"\"", x1)
#endmacro


#macro InternalConcatMacro4(string, times)
  #if times = 0
    .echo >> ConcatMacro4.txt string
  #else
    #define string string,"X"
    InternalConcatMacro4(string, times - 1)
  #endif
#endmacro


#macro ConcatMacro4(x1)
  InternalConcatMacro4("", x1)
#endmacro


#macro Fixed14Power(val, pow)
  #if pow = 0
    16384
  #else
    #define prev_val eval(Fixed14Power(val, pow - 1))
    val * prev_val / 16384
  #endif
#endmacro

#macro DegreesToRadians(deg)
  (deg * 16384 / 180 * 51472 / 16384)
#endmacro

#macro Fixed14Cos(deg)
  #define rad eval(DegreesToRadians(deg))
  #define rad2 eval(rad * 16384 / 20643)
  16384 - (Fixed14Power(rad, 2)/2) + (Fixed14Power(rad, 4)/24) - ((Fixed14Power(rad2, 6)/720) * 4)
#endmacro

#macro LocalLabel1()
  .org 0
  .block 3
_:
#endmacro

#macro LocalLabel3()
  .org 0
  .block 3
_:
  .block 2
_:
#endmacro

#macro LocalLabel4()
  .org 0
_:
  .block 2
  .db -_ + +_ + ++_
_:
  .block 1
_:
#endmacro

#macro ParserError102(x)
  x
#endmacro

#define EmitDefine1 10
#define EmitDefine2 10,11,12

#macro EmitMacro3(arg1, arg2)
  arg1, arg2, 12
#endmacro
#define EmitDefine3 EmitMacro3(10, 11)

#define EmitDefine4 10+
#define EmitDefine5 "A"
