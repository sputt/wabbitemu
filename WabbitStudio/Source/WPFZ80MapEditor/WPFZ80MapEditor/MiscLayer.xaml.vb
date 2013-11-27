Public Class MiscLayer
    Implements IMapLayer

    Public Shared ReadOnly LeftProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("Left", GetType(Double), GetType(MiscLayer), New UIPropertyMetadata(CDbl(-1)))

    Public Shared ReadOnly TopProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("Top", GetType(Double), GetType(MiscLayer), New UIPropertyMetadata(CDbl(-1)))

    Public Shared ReadOnly MiscWidthProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("Width", GetType(Double), GetType(MiscLayer), New UIPropertyMetadata(CDbl(-1)))

    Public Shared ReadOnly MiscHeightProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("Height", GetType(Double), GetType(MiscLayer), New UIPropertyMetadata(CDbl(-1)))

    Public Shared ReadOnly SelectionOpacityProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("SelectionOpacity", GetType(Double), GetType(MiscLayer), New UIPropertyMetadata(CDbl(1.0)))

    Public Shared Sub SetLeft(d As DependencyObject, value As Double)
        d.SetValue(LeftProperty, value)
    End Sub

    Public Shared Function GetLeft(d As DependencyObject) As Double
        Return d.GetValue(LeftProperty)
    End Function

    Public Shared Sub SetTop(d As DependencyObject, value As Double)
        d.SetValue(TopProperty, value)
    End Sub

    Public Shared Function GetTop(d As DependencyObject) As Double
        Return d.GetValue(TopProperty)
    End Function

    Private _IsActive As Boolean

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.
        Height = LayerContainer.TileSize.Height * LayerContainer.MapSize.Height
        Width = LayerContainer.TileSize.Width * LayerContainer.MapSize.Width
    End Sub

    Public WriteOnly Property Active As Boolean Implements IMapLayer.Active
        Set(value As Boolean)
            _IsActive = value
            IsHitTestVisible = value
            DeselectAll()
        End Set
    End Property

    Public Sub DeselectAll() Implements IMapLayer.DeselectAll
        ItemsControl.SelectedItems.Clear()
    End Sub

    Private Sub UserControl_MouseDown(sender As Object, e As MouseButtonEventArgs)
        If e.ChangedButton = MouseButton.Left Then
            DeselectAll()
        End If
    End Sub

    Private _IsDraggingMisc As Boolean = False
    Private _StartDrag As New Point

    Private Sub Misc_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        Dim Obj As FrameworkElement = sender

        Obj.ReleaseMouseCapture()
        _IsDraggingMisc = False
        e.Handled = True

        SetValue(SelectionOpacityProperty, 0.6)
    End Sub

    Private Sub Misc_MouseLeftButtonDown(sender As Object, e As MouseButtonEventArgs)
        Dim Obj As FrameworkElement = sender
        Dim ZMisc As ZMisc = Obj.DataContext

        Obj.CaptureMouse()
        _StartDrag = e.GetPosition(Me)
        If Not ItemsControl.SelectedItems.Contains(ZMisc) Then
            DeselectAll()
            ItemsControl.SelectedItems.Add(ZMisc)
        End If

        For Each ZMisc2 As ZMisc In ItemsControl.SelectedItems
            MiscLayer.SetLeft(ZMisc2, ZMisc2.X)
            MiscLayer.SetTop(ZMisc2, ZMisc2.Y)
        Next
        _IsDraggingMisc = True
        e.Handled = True
    End Sub

    Private Sub Misc_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If _IsDraggingMisc Then
            Dim CurPoint As Point = e.GetPosition(Me)
            Dim DragDelta = CurPoint - _StartDrag

            '_StartDrag = CurPoint
            For Each ZMisc As ZMisc In ItemsControl.SelectedItems
                Dim NewX = MiscLayer.GetLeft(ZMisc) + DragDelta.X
                Dim NewY = MiscLayer.GetTop(ZMisc) + DragDelta.Y

                ZMisc.UpdatePosition(NewX, NewY, False)
            Next

            SetValue(SelectionOpacityProperty, 0.25)

            e.Handled = True
        End If
    End Sub
End Class
