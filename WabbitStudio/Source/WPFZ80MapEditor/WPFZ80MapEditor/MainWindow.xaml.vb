Imports System.Windows.Media.Animation
Imports System.Linq
Imports System.IO
Imports WPFZ80MapEditor.ValueConverters
Imports System.Threading.Tasks

<TemplatePart(Name:="PART_LayerContainer", Type:=GetType(XLayerContainer))>
Public Class MainWindow
    Public Shared Property ZeldaFolder As String
    Public Shared Property RomPath As String

    Public Shared Instance As MainWindow

    Private _Model As New AppModel
    Public ReadOnly Property Model As AppModel
        Get
            Return _Model
        End Get
    End Property

    Public Sub New()
        Instance = Me
        InitializeComponent()
    End Sub

    Private Async Sub Window_Loaded(sender As Object, e As RoutedEventArgs) Handles MyBase.Loaded
        DataContext = Model
        Model.CurrentLayer = LayerType.MapsetLayer

        If ZeldaFolder IsNot Nothing Then
            Dim HillScenario As New Scenario
            Await HillScenario.LoadScenario(Path.Combine(ZeldaFolder, "maps\hill.asm"))
            Model.Scenario = HillScenario
        End If
    End Sub

    Private Sub Background_MouseWheel(sender As Object, e As MouseWheelEventArgs)
        Const DefaultZoomFactor As Double = 1.4
        Dim zoomFactor = DefaultZoomFactor

        Dim physicalPoint As Point = e.GetPosition(sender)

        Dim RenderTransform As TransformGroup = Resources("MapCanvasRenderTransform")
        Dim tt As TranslateTransform = RenderTransform.Children.First(Function(t) TypeOf t Is TranslateTransform)
        Dim st As ScaleTransform = RenderTransform.Children.First(Function(t) TypeOf t Is ScaleTransform)

        Dim mousePosition = RenderTransform.Inverse.Transform(physicalPoint)

        If e.Delta <= 0 Then zoomFactor = 1.0 / DefaultZoomFactor
        Dim currentZoom = st.ScaleX

        Dim conv = New DoubleToZoomLevelConverter
        currentZoom *= zoomFactor

        currentZoom = Math.Min(4.0, Math.Max(0.5, currentZoom))

        tt.BeginAnimation(TranslateTransform.XProperty, CreateZoomAnimation(-1 * (mousePosition.X * currentZoom - physicalPoint.X)), HandoffBehavior.Compose)
        tt.BeginAnimation(TranslateTransform.YProperty, CreateZoomAnimation(-1 * (mousePosition.Y * currentZoom - physicalPoint.Y)), HandoffBehavior.Compose)

        st.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(currentZoom), HandoffBehavior.Compose)
        st.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(currentZoom), HandoffBehavior.Compose)

        If currentZoom < 2.0 Then
            RenderOptions.SetBitmapScalingMode(LayerContainer, BitmapScalingMode.Fant)
        Else
            RenderOptions.SetBitmapScalingMode(LayerContainer, BitmapScalingMode.NearestNeighbor)
        End If
    End Sub

    Private StartPoint As Point
    Private Origin As Point

    Private Sub ZoomAnimationEnd(sender As Object, e As EventArgs)
        Dim clock As Clock = sender

        If clock.CurrentState <> ClockState.Active Then
            'Dim st As ScaleTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is ScaleTransform)
            'Dim conv = New DoubleToZoomLevelConverter
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

    'Private Sub ZoomLevelCombo_SelectionChanged(sender As Object, e As SelectionChangedEventArgs) Handles ZoomLevelCombo.SelectionChanged
    '    Dim combo As ComboBox = sender

    '    Dim newzoom As Double
    '    Dim sel As String = combo.SelectedItem.Content
    '    Try
    '        Dim NumStr As String = sel.Replace("%", "")
    '        newzoom = Double.Parse(NumStr) / 100.0
    '    Catch ex As Exception
    '        Debug.WriteLine(ex)
    '    End Try

    '    Try
    '        If Not LayerContainer Is Nothing AndAlso Not LayerContainer.RenderTransform Is Nothing Then
    '            Dim tt As TranslateTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is TranslateTransform)
    '            Dim st As ScaleTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is ScaleTransform)

    '            Dim PhysPos As New Point(Background.ActualWidth / 2, Background.ActualHeight / 2)
    '            Dim MousePos = LayerContainer.RenderTransform.Inverse.Transform(PhysPos)
    '            tt.BeginAnimation(TranslateTransform.XProperty, CreateZoomAnimation(-1 * (MousePos.X * newzoom - PhysPos.X)), HandoffBehavior.Compose)
    '            tt.BeginAnimation(TranslateTransform.YProperty, CreateZoomAnimation(-1 * (MousePos.Y * newzoom - PhysPos.Y)), HandoffBehavior.Compose)

    '            st.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(newzoom), HandoffBehavior.Compose)
    '            st.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(newzoom), HandoffBehavior.Compose)
    '        End If
    '    Catch ex As Exception

    '    End Try
    'End Sub

    'Private Sub ZoomLevelCombo_DropDownClosed(sender As Object, e As EventArgs) Handles ZoomLevelCombo.DropDownClosed
    '    CurrentZoomLevelItem.ClearValue(HeightProperty)
    'End Sub

    'Private Sub ZoomLevelCombo_DropDownOpened(sender As Object, e As EventArgs) Handles ZoomLevelCombo.DropDownOpened
    '    CurrentZoomLevelItem.Height = 0
    'End Sub

    'Private Sub LayerRadioButton_Checked(sender As Object, e As RoutedEventArgs) _
    '    Handles RadioMapSet.Checked, RadioMapView.Checked, RadioObjectLayer.Checked, RadioMiscLayer.Checked, RadioEnemyLayer.Checked
    '    If sender.tag IsNot Nothing Then
    '        Dim result = VisualStateManager.GoToState(Me, sender.Tag, True)
    '        Debug.WriteLine("Result: " & result)
    '    End If
    'End Sub

    Private Sub MapsetNew_Click(sender As Object, e As RoutedEventArgs)
        If Model.SelectedMap Is Nothing Then Exit Sub

        If Model.SelectedMap.Exists Then
            MessageBox.Show("This map already exists", "Error", MessageBoxButton.OK)
            Exit Sub
        End If

        Dim EmptyMap = MapData.EmptyMap(Model.Scenario, 0, Model.SelectedMap.X, Model.SelectedMap.Y, 16 * 16)
        Model.Scenario.AddMap(EmptyMap)

        Model.SelectedMap = EmptyMap
    End Sub

    Private Sub MapsetDelete_Click(sender As Object, e As RoutedEventArgs)
        If Model.SelectedMap Is Nothing Then Exit Sub

        Model.Scenario.RemoveMap(Model.SelectedMap)
    End Sub

    Private Async Sub OpenScenario_Click(sender As Object, e As RoutedEventArgs)
        Dim dlg As New Windows.Forms.OpenFileDialog
        If dlg.ShowDialog() = Forms.DialogResult.OK Then
            ZeldaFolder = Path.GetDirectoryName(Path.GetDirectoryName(dlg.FileName))

            Dim Scenario As New Scenario
            Await Scenario.LoadScenario(dlg.FileName)
            Model.Scenario = Scenario
        End If
    End Sub

    Private Sub NewScenario_Click(sender As Object, e As RoutedEventArgs)
        Dim dlg As New Windows.Forms.OpenFileDialog
        If dlg.ShowDialog() = Forms.DialogResult.OK Then
            ZeldaFolder = Path.GetDirectoryName(dlg.FileName)

            Dim Scenario As New Scenario
            Model.Scenario = Scenario
        End If
    End Sub

    'Private Sub TestButton_Click(sender As Object, e As RoutedEventArgs) Handles TestButton.Click
    '    Dim GameWindow As New GameWindow()
    '    GameWindow.Scenario = Scenario
    '    GameWindow.Show()
    'End Sub

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

    Private Sub RadioMapSet_Click(sender As Object, e As RoutedEventArgs)
        If sender.tag IsNot Nothing Then
            Model.CurrentLayer = sender.Tag
        End If
    End Sub

    Private Sub Map_MouseDoubleClick(sender As Object, e As MouseButtonEventArgs)
        MapsetNew_Click(sender, e)
    End Sub

    Private Sub Save_Click(sender As Object, e As RoutedEventArgs)
        Model.Scenario.SaveScenario()
    End Sub
End Class
