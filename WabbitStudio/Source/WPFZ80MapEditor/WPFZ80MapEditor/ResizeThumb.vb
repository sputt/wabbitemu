Imports System.Windows.Controls.Primitives

Public Class ResizeThumb
    Inherits Panel

    Public Sub New()
        AddHandler Me.MouseLeftButtonDown, AddressOf Drag_MouseLeftButtonDown
        AddHandler Me.MouseLeftButtonUp, AddressOf Drag_MouseLeftButtonUp
        AddHandler Me.MouseMove, AddressOf Drag_MouseMove

        Background = WPFZ80MapEditor.Application.Current.Resources("SelectionBrush")
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

        Misc.PreviousVersion = Misc.Clone()

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

            If Misc.PreviousVersion Is Nothing Then Exit Sub

            Dim PrevX = SPASMHelper.Eval(Misc.PreviousVersion.Args(0).Value)
            Dim PrevY = SPASMHelper.Eval(Misc.PreviousVersion.Args(1).Value)
            Dim PrevW = SPASMHelper.Eval(Misc.PreviousVersion.Args(2).Value)
            Dim PrevH = SPASMHelper.Eval(Misc.PreviousVersion.Args(3).Value)

            Dim CurX = SPASMHelper.Eval(Misc.Args(0).Value)
            Dim CurY = SPASMHelper.Eval(Misc.Args(1).Value)
            Dim CurW = SPASMHelper.Eval(Misc.Args(2).Value)
            Dim CurH = SPASMHelper.Eval(Misc.Args(3).Value)

            If VerticalAlignment = Windows.VerticalAlignment.Bottom Then
                CurH = PrevH + VDelta
                Misc.UpdatePosition(CurX, PrevY, CurW, CurH)
            ElseIf VerticalAlignment = Windows.VerticalAlignment.Top Then
                CurY = PrevY + VDelta
                CurH = PrevH - VDelta
                Misc.UpdatePosition(CurX, CurY, CurW, CurH)
            End If

            If HorizontalAlignment = Windows.HorizontalAlignment.Right Then
                Misc.UpdatePosition(PrevX, CurY, PrevW + HDelta, CurH)
            ElseIf HorizontalAlignment = Windows.HorizontalAlignment.Left Then
                Misc.UpdatePosition(PrevX + HDelta, CurY, PrevW - HDelta, CurH)
            End If

            Layer.SetValue(MiscLayer.SelectionOpacityProperty, 0.25)
            e.Handled = True
        End If
    End Sub
End Class
