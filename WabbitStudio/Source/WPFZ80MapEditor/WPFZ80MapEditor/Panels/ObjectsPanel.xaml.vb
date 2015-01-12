Public Class ObjectsPanel

    Dim _DragStart As Point = New Point(-1, -1)
    Dim DragCount As Integer

    Private Sub Object_MouseMove(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        If Not e.LeftButton = MouseButtonState.Pressed Then
            Exit Sub
        End If

        Dim CurPos As Point = e.GetPosition(Me)

        If _DragStart.X = -1 Then
            _DragStart = CurPos
            sender.CaptureMouse()
            DragCount = 0
        Else
            Debug.WriteLine("Drag count: " & DragCount)
            DragCount = DragCount + 1
            If DragCount = SystemParameters.MinimumHorizontalDragDistance Then
                Dim Obj As ZDef = CType(sender, StackPanel).DataContext
                Debug.WriteLine("Dragging object!")

                StartDragInProcAdorner(sender, e, Obj)

                _DragStart.X = -1
                sender.ReleaseMouseCapture()
            End If
        End If
    End Sub

    Private _adorner As DragAdorner
    Private _layer As AdornerLayer
    Private IsDragging As Boolean
    Private _dragHasLeftScope As Boolean
    Private DragScope As FrameworkElement

    Private Sub StartDragInProcAdorner(sender As System.Object, e As MouseEventArgs, Def As ZDef)
        DragScope = Application.Current.MainWindow.Content

        Dim previousDrop As Boolean = DragScope.AllowDrop
        DragScope.AllowDrop = True

        Dim draghandler As New DragEventHandler(AddressOf Window1_DragOver)
        AddHandler DragScope.PreviewDragOver, draghandler


        '// Drag Leave is optional, but write up explains why I like it .. 
        'DragEventHandler dragleavehandler = new DragEventHandler(DragScope_DragLeave);
        'DragScope.DragLeave += dragleavehandler; 

        '// QueryContinue Drag goes with drag leave... 
        'QueryContinueDragEventHandler queryhandler = new QueryContinueDragEventHandler(DragScope_QueryContinueDrag);
        'DragScope.QueryContinueDrag += queryhandler; 

        Dim image As New Image()
        Dim Src As ImageSource = Me.DataContext.Scenario.Images(Def.DefaultImage).Image

        Dim Layers = Application.Current.MainWindow.FindName("LayerContainer")
        Dim st As ScaleTransform = CType(Layers.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is ScaleTransform)
        Dim currentZoom = st.ScaleX

        image.Source = Src
        image.Width = Src.Width * currentZoom
        image.Height = Src.Height * currentZoom

        image.Measure(New Size(image.Width, image.Height))

        _adorner = New DragAdorner(DragScope, image)
        _layer = AdornerLayer.GetAdornerLayer(DragScope)
        _layer.Add(_adorner)


        IsDragging = True
        _dragHasLeftScope = False

        DataContext.CurrentLayer = ObjectsTabControl.SelectedItem.Tag

        DragDrop.DoDragDrop(sender, Def, DragDropEffects.Move)

        DragScope.AllowDrop = previousDrop
        AdornerLayer.GetAdornerLayer(DragScope).Remove(_adorner)
        _adorner = Nothing

        RemoveHandler DragScope.PreviewDragOver, draghandler

        'DragSource.GiveFeedback -= feedbackhandler;
        'DragScope.DragLeave -= dragleavehandler;
        'DragScope.QueryContinueDrag -= queryhandler;
        'DragScope.PreviewDragOver -= draghandler;  

        IsDragging = False
    End Sub



    '    private bool _dragHasLeftScope = false; 
    'void DragScope_QueryContinueDrag(object sender, QueryContinueDragEventArgs e)
    '{
    '    if (this._dragHasLeftScope)
    '    {
    '        e.Action = DragAction.Cancel;
    '        e.Handled = true; 
    '    }

    '}


    'void DragScope_DragLeave(object sender, DragEventArgs e)
    '{
    '    if (e.OriginalSource == DragScope)
    '    {
    '        Point p = e.GetPosition(DragScope);
    '        Rect r = VisualTreeHelper.GetContentBounds(DragScope);
    '        if (!r.Contains(p))
    '        {
    '            this._dragHasLeftScope = true;
    '            e.Handled = true;
    '        }
    '    }

    '} 

    Sub Window1_DragOver(sender As Object, args As DragEventArgs)
        If Not _adorner Is Nothing Then
            _adorner.LeftOffset = args.GetPosition(DragScope).X
            _adorner.TopOffset = args.GetPosition(DragScope).Y
        End If
    End Sub







    Private Sub Object_MouseUp(sender As System.Object, e As System.Windows.Input.MouseEventArgs)
        Debug.WriteLine("MouseUp")
        _DragStart.X = -1
        sender.ReleaseMouseCapture()
    End Sub

    Private Sub Object_GiveFeedback(sender As System.Object, e As System.Windows.GiveFeedbackEventArgs)
        e.UseDefaultCursors = True

        e.Handled = True
    End Sub
End Class
