Option Explicit On

Imports WabbitemuLib
Imports SPASM
Imports System.Threading

Public Class GameWindow

    Public Model As GameModel

    Private ReadOnly _Calc As New Wabbitemu
    Private ReadOnly _Asm As New Z80Assembler

    Public Property Scenario() As Scenario


    Private Sub LaunchApp(Name As String)
        Const ProgToEdit = &H84BF
        Const RamCode = &H8100

        Dim AppCode() As Byte = {&HEF, &HD3, &H48, &HEF, &H51, &H4C}
        Dim NameBytes = System.Text.Encoding.ASCII.GetBytes(Name)
        _Calc.Break()
        _Calc.Memory.Write(ProgToEdit, NameBytes)
        _Calc.Memory.Write(RamCode, AppCode)
        _Calc.CPU.Halt = False
        _Calc.CPU.PC = RamCode
        _Calc.Run()
    End Sub

    Private Sub Window_Loaded(sender As Object, e As RoutedEventArgs)
        _Asm.CurrentDirectory = MainWindow.ZeldaFolder

        _Asm.InputFile = "zelda_all.asm"
        _Asm.OutputFile = IO.Path.Combine(MainWindow.ZeldaFolder, "zelda.8xk")

        _Asm.IncludeDirectories.Add("defaults")
        _Asm.IncludeDirectories.Add("images")
        _Asm.IncludeDirectories.Add("maps")
        _Asm.IncludeDirectories.Add("scripts")

        _Asm.Assemble()
        Debug.Write(_Asm.StdOut.ReadAll())

        _Calc.LoadFile(MainWindow.RomPath)
        _Calc.LoadFile(_Asm.OutputFile)

        _Calc.Run()

        _Calc.Keypad.PressKey(CalcKey.KEY_ON)
        Thread.Sleep(200)
        _Calc.Keypad.ReleaseKey(CalcKey.KEY_ON)

        LaunchApp("Zelda   ")

        Model = New GameModel(Scenario, _Asm, _Calc)

        For Each Define As String In SPASMHelper.Assembler.Defines
            Dim DefineKey As String = Define.ToUpper()
            If DefineKey Like "*_GFX?" Or DefineKey Like "*_GFX" Then
                Dim Address As UShort = _Asm.Labels(Define.ToUpper()) And &HFFFF
                If Not Model.ImageMap.ContainsKey(Address) Then
                    Model.ImageMap.Add(Address, SPASMHelper.Assembler.Defines(Define))
                End If
            End If
        Next


        Dim ZeldaApp As ITIApplication = Nothing
        For Each App As ITIApplication In _Calc.Apps
            If App.Name Like "Zelda*" Then
                ZeldaApp = App
                Exit For
            End If
        Next

        Dim Page As Byte = _Asm.Labels("SORT_DONE") / &H10000

        _Calc.Break()

        Dim SortDone As New CalcAddress
        SortDone.Initialize(_Calc.Memory.Flash(ZeldaApp.Page.Index - Page), _Asm.Labels("SORT_DONE") And &HFFFF)
        _Calc.Breakpoints.Add(SortDone)

        AddHandler _Calc.Breakpoint, AddressOf Calc_Breakpoint

        _Calc.Run()
        _Calc.Visible = True

        RenderOptions.SetBitmapScalingMode(Me, BitmapScalingMode.NearestNeighbor)

        DataContext = Model
    End Sub

    Private Sub Calc_Breakpoint(Calc As Wabbitemu, Breakpoint As IBreakpoint)
        Model.UpdateModel(_Asm, Calc)
        Calc.Step()
        Calc.Run()
    End Sub

    Private Sub Window_KeyDown(sender As Object, e As KeyEventArgs)
        If e.Key = Key.LeftAlt Then
            Exit Sub
        End If
        _Calc.Keypad.PressVirtKey(KeyInterop.VirtualKeyFromKey(e.Key))
    End Sub

    Private Sub Window_KeyUp(sender As Object, e As KeyEventArgs)
        If e.Key = Key.LeftAlt Then
            Exit Sub
        End If
        _Calc.Keypad.ReleaseVirtKey(KeyInterop.VirtualKeyFromKey(e.Key))
    End Sub
End Class
