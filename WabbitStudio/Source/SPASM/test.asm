#define object3 34
#define index concat("",3)



#define x concat("object",index)

#define object 32+(

#define object.x 4

#macro zobjects(z)
  #define objectname concat(object,z,"*2)")
  #define result eval(objectname)
  result
#endmacro

#macro objects(a)
  #define v eval(zobjects(a))
  v
#endmacro

#macro .x
  #ifdef zlastobj
    #define res 2
  #endif
  res
#endmacro

#macro obj()
  #define zobj 33
  0
#endmacro

#define tobj() eval(obj())

#define z(a) test(a)+
#define .object_x 4

#define TEST_SLOT 6

 ld (z(TEST_SLOT).object_x),a

.echo zobj

#macro test(n)
  #define result eval($4000 + (n * 10))
  result
#endmacro
