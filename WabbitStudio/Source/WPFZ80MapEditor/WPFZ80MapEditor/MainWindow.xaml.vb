Imports System.Windows.Media.Animation
Imports System.Linq
Imports System.IO
Imports WPFZ80MapEditor.ValueConverters
Imports System.Threading.Tasks

Public Class MainWindow
    Private ReadOnly Property Scenario As Scenario
        Get
            Return DataContext
        End Get
    End Property

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

        If ZeldaFolder IsNot Nothing Then
            Dim HillScenario As New Scenario
            Await HillScenario.LoadScenario(Path.Combine(ZeldaFolder, "hill.asm"))
            Model.Scenario = HillScenario
        End If
    End Sub

    'Private Sub Background_MouseWheel(sender As Object, e As MouseWheelEventArgs) Handles Background.MouseWheel
    '    Const DefaultZoomFactor As Double = 1.4
    '    Dim zoomFactor = DefaultZoomFactor

    '    Dim physicalPoint As Point = e.GetPosition(Background)

    '    Dim tt As TranslateTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is TranslateTransform)
    '    Dim st As ScaleTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is ScaleTransform)

    '    Dim mousePosition = LayerContainer.RenderTransform.Inverse.Transform(physicalPoint)

    '    If e.Delta <= 0 Then zoomFactor = 1.0 / DefaultZoomFactor
    '    Dim currentZoom = st.ScaleX

    '    Dim conv = New DoubleToZoomLevelConverter
    '    CurrentZoomLevelItem.Content = conv.Convert(currentZoom, GetType(String), Nothing, Nothing)
    '    ZoomLevelCombo.SelectedIndex = 0

    '    currentZoom *= zoomFactor

    '    currentZoom = Math.Min(4.0, Math.Max(0.5, currentZoom))

    '    tt.BeginAnimation(TranslateTransform.XProperty, CreateZoomAnimation(-1 * (mousePosition.X * currentZoom - physicalPoint.X)), HandoffBehavior.Compose)
    '    tt.BeginAnimation(TranslateTransform.YProperty, CreateZoomAnimation(-1 * (mousePosition.Y * currentZoom - physicalPoint.Y)), HandoffBehavior.Compose)

    '    st.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(currentZoom), HandoffBehavior.Compose)
    '    st.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(currentZoom), HandoffBehavior.Compose)

    '    If currentZoom < 2.0 Then
    '        RenderOptions.SetBitmapScalingMode(LayerContainer, BitmapScalingMode.Fant)
    '    Else
    '        RenderOptions.SetBitmapScalingMode(LayerContainer, BitmapScalingMode.NearestNeighbor)
    '    End If
    'End Sub

    'Private StartPoint As Point
    'Private Origin As Point

    'Private Sub ZoomAnimationEnd(sender As Object, e As EventArgs)
    '    Dim clock As Clock = sender

    '    If clock.CurrentState <> ClockState.Active Then
    '        Dim st As ScaleTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is ScaleTransform)
    '        Dim conv = New DoubleToZoomLevelConverter
    '        CurrentZoomLevelItem.Content = conv.Convert(st.ScaleX, GetType(String), Nothing, Nothing)
    '    End If
    'End Sub

    'Private Function CreateZoomAnimation(toValue As Double) As DoubleAnimation
    '    Dim da = New DoubleAnimation(toValue, New Duration(TimeSpan.FromMilliseconds(250)))
    '    da.AccelerationRatio = 0.1
    '    da.DecelerationRatio = 0.9
    '    AddHandler da.CurrentStateInvalidated, AddressOf ZoomAnimationEnd
    '    da.Freeze()

    '    Return da
    'End Function

    'Private Sub Background_MouseMove(sender As Object, e As MouseEventArgs) Handles Background.MouseMove
    '    If Background.IsMouseCaptured Then
    '        Dim tt As TranslateTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is TranslateTransform)

    '        Dim physicalPoint = e.GetPosition(Background)

    '        Dim toX = physicalPoint.X - StartPoint.X + Origin.X
    '        Dim toY = physicalPoint.Y - StartPoint.Y + Origin.Y

    '        If tt.HasAnimatedProperties Then
    '            tt.ApplyAnimationClock(TranslateTransform.XProperty, Nothing)
    '            tt.ApplyAnimationClock(TranslateTransform.YProperty, Nothing)
    '        End If

    '        tt.X = toX
    '        tt.Y = toY
    '    End If
    'End Sub

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

    'Private Sub MapsetNew_Click(sender As Object, e As RoutedEventArgs) Handles MapsetNew.Click
    '    Dim SelectedMap As MapData = LayerContainer.SelectedItem
    '    If SelectedMap.Exists Then
    '        MessageBox.Show("This map already exists", "Error", MessageBoxButton.OK)
    '        Exit Sub
    '    End If

    '    Scenario.AddMap(MapData.EmptyMap(Scenario, 0, SelectedMap.X, SelectedMap.Y, 16 * 16))
    'End Sub

    'Private Sub MapsetDelete_Click(sender As Object, e As RoutedEventArgs) Handles MapsetDelete.Click
    '    Dim SelectedMap As MapData = LayerContainer.SelectedItem
    '    Scenario.RemoveMap(SelectedMap)
    'End Sub

    'Private Async Sub OpenScenario_Click(sender As Object, e As RoutedEventArgs) Handles OpenScenario.Click
    '    Dim dlg As New Windows.Forms.FolderBrowserDialog
    '    If dlg.ShowDialog() = Forms.DialogResult.OK Then
    '        ZeldaFolder = dlg.SelectedPath
    '        Dim HillScenario As New Scenario
    '        Await HillScenario.LoadScenario(Path.Combine(ZeldaFolder, "hill.asm"))
    '        Model.Scenario = HillScenario
    '    End If
    'End Sub

    'Private Sub Button1_Click(sender As Object, e As RoutedEventArgs) Handles Button1.Click
    '    Scenario.SaveScenario()
    'End Sub

    'Private Sub AddColumnLeft_Click(sender As Object, e As RoutedEventArgs) Handles AddColumnLeft.Click
    '    'LayerContainer.AddLeftColumn()
    '    'For Each Child In LayerContainer.Children
    '    '    Grid.SetColumn(Child, Grid.GetColumn(Child) + 1)
    '    'Next
    '    'For i = 0 To LayerContainer.RowDefinitions.Count - 1
    '    '    Scenario.AddMap(0, i, Nothing)
    '    'Next
    'End Sub

    'Private Sub DeleteMapColumn_Click(sender As Object, e As RoutedEventArgs) Handles DeleteMapColumn.Click
    '    'Dim CurCol = Grid.GetColumn(MapSet.CurrentlySelected)

    '    'Dim LayersInColumn = (From m In LayerContainer.Children Where Grid.GetColumn(m) = CurCol).ToList()
    '    'LayersInColumn.ForEach(Sub(m) LayerContainer.Children.Remove(m))

    '    'LayerContainer.ColumnDefinitions.RemoveAt(CurCol)
    '    'For Each Child In (From m In LayerContainer.Children Where Grid.GetColumn(m) > CurCol)
    '    '    Grid.SetColumn(Child, Grid.GetColumn(Child) - 1)
    '    'Next
    'End Sub

    'Private Sub AddColumnRight_Click(sender As Object, e As RoutedEventArgs) Handles AddColumnRight.Click
    '    'LayerContainer.AddRightColumn()
    '    'For i = 0 To LayerContainer.RowDefinitions.Count - 1
    '    '    Scenario.AddMap(LayerContainer.ColumnDefinitions.Count - 1, i, Nothing)
    '    'Next
    'End Sub

    'Private Sub AddRowTop_Click(sender As Object, e As RoutedEventArgs) Handles AddRowTop.Click
    '    'LayerContainer.AddTopRow()
    '    'For Each Child In LayerContainer.Children
    '    '    Grid.SetRow(Child, Grid.GetRow(Child) + 1)
    '    'Next
    '    'For i = 0 To LayerContainer.ColumnDefinitions.Count - 1
    '    '    Scenario.AddMap(i, 0, Nothing)
    '    'Next
    'End Sub

    'Private Sub AddRowBottom_Click(sender As Object, e As RoutedEventArgs) Handles AddRowBottom.Click
    '    'LayerContainer.AddBottomRow()
    '    'For i = 0 To LayerContainer.ColumnDefinitions.Count - 1
    '    '    Scenario.AddMap(i, LayerContainer.RowDefinitions.Count - 1, Nothing)
    '    'Next
    'End Sub

    'Private Sub DeleteMapRow_Click(sender As Object, e As RoutedEventArgs) Handles DeleteMapRow.Click
    '    'Dim CurRow = Grid.GetRow(MapSet.CurrentlySelected)

    '    'Dim LayersInRow = (From m In LayerContainer.Children Where Grid.GetRow(m) = CurRow).ToList()
    '    'LayersInRow.ForEach(Sub(m) LayerContainer.Children.Remove(m))

    '    'LayerContainer.RowDefinitions.RemoveAt(CurRow)
    '    'For Each Child In (From m In LayerContainer.Children Where Grid.GetRow(m) > CurRow)
    '    '    Grid.SetRow(Child, Grid.GetRow(Child) - 1)
    '    'Next
    'End Sub

    'Private Sub TestButton_Click(sender As Object, e As RoutedEventArgs) Handles TestButton.Click
    '    Dim GameWindow As New GameWindow()
    '    GameWindow.Scenario = Scenario
    '    GameWindow.Show()
    'End Sub

    'Private Sub Background_PreviewMouseRightButtonDown(sender As Object, e As MouseButtonEventArgs) Handles Background.PreviewMouseRightButtonDown
    '    Dim tt As TranslateTransform = CType(LayerContainer.RenderTransform, TransformGroup).Children.First(Function(t) TypeOf t Is TranslateTransform)

    '    StartPoint = e.GetPosition(Background)
    '    Origin = New Point(tt.X, tt.Y)
    '    Background.CaptureMouse()
    'End Sub

    'Private Sub Background_PreviewMouseRightButtonUp(sender As Object, e As MouseButtonEventArgs) Handles Background.PreviewMouseRightButtonUp
    '    Background.ReleaseMouseCapture()
    'End Sub

    Private Sub RadioMapSet_Click(sender As Object, e As RoutedEventArgs)
        If sender.tag IsNot Nothing Then
            Model.SelectedMap = Nothing

            Dim result = VisualStateManager.GoToState(Me, sender.Tag.ToString, True)
            Model.CurrentLayer = sender.Tag

            Debug.WriteLine("Result: " & result)
        End If
    End Sub
End Class
