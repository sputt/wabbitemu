Imports System.ComponentModel

Public Class ObjectLayer
    Implements IMapLayer



    Public Shared ReadOnly LeftProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("Left", GetType(Double), GetType(ObjectLayer), New UIPropertyMetadata(CDbl(-1)))

    Public Shared ReadOnly TopProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("Top", GetType(Double), GetType(ObjectLayer), New UIPropertyMetadata(CDbl(-1)))

    Public Shared ReadOnly ObjectImageProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("ObjectImage", GetType(ImageSource), GetType(ObjectLayer),
                                            New PropertyMetadata(Nothing, Nothing))

    Public Property Scenario As Scenario

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

    Shared Sub New()
        'Dim imagePropertyDescriptor = DependencyPropertyDescriptor.FromName("Image", GetType(ZObject), GetType(ZObject))
        'imagePropertyDescriptor.DependencyProperty.OverrideMetadata(
        '        GetType(ZObject), New PropertyMetadata(-1, AddressOf OnImagePropertyChanged))
    End Sub

    Public Sub DeselectAll() Implements IMapLayer.DeselectAll
        ObjectItemsControl.SelectedItems.Clear()
    End Sub

    Private _StartDrag As New Point
#Region "Object events"
    Private _IsDraggingObjects As Boolean = False

    Private Sub Object_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        Dim Obj As FrameworkElement = sender
        Dim ZObj As ZObject = Obj.DataContext

        Obj.CaptureMouse()
        _StartDrag = e.GetPosition(Me)
        If Not ObjectItemsControl.SelectedItems.Contains(ZObj) Then
            DeselectAll()
            ObjectItemsControl.SelectedItems.Add(ZObj)
        End If

        For Each ZObj2 As ZObject In ObjectItemsControl.SelectedItems
            'ObjectLayer.SetLeft(ZObj2, ZObj2.X)
            'ObjectLayer.SetTop(ZObj2, ZObj2.Y)
        Next
        _IsDraggingObjects = True
        e.Handled = True
    End Sub

    Private Sub Object_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        Dim Obj As FrameworkElement = sender
        Dim ZObj As ZObject = Obj.DataContext

        Obj.ReleaseMouseCapture()
        _IsDraggingObjects = False
        e.Handled = True
    End Sub

    Private Sub Object_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If _IsDraggingObjects Then
            Dim CurPoint As Point = e.GetPosition(Me)
            Dim DragDelta = CurPoint - _StartDrag

            _StartDrag = CurPoint
            For Each ZObj As ZObject In ObjectItemsControl.SelectedItems

                'ObjectLayer.SetLeft(ZObj, ObjectLayer.GetLeft(ZObj) + DragDelta.X)
                'ObjectLayer.SetTop(ZObj, ObjectLayer.GetTop(ZObj) + DragDelta.Y)

                'ZObj.UpdatePosition(ObjectLayer.GetLeft(ZObj), ObjectLayer.GetTop(ZObj))
            Next
            e.Handled = True
        End If
    End Sub
#End Region

#Region "Canvas events"
    Private Sub ObjectCanvas_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        sender.CaptureMouse()

        _StartDrag = Mouse.GetPosition(ObjectCanvas)

        SelectionRect.Width = 0
        SelectionRect.Height = 0
        SelectionRect.Visibility = Windows.Visibility.Visible

        DeselectAll()

        e.Handled = True
    End Sub

    Private Sub ObjectCanvas_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        sender.ReleaseMouseCapture()
        SelectionRect.Visibility = Windows.Visibility.Hidden

        e.Handled = True
    End Sub

    Private Sub ObjectCanvas_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If Mouse.Captured Is sender Then
            Dim CurPoint = Mouse.GetPosition(ObjectCanvas)
            CurPoint.X = Math.Max(0, Math.Min(Me.Width, CurPoint.X))
            CurPoint.Y = Math.Max(0, Math.Min(Me.Height, CurPoint.Y))

            Canvas.SetLeft(SelectionRect, Math.Min(CurPoint.X, _StartDrag.X))
            Canvas.SetTop(SelectionRect, Math.Min(CurPoint.Y, _StartDrag.Y))
            SelectionRect.Width = Math.Abs(CurPoint.X - _StartDrag.X)
            SelectionRect.Height = Math.Abs(CurPoint.Y - _StartDrag.Y)

            Dim SelRect As New Rect
            SelRect.X = Canvas.GetLeft(SelectionRect)
            SelRect.Y = Canvas.GetTop(SelectionRect)
            SelRect.Width = SelectionRect.Width
            SelRect.Height = SelectionRect.Height

            SelectObjectsInRect(SelRect)

            e.Handled = True
        End If
    End Sub

    Private Sub SelectObjectsInRect(SelRect As Rect)
        ObjectItemsControl.SelectedItems.Clear()

        For Each ZObj As ZObject In CType(ObjectCanvas.DataContext, MapData).ZObjects
            Dim IntersectResult As Rect = Rect.Intersect(ZObj.Bounds, SelRect)
            If Not IntersectResult.IsEmpty() Then
                If (IntersectResult.Width * IntersectResult.Height) > (ZObj.Bounds.Width * ZObj.Bounds.Height * 0.75) Then
                    ObjectItemsControl.SelectedItems.Add(ZObj)
                End If
            End If
        Next
    End Sub
#End Region

    Private Sub ObjectItemsControl_MouseDoubleClick(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs) Handles ObjectItemsControl.MouseDoubleClick
        Dim Frm = New ObjectProperties()
        Frm.Owner = Window.GetWindow(Me)

        Dim ObjClone As ZObject = ObjectItemsControl.SelectedItem.Clone
        Frm.DataContext = ObjClone
        If Frm.ShowDialog() = True Then
            ObjClone.UpdatePosition(ObjClone.Args(0).Value, ObjClone.Args(1).Value)
            Me.DataContext.ZObjects(ObjectItemsControl.SelectedIndex) = ObjClone
            ObjectItemsControl.SelectedItem = ObjClone
        End If
        _IsDraggingObjects = False
        Mouse.Capture(Nothing)
    End Sub

    Private Sub ObjectItemsControl_PreviewKeyDown(sender As System.Object, e As System.Windows.Input.KeyEventArgs) Handles ObjectItemsControl.PreviewKeyDown
        Dim Objs = New List(Of ZObject)(CType(sender, ListBox).SelectedItems.Cast(Of ZObject))
        Debug.WriteLine("Objects previewkeydown")
        If Objs.Count > 0 Then
            Select Case e.Key
                Case Key.Delete
                    For Each Obj In Objs
                        CType(Me.DataContext, MapData).ZObjects.Remove(Obj)
                    Next
                Case Key.Down
                    For Each Obj In Objs
                        Obj.Jump(0, 1)
                    Next
                Case Key.Left
                    For Each Obj In Objs
                        Obj.Jump(-1, 0)
                    Next
                Case Key.Right
                    For Each Obj In Objs
                        Obj.Jump(1, 0)
                    Next
                Case Key.Up
                    For Each Obj In Objs
                        Obj.Jump(0, -1)
                    Next
            End Select
            e.Handled = True
        Else
            e.Handled = False
        End If
    End Sub

    Private Sub ObjectItemsControl_SelectionChanged(sender As System.Object, e As System.Windows.Controls.SelectionChangedEventArgs) Handles ObjectItemsControl.SelectionChanged
        sender.Focus()
    End Sub

    Public ReadOnly Property LayerType As LayerType Implements IMapLayer.LayerType
        Get
            Return WPFZ80MapEditor.LayerType.ObjectLayer
        End Get
    End Property
End Class
