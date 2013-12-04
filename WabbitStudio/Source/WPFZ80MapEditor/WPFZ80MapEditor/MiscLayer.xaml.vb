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
        DependencyProperty.RegisterAttached("SelectionOpacity", GetType(Double), GetType(MiscLayer), New UIPropertyMetadata(CDbl(0.6)))

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
        ItemsControl.SelectedItem = Nothing
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

        ItemsControl.SelectedItem = ZMisc
        ItemsControl.Focus()

        MiscLayer.SetLeft(ZMisc, ZMisc.X)
        MiscLayer.SetTop(ZMisc, ZMisc.Y)

        _IsDraggingMisc = True
        e.Handled = True
    End Sub

    Private Sub Misc_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If _IsDraggingMisc Then
            Dim CurPoint As Point = e.GetPosition(Me)
            Dim DragDelta = CurPoint - _StartDrag

            Dim NewX = MiscLayer.GetLeft(ItemsControl.SelectedValue) + DragDelta.X
            Dim NewY = MiscLayer.GetTop(ItemsControl.SelectedValue) + DragDelta.Y

            ItemsControl.SelectedValue.UpdatePosition(NewX, NewY, False)

            SetValue(SelectionOpacityProperty, 0.25)

            e.Handled = True
        End If
    End Sub

    Private Sub ItemsControl_KeyDown(sender As Object, e As Windows.Input.KeyEventArgs) Handles ItemsControl.KeyDown
        Dim SelectedMisc As ZMisc = CType(sender, ListBox).SelectedItem
        If SelectedMisc IsNot Nothing Then
            Select Case e.Key
                Case Key.Delete
                    CType(Me.DataContext, MapData).ZMisc.Remove(SelectedMisc)
            End Select
        Else
            e.Handled = False
        End If
    End Sub

    Private Sub ItemsControl_MouseDoubleClick(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs) Handles ItemsControl.MouseDoubleClick
        Dim Frm = New ObjectProperties()
        Frm.Owner = Window.GetWindow(Me)

        Dim ObjClone As ZMisc = ItemsControl.SelectedItem.Clone
        Frm.DataContext = ObjClone
        If Frm.ShowDialog() = True Then
            'ObjClone.UpdatePosition(ObjClone.Args(0).Value, ObjClone.Args(1).Value)
            Me.DataContext.ZMisc(ItemsControl.SelectedIndex) = ObjClone
            ItemsControl.SelectedItem = ObjClone
        End If
        _IsDraggingMisc = False
        Mouse.Capture(Nothing)
    End Sub

    'Private Sub ItemsControl_PreviewKeyDown(sender As System.Object, e As System.Windows.Input.KeyEventArgs) Handles ItemsControl.PreviewKeyDown
    '    Dim SelectedMisc As ZMisc = CType(sender, ListBox).SelectedItem
    '    Debug.WriteLine("Misc previewkeydown")
    '    If SelectedMisc IsNot Nothing Then
    '        Select Case e.Key
    '            Case Key.Delete
    '                CType(Me.DataContext, MapData).ZMisc.Remove(SelectedMisc)
    '        End Select
    '    Else
    '        e.Handled = False
    '    End If
    'End Sub

End Class
