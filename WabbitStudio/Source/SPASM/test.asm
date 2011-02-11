#macro errortest()
  10+
#endmacro
#define err2 24a
#define err3 "test"

 .db errortest + 1, err2, err3