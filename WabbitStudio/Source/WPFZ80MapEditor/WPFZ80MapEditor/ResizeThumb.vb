Imports System.Windows.Controls.Primitives

Public Class ResizeThumb
    Inherits Panel

    Public Sub New()
        AddHandler Me.MouseLeftButtonDown, AddressOf Drag_MouseLeftButtonDown
        AddHandler Me.MouseLeftButtonUp, AddressOf Drag_MouseLeftButtonUp
        AddHandler Me.MouseMove, AddressOf Drag_MouseMove

        Background = New SolidColorBrush(Color.FromRgb(&H40, &H40, &HFF))
    End Sub

    Private _IsDraggingMisc As Boolean = False
    Private _StartDrag As New Point


    Private Sub Drag_MouseLeftButtonDown(sender As Object, e As MouseButtonEventArgs)
        Dim Layer = Utils.FindVisualParent(Of MiscLayer)(Me)

        Dim Obj As FrameworkElement = sender
        Dim Misc As ZMisc = Obj.DataContext

        Obj.CaptureMouse()
        _StartDrag = e.GetPosition(Layer)

        _IsDraggingMisc = True

        'Misc.SetValue(MiscLayer.LeftProperty, CDbl(Misc.Args(0).Value))
        'Misc.SetValue(MiscLayer.TopProperty, CDbl(Misc.Args(1).Value))

        'Misc.SetValue(MiscLayer.MiscWidthProperty, CDbl(Misc.Args(2).Value))
        'Misc.SetValue(MiscLayer.MiscHeightProperty, CDbl(Misc.Args(3).Value))
        e.Handled = True
    End Sub

    Private Sub Drag_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        Dim Obj As FrameworkElement = sender

        Obj.ReleaseMouseCapture()
        _IsDraggingMisc = False
        e.Handled = True

        Dim Layer = Utils.FindVisualParent(Of MiscLayer)(Me)
        Layer.SetValue(MiscLayer.SelectionOpacityProperty, 0.6)
    End Sub

    Private Sub Drag_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If _IsDraggingMisc Then
            Dim Layer = Utils.FindVisualParent(Of MiscLayer)(Me)

            Dim CurPoint As Point = e.GetPosition(Layer)
            Dim DragDelta = CurPoint - _StartDrag

            Dim HDelta = DragDelta.X
            Dim VDelta = DragDelta.Y

            Dim Misc As ZMisc = DataContext

            Dim X As Double '= Misc.GetValue(MiscLayer.LeftProperty)
            Dim Y As Double ' = Misc.GetValue(MiscLayer.TopProperty)
            Dim MiscWidth As Double '= Misc.GetValue(MiscLayer.MiscWidthProperty)
            Dim MiscHeight As Double ' = Misc.GetValue(MiscLayer.MiscHeightProperty)

            If VerticalAlignment = Windows.VerticalAlignment.Bottom Then
                MiscHeight += VDelta
            ElseIf VerticalAlignment = Windows.VerticalAlignment.Top Then
                MiscHeight -= VDelta
                Misc.UpdatePosition(Misc.X, Y + VDelta, False)
            End If

            If HorizontalAlignment = Windows.HorizontalAlignment.Right Then
                MiscWidth += HDelta
            ElseIf HorizontalAlignment = Windows.HorizontalAlignment.Left Then
                MiscWidth -= HDelta
                Misc.UpdatePosition(X + HDelta, Misc.Y, False)
            End If

            Misc.Args(2).Value = Math.Round(MiscWidth)
            Misc.Args(3).Value = Math.Round(MiscHeight)

            Layer.SetValue(MiscLayer.SelectionOpacityProperty, 0.25)

            Debug.WriteLine("Width: " & MiscWidth & " (" & HDelta & ") = " & Misc.Args(2).Value)
            e.Handled = True
        End If
    End Sub
End Class
