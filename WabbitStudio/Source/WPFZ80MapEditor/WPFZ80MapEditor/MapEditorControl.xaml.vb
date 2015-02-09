Imports System.Windows.Media.Animation
Imports System.Linq

Imports WPFZ80MapEditor.ValueConverters

<TemplatePart(Name:="PART_LayerContainer", Type:=GetType(XLayerContainer))>
Public Class MapEditorControl
    Public Shared Property StartTime As Long
    Public Shared Property ZeldaFolder As String
    Public Shared Property RomPath As String

    Private _Model As New AppModel
    Public ReadOnly Property Model As AppModel
        Get
            Return _Model
        End Get
    End Property

    Private _ZoomLevel As Double
    Public Property ZoomLevel As Double
        Get
            Return _ZoomLevel
        End Get
        Set(value As Double)
            If Math.Abs(value - _ZoomLevel) > Double.Epsilon Then
                _ZoomLevel = value

                If _ZoomLevel < 2.0 Then
                    RenderOptions.SetBitmapScalingMode(LayerContainer, BitmapScalingMode.Fant)
                    RenderOptions.SetBitmapScalingMode(TestView, BitmapScalingMode.Fant)
                Else
                    RenderOptions.SetBitmapScalingMode(LayerContainer, BitmapScalingMode.NearestNeighbor)
                    RenderOptions.SetBitmapScalingMode(TestView, BitmapScalingMode.NearestNeighbor)
                End If
            End If
        End Set
    End Property

    Public Shared ReadOnly Property MouseDoubleClickEventProperty() As RoutedEvent
        Get
            Return MouseDoubleClickEvent
        End Get
    End Property

    Public Sub New()
        InitializeComponent()
        DataContext = Model
        _ZoomLevel = 1.0
    End Sub

    Public Async Sub OpenScenario(fileName As String)
        Dim scenario As New Scenario
        Await scenario.LoadScenario(fileName)

        Debug.Print("Done loading: " & (Now - DateTime.FromFileTime(StartTime)).TotalMilliseconds & " ms")
        AddHandler LayerContainer.LayoutUpdated, AddressOf LayoutChanged
        Model.Scenario = scenario
    End Sub

    Private Sub LayoutChanged(sender As Object, e As Object)
        Debug.Print("Done with layout: " & (Now - DateTime.FromFileTime(StartTime)).TotalMilliseconds & " ms")
        RemoveHandler LayerContainer.LayoutUpdated, AddressOf LayoutChanged
    End Sub

    Private Sub Background_MouseWheel(sender As Object, e As MouseWheelEventArgs)
        Const DefaultZoomFactor As Double = 1.4
        Dim zoomFactor = DefaultZoomFactor

        Dim physicalPoint As Point = e.GetPosition(sender)

        Dim RenderTransform As TransformGroup = Resources("MapCanvasRenderTransform")
        Dim tt As TranslateTransform = RenderTransform.Children.First(Function(t) TypeOf t Is TranslateTransform)
        Dim st As ScaleTransform = RenderTransform.Children.First(Function(t) TypeOf t Is ScaleTransform)

        Dim mousePosition = RenderTransform.Inverse.Transform(physicalPoint)

        'TODO: FIX
        If e.Delta <= 0 Then zoomFactor = 1.0 / DefaultZoomFactor
        Dim currentZoom = st.ScaleX

        Dim conv = New DoubleToZoomLevelConverter
        currentZoom *= zoomFactor

        currentZoom = Math.Min(8.0, Math.Max(0.5, currentZoom))
        ZoomLevel = currentZoom

        tt.BeginAnimation(TranslateTransform.XProperty, CreateZoomAnimation(-1 * (mousePosition.X * currentZoom - physicalPoint.X)), HandoffBehavior.Compose)
        tt.BeginAnimation(TranslateTransform.YProperty, CreateZoomAnimation(-1 * (mousePosition.Y * currentZoom - physicalPoint.Y)), HandoffBehavior.Compose)

        st.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(currentZoom), HandoffBehavior.Compose)
        st.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(currentZoom), HandoffBehavior.Compose)
    End Sub

    Private StartPoint As Point
    Private Origin As Point

    Private Sub ZoomAnimationEnd(sender As Object, e As EventArgs)
        Dim clock As Clock = sender

        If clock.CurrentState <> ClockState.Active Then
            Dim RenderTransform As TransformGroup = Resources("MapCanvasRenderTransform")
            Dim st As ScaleTransform = RenderTransform.Children.First(Function(t) TypeOf t Is ScaleTransform)
            Dim conv = New DoubleToZoomLevelConverter
            'CurrentZoomLevelItem.Content = conv.Convert(st.ScaleX, GetType(String), Nothing, Nothing)
        End If
    End Sub

    Private Function CreateZoomAnimation(toValue As Double) As DoubleAnimation
        Dim da = New DoubleAnimation(toValue, New Duration(TimeSpan.FromMilliseconds(250)))
        da.AccelerationRatio = 0.1
        da.DecelerationRatio = 0.9
        AddHandler da.CurrentStateInvalidated, AddressOf ZoomAnimationEnd
        da.Freeze()

        Return da
    End Function

    Private Sub Background_MouseMove(sender As Object, e As MouseEventArgs)
        Dim MapCanvas As Canvas = sender
        If MapCanvas.IsMouseCaptured Then

            Dim RenderTransform As TransformGroup = Resources("MapCanvasRenderTransform")

            Dim tt As TranslateTransform = RenderTransform.Children.First(Function(t) TypeOf t Is TranslateTransform)

            Dim physicalPoint = e.GetPosition(sender)

            Dim toX = physicalPoint.X - StartPoint.X + Origin.X
            Dim toY = physicalPoint.Y - StartPoint.Y + Origin.Y

            If tt.HasAnimatedProperties Then
                tt.ApplyAnimationClock(TranslateTransform.XProperty, Nothing)
                tt.ApplyAnimationClock(TranslateTransform.YProperty, Nothing)
            End If

            tt.X = toX
            tt.Y = toY
        End If
    End Sub

    Private Sub MapsetNew_Click(sender As Object, e As RoutedEventArgs)
        If Model.SelectedMap Is Nothing Then Exit Sub

        If Model.SelectedMap.Exists Then
            Exit Sub
        End If

        Dim EmptyMap = MapData.EmptyMap(Model.Scenario, 0, Model.SelectedMap.X, Model.SelectedMap.Y, 16 * 16)
        Model.Scenario.AddMap(EmptyMap)

        Model.SelectedMap = EmptyMap
    End Sub

    Private Sub Background_PreviewMouseRightButtonDown(sender As Object, e As MouseButtonEventArgs)
        Dim RenderTransform As TransformGroup = Resources("MapCanvasRenderTransform")
        Dim tt As TranslateTransform = RenderTransform.Children.First(Function(t) TypeOf t Is TranslateTransform)

        StartPoint = e.GetPosition(sender)
        Origin = New Point(tt.X, tt.Y)
        sender.CaptureMouse()
    End Sub

    Private Sub Background_PreviewMouseRightButtonUp(sender As Object, e As MouseButtonEventArgs)
        sender.ReleaseMouseCapture()
    End Sub

    Private Sub Map_MouseDoubleClick(sender As Object, e As MouseButtonEventArgs)
        MapsetNew_Click(sender, e)
    End Sub

    Public Sub StartTesting()
        TestView.Visibility = Visibility.Visible
        LayerContainer.Focusable = False
    End Sub

    Public Sub StopTesting()
        TestView.Visibility = Visibility.Hidden
    End Sub

End Class
