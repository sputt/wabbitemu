Public Class MiscLayer
    Implements IMapLayer

    Protected ReadOnly Property Map As MapData
        Get
            Return DirectCast(DataContext, MapData)
        End Get
    End Property

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

    Private _IsActive As Boolean

    Private Sub UserControl_MouseDown(sender As Object, e As MouseButtonEventArgs)
        If e.ChangedButton = MouseButton.Left Then
            DeselectAll()
        End If
    End Sub

    Private Sub Misc_Drop(sender As Object, e As DragEventArgs)
        Dim Pos = e.GetPosition(sender)
        Dim Def As ZDef = e.Data.GetData("WPFZ80MapEditor.ZDef")

        Dim Args As New List(Of Object)

        Args.Add(CInt(Math.Round(Pos.X)))
        Args.Add(CInt(Math.Round(Pos.Y)))
        Args.Add(32)
        Args.Add(32)

        Dim Misc As ZMisc = ZMisc.FromDef(Def, Args)
        If Misc IsNot Nothing Then
            Map.ZMisc.Add(Misc)
            ItemsControl.SelectedItem = Misc
            ItemsControl.Focus()
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

        ZMisc.PreviousVersion = ZMisc.Clone()

        _IsDraggingMisc = True
        e.Handled = True
    End Sub

    Private Sub Misc_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If _IsDraggingMisc Then
            Dim CurPoint As Point = e.GetPosition(Me)
            Dim DragDelta = CurPoint - _StartDrag

            Dim NewX = SPASMHelper.Eval(ItemsControl.SelectedValue.PreviousVersion.Args(0).Value) + DragDelta.X
            Dim NewY = SPASMHelper.Eval(ItemsControl.SelectedValue.PreviousVersion.Args(1).Value) + DragDelta.Y

            ItemsControl.SelectedValue.UpdatePosition(NewX, NewY)

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

    Public Overrides ReadOnly Property LayerType As LayerType
        Get
            Return WPFZ80MapEditor.LayerType.MiscLayer
        End Get
    End Property

End Class
