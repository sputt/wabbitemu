Public Class XObjectLayer
    Inherits MapLayer

    Private ReadOnly Property Map As MapData
        Get
            Return DirectCast(DataContext, MapData)
        End Get
    End Property

    Public Property Tint As Color
        Get
            Return GetValue(TintProperty)
        End Get

        Set(ByVal value As Color)
            SetValue(TintProperty, value)
        End Set
    End Property

    Public Shared ReadOnly TintProperty As DependencyProperty = _
                           DependencyProperty.Register("Tint", _
                           GetType(Color), GetType(XObjectLayer), _
                           New PropertyMetadata(Nothing))


#Region "Canvas events"
    Private _StartDrag As New Point

    Private Sub ObjectCanvas_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        SelectionRect.Width = 0
        SelectionRect.Height = 0

        _StartDrag = Mouse.GetPosition(ObjectCanvas)

        If sender.CaptureMouse() Then
            SelectionRect.Visibility = Windows.Visibility.Visible
            'DeselectAll()
            ObjectListBox.Focus()

            e.Handled = True
        End If
    End Sub

    Private Sub ObjectCanvas_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        If Mouse.Captured Is sender Then
            sender.ReleaseMouseCapture()
            SelectionRect.Visibility = Windows.Visibility.Hidden

            Dim SelRect As New Rect
            SelRect.X = Canvas.GetLeft(SelectionRect)
            SelRect.Y = Canvas.GetTop(SelectionRect)
            SelRect.Width = SelectionRect.Width
            SelRect.Height = SelectionRect.Height

            SelectObjectsInRect(SelRect)

            e.Handled = True
        End If
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
        ObjectListBox.SelectedItems.Clear()



        For Each ZObj As ZObject In CType(Me.DataContext, MapData).ZObjects
            Dim IntersectResult As Rect = Rect.Intersect(ZObj.Bounds, SelRect)
            If Not IntersectResult.IsEmpty() Then
                If (IntersectResult.Width * IntersectResult.Height) > (ZObj.Bounds.Width * ZObj.Bounds.Height * 0.75) Then
                    ObjectListBox.SelectedItems.Add(ZObj)
                ElseIf IntersectResult.Equals(SelRect) Then
                    ObjectListBox.SelectedItems.Add(ZObj)
                End If
            End If
        Next
    End Sub
#End Region

    Private _IsDraggingObject As Boolean = False
    Private _StartObjectDrag As New Point
    Private _StartClickTime As Long? = Nothing
    Private _FirstClick As Boolean = True
    Private _LastItemClicked As IBaseGeneralObject = Nothing

    Private Sub ItemContainer_MouseLeftButtonDown(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        If _LastItemClicked IsNot sender.DataContext Then
            _FirstClick = True
        End If
        If _StartClickTime IsNot Nothing AndAlso Environment.TickCount - _StartClickTime > 800 Then
            _FirstClick = True
        End If
        If _FirstClick OrElse _StartClickTime Is Nothing Then
            _StartClickTime = Environment.TickCount
        End If

        _StartObjectDrag = Mouse.GetPosition(ObjectCanvas)
        If sender.CaptureMouse() Then
            e.Handled = True
            ObjectListBox.Focus()
        End If

        _LastItemClicked = sender.DataContext
    End Sub

    Private Sub ItemContainer_MouseDoubleClick(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        Dim Frm = New ObjectProperties()
        Frm.Owner = Window.GetWindow(Me)

        Dim Map As MapData = Me.DataContext

        Dim ObjClone As ZObject = ObjectListBox.SelectedItem.Clone
        Frm.DataContext = ObjClone
        If Frm.ShowDialog() = True Then
            'ObjClone.UpdatePosition(ObjClone.Args(0).Value, ObjClone.Args(1).Value)
            Map.ZObjects(ObjectListBox.SelectedIndex) = ObjClone
            ObjectListBox.SelectedItem = ObjClone
        End If
        Mouse.Capture(Nothing)
    End Sub

    Private Sub ItemContainer_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        Dim Pos = Mouse.GetPosition(ObjectCanvas)
        If Not _IsDraggingObject And e.LeftButton = MouseButtonState.Pressed Then
            If Math.Abs(Pos.X - _StartObjectDrag.X) > SystemParameters.MinimumHorizontalDragDistance OrElse
               Math.Abs(Pos.Y - _StartObjectDrag.Y) > SystemParameters.MinimumVerticalDragDistance Then

                _IsDraggingObject = True
                If Not ObjectListBox.SelectedItems.Contains(sender.Content) Then
                    ObjectListBox.SelectedItem = sender.Content
                End If

                For Each ZObj As ZObject In ObjectListBox.SelectedItems
                    ZObj.PreviousVersion = ZObj.Clone
                Next
            End If
        End If

        If _IsDraggingObject Then
            Dim CurPoint = Mouse.GetPosition(ObjectCanvas)
            Dim Diff = CurPoint - _StartObjectDrag

            For Each ZObj As ZObject In ObjectListBox.SelectedItems
                Dim StartX = SPASMHelper.Eval(ZObj.PreviousVersion.Args(0).Value)
                Dim StartY = SPASMHelper.Eval(ZObj.PreviousVersion.Args(1).Value)

                ZObj.Args(0).Value = CInt(Math.Round(StartX + Diff.X))
                ZObj.Args(1).Value = CInt(Math.Round(StartY + Diff.Y))
            Next

            e.Handled = True
        End If
    End Sub


    Private Sub ItemContainer_MouseLeftButtonUp(sender As System.Object, e As System.Windows.Input.MouseButtonEventArgs)
        If Mouse.Captured Is sender Then
            sender.ReleaseMouseCapture()
            If Not _IsDraggingObject Then
                ObjectListBox.SelectedItem = sender.Content
            End If


            If _FirstClick Then
                _FirstClick = False
            Else
                If Not _IsDraggingObject And Environment.TickCount - _StartClickTime < 500 Then
                    ItemContainer_MouseDoubleClick(sender, e)
                End If
                _StartClickTime = Nothing
                _FirstClick = True
            End If

            _IsDraggingObject = False
        End If
    End Sub

    Public Overrides Sub DeselectAll()

    End Sub

    Public Overrides ReadOnly Property LayerType As LayerType
        Get
            Return LayerType.ObjectLayer
        End Get
    End Property

    Private Sub Object_Drop(sender As Object, e As DragEventArgs)
        Dim Pos = e.GetPosition(ObjectListBox)
        Dim Def As ZDef = e.Data.GetData("WPFZ80MapEditor.ZDef")

        Dim Args As New List(Of Object)

        Dim ImageW = Map.Scenario.Images(Def.DefaultImage).Image.Width
        Dim ImageH = Map.Scenario.Images(Def.DefaultImage).Image.Height

        Args.Add(CInt(Math.Round(Pos.X - (Def.DefaultW / 2))))
        Args.Add(CInt(Math.Round(Pos.Y + (ImageH / 2) - (Def.DefaultH) + Def.DefaultZ)))
        Dim Obj As ZObject = ZObject.FromDef(Def, Args)


        Map.ZObjects.Add(Obj)
        ObjectListBox.SelectedItem = Obj
        ObjectListBox.Focus()
    End Sub
End Class