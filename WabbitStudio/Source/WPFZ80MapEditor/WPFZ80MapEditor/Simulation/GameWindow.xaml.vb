Option Explicit On

Imports WabbitemuLib
Imports SPASM
Imports System.Threading

Public Class GameWindow

    Public Model As GameModel

    Private Calc As New Wabbitemu
    Private Asm As New Z80Assembler

    Private Sub LaunchApp(Name As String)
        Const ProgToEdit = &H84BF
        Const RamCode = &H8100

        Dim AppCode() As Byte = {&HEF, &HD3, &H48, &HEF, &H51, &H4C}
        Dim NameBytes = System.Text.Encoding.ASCII.GetBytes(Name)
        Calc.Break()
        Calc.Memory.Write(ProgToEdit, NameBytes)
        Calc.Memory.Write(RamCode, AppCode)
        Calc.CPU.Halt = False
        Calc.CPU.PC = RamCode
        Calc.Run()
    End Sub

    Private Sub Window_Loaded(sender As Object, e As RoutedEventArgs)
        Asm.CurrentDirectory = "C:\users\spencer\desktop\zelda"

        Asm.InputFile = "zelda_all.asm"
        Asm.OutputFile = "C:\users\spencer\desktop\zelda\zelda.8xk"

        Asm.IncludeDirectories.Add("defaults")
        Asm.IncludeDirectories.Add("images")
        Asm.IncludeDirectories.Add("maps")
        Asm.IncludeDirectories.Add("scripts")

        Asm.Assemble()
        Debug.Write(Asm.StdOut.ReadAll())

        Calc.LoadFile("C:\Users\Spencer\Documents\test.sav")
        Calc.LoadFile(Asm.OutputFile)

        Calc.Run()

        Calc.Keypad.PressKey(Calc_Key.KEY_ON)
        Thread.Sleep(200)
        Calc.Keypad.ReleaseKey(Calc_Key.KEY_ON)

        LaunchApp("Zelda   ")

        Model = New GameModel(Asm, Calc)

        For Each Define As String In SPASMHelper.Assembler.Defines
            If Define.ToUpper() Like "*_GFX" Then
                Dim Address As UShort = Asm.Labels(Define.ToUpper()) And &HFFFF
                If Not Model.ImageMap.ContainsKey(Address) Then
                    Model.ImageMap.Add(Address, Scenario.Instance.Images(SPASMHelper.Assembler.Defines(Define)).Image)
                End If
            End If
        Next


        Dim ZeldaApp As TIApplication = Nothing
        For Each App As TIApplication In Calc.Apps
            If App.Name Like "Zelda*" Then
                ZeldaApp = App
                Exit For
            End If
        Next

        Dim Page As Byte = Asm.Labels("SORT_DONE") / &H10000

        Calc.Break()

        Dim SortDone As New CalcAddress
        SortDone.Initialize(Calc.Memory.Flash(ZeldaApp.Page.Index - Page), Asm.Labels("SORT_DONE") And &HFFFF)
        Calc.Breakpoints.Add(SortDone)

        AddHandler Calc.Breakpoint, AddressOf Calc_Breakpoint

        Calc.Run()
        Calc.Visible = True

        DataContext = Model
    End Sub

    Private Sub Calc_Breakpoint(Calc As Wabbitemu, Breakpoint As IBreakpoint)
        Model.UpdateModel(Asm, Calc)
        Calc.Step()
        Calc.Run()
    End Sub
End Class
