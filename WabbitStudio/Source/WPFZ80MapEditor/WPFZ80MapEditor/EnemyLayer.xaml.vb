Public Class EnemyLayer
    Implements IMapLayer

    Public Shared ReadOnly ImageProperty As DependencyProperty =
        DependencyProperty.RegisterAttached("Image", GetType(ImageSource), GetType(ObjectLayer),
                                            New PropertyMetadata(Nothing, AddressOf ImagePropertyChanged))

    Public Property Scenario As Scenario

    Private _Active As Boolean

    Public Sub New()

        ' This call is required by the designer.
        InitializeComponent()

        ' Add any initialization after the InitializeComponent() call.
        Height = LayerContainer.TileSize.Height * LayerContainer.MapSize.Height
        Width = LayerContainer.TileSize.Width * LayerContainer.MapSize.Width
    End Sub

    Private Shared Sub ImagePropertyChanged(d As DependencyObject, e As DependencyPropertyChangedEventArgs)
        Dim objectLayer As ObjectLayer = d
        'TODO: replacement for ImageIndexConverter
        'Return objectLayer.Scenario.Images(Index).Image
    End Sub

    Public WriteOnly Property Active As Boolean Implements IMapLayer.Active
        Set(value As Boolean)
            _Active = value
            IsHitTestVisible = value
            DeselectAll()
        End Set
    End Property

    Public Sub DeselectAll() Implements IMapLayer.DeselectAll
        ItemsControl.SelectedItems.Clear()
    End Sub

    Private Sub ItemsControl_MouseDoubleClick(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs) Handles ItemsControl.MouseDoubleClick
        Dim Frm = New ObjectProperties()
        Frm.Owner = Window.GetWindow(Me)

        Dim ObjClone As ZEnemy = ItemsControl.SelectedItem.Clone
        Frm.DataContext = ObjClone
        If Frm.ShowDialog() = True Then
            ObjClone.UpdatePosition(ObjClone.Args(0).Value, ObjClone.Args(1).Value)
            Me.DataContext.ZEnemies(ItemsControl.SelectedIndex) = ObjClone
            ItemsControl.SelectedItem = ObjClone
        End If
        _IsDraggingObjects = False
        Mouse.Capture(Nothing)
    End Sub


    Private _IsDraggingObjects As Boolean = False

    Private Sub Object_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        Dim Obj As FrameworkElement = sender
        Dim ZObj = Obj.DataContext

        Obj.CaptureMouse()
        _StartDrag = e.GetPosition(Me)
        If Not ItemsControl.SelectedItems.Contains(ZObj) Then
            DeselectAll()
            ItemsControl.SelectedItems.Add(ZObj)
        End If

        For Each ZObj2 In ItemsControl.SelectedItems
            ObjectLayer.SetLeft(ZObj2, ZObj2.X)
            ObjectLayer.SetTop(ZObj2, ZObj2.Y)
        Next
        _IsDraggingObjects = True
        e.Handled = True
    End Sub

    Private Sub Object_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        Dim Obj As FrameworkElement = sender
        Dim ZObj = Obj.DataContext

        Obj.ReleaseMouseCapture()
        _IsDraggingObjects = False
        e.Handled = True
    End Sub

    Private Sub Object_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If _IsDraggingObjects Then
            Dim CurPoint As Point = e.GetPosition(Me)
            Dim DragDelta = CurPoint - _StartDrag

            _StartDrag = CurPoint
            For Each ZObj In ItemsControl.SelectedItems

                ObjectLayer.SetLeft(ZObj, ObjectLayer.GetLeft(ZObj) + DragDelta.X)
                ObjectLayer.SetTop(ZObj, ObjectLayer.GetTop(ZObj) + DragDelta.Y)

                ZObj.UpdatePosition(ObjectLayer.GetLeft(ZObj), ObjectLayer.GetTop(ZObj))
            Next
            e.Handled = True
        End If
    End Sub

#Region "Canvas events"
    Private _StartDrag As New Point
    Private Sub ObjectCanvas_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        sender.CaptureMouse()

        _StartDrag = e.GetPosition(EnemyCanvas)

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
            Dim CurPoint = e.GetPosition(EnemyCanvas)
            CurPoint.X = Math.Max(0, Math.Min(Me.ActualWidth, CurPoint.X))
            CurPoint.Y = Math.Max(0, Math.Min(Me.ActualHeight, CurPoint.Y))

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
        ItemsControl.SelectedItems.Clear()

        For Each ZObj As ZEnemy In CType(EnemyCanvas.DataContext, MapData).ZEnemies
            Dim IntersectResult As Rect = Rect.Intersect(ZObj.Bounds, SelRect)
            If Not IntersectResult.IsEmpty() Then
                If (IntersectResult.Width * IntersectResult.Height) > (ZObj.Bounds.Width * ZObj.Bounds.Height * 0.75) Then
                    ItemsControl.SelectedItems.Add(ZObj)
                End If
            End If
        Next
    End Sub
#End Region
End Class
