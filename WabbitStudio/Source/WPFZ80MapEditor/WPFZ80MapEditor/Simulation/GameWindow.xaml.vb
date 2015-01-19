Option Explicit On

Imports WabbitemuLib
Imports SPASM
Imports System.Threading

Public Class GameWindow

    Public Model As GameModel

    Public Property Scenario() As Scenario

    Private Sub Window_Loaded(sender As Object, e As RoutedEventArgs)
        Model = New GameModel(Scenario)
        Model.Start()
        DataContext = Model
    End Sub

    Private Sub Window_KeyDown(sender As Object, e As KeyEventArgs)
        If e.Key = Key.LeftAlt Then
            Exit Sub
        End If
        '_Calc.Keypad.PressVirtKey(KeyInterop.VirtualKeyFromKey(e.Key))
    End Sub

    Private Sub Window_KeyUp(sender As Object, e As KeyEventArgs)
        If e.Key = Key.LeftAlt Then
            Exit Sub
        End If
        '_Calc.Keypad.ReleaseVirtKey(KeyInterop.VirtualKeyFromKey(e.Key))
    End Sub
End Class
