Set Calc = CreateObject("Wabbit.Wabbitemu")

Calc.LoadFile "C:\Users\Spencer\Documents\test.sav"

'WScript.Echo UBound(Calc.LCD.Display)
'WScript.Echo Calc.CPU.A
Calc.Running = True
Calc.Visible = True
WScript.Sleep 2000
Calc.Visible = False
Calc.Running = False

'WScript.Echo Calc.CPU.A

