Set Calc = CreateObject("Wabbit.Wabbitemu")

WScript.Echo "Attach now"

Calc.LoadFile "C:\Users\Spencer\Documents\test.sav"

WScript.Echo Calc.Memory.Bank(1).Index
WScript.Echo Calc.Memory.Bank(2).Index
WScript.Echo Calc.Memory.Bank(3).Index
WScript.Echo Calc.Memory.Bank(4).Index
