Imports System.Windows.Media.Animation
Imports System.Linq
Imports System.IO
Imports Microsoft.Win32
Imports WPFZ80MapEditor.ValueConverters

Public Class MainWindow
    Private ReadOnly Property Model As AppModel
        Get
            Return MapControl.Model
        End Get
    End Property

    Public Sub New()
        InitializeComponent()

        DataContext = Model
    End Sub

    Private Sub Window_Loaded(sender As Object, e As RoutedEventArgs) Handles MyBase.Loaded
        DataContext = Model
        Model.CurrentLayer = LayerType.MapsetLayer

        MapEditorControl.RomPath = Registry.GetValue("HKEY_CURRENT_USER\Software\Wabbitemu", "rom_path", "")
        ObjectsPanel.DragScope = MapControl

        If MapEditorControl.ZeldaFolder IsNot Nothing Then
            Dim fileName = Path.Combine(MapEditorControl.ZeldaFolder, "maps\hill.asm")
            MapControl.OpenScenario(fileName)
        End If

        MapControl.LayerContainer.AddHandler(MapLayer.CoordinatesUpdatedEvent, New RoutedEventHandler(Sub(ctrl As Object, args As CoordinatesUpdatedArgs)
                                                                                                          Model.Status = args.Point.X & ", " & args.Point.Y
                                                                                                      End Sub))

    End Sub

    Private Sub ZoomAnimationEnd(sender As Object, e As EventArgs)
        Dim clock As Clock = sender

        If clock.CurrentState <> ClockState.Active Then
            Dim RenderTransform As TransformGroup = MapControl.Resources("MapCanvasRenderTransform")
            Dim st As ScaleTransform = RenderTransform.Children.First(Function(t) TypeOf t Is ScaleTransform)
            Dim conv = New DoubleToZoomLevelConverter
            CurrentZoomLevelItem.Content = conv.Convert(st.ScaleX, GetType(String), Nothing, Nothing)
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

    Private Sub ZoomLevelCombo_SelectionChanged(sender As Object, e As SelectionChangedEventArgs) Handles ZoomLevelCombo.SelectionChanged
        Dim combo As ComboBox = sender

        Dim newzoom As Double
        Dim sel As String = combo.SelectedItem.Content
        Try
            Dim NumStr As String = sel.Replace("%", "")
            newzoom = Double.Parse(NumStr) / 100.0
        Catch ex As Exception
            Debug.WriteLine(ex)
        End Try

        Try
            Dim RenderTransform As TransformGroup = MapControl.Resources("MapCanvasRenderTransform")
            If Not MapControl.LayerContainer Is Nothing AndAlso Not RenderTransform Is Nothing Then
                Dim tt As TranslateTransform = RenderTransform.Children.First(Function(t) TypeOf t Is TranslateTransform)
                Dim st As ScaleTransform = RenderTransform.Children.First(Function(t) TypeOf t Is ScaleTransform)

                Dim PhysPos As New Point(MapControl.MapCanvas.ActualWidth / 2, MapControl.MapCanvas.ActualHeight / 2)
                Dim MousePos = RenderTransform.Inverse.Transform(PhysPos)
                tt.BeginAnimation(TranslateTransform.XProperty, CreateZoomAnimation(-1 * (MousePos.X * newzoom - PhysPos.X)), HandoffBehavior.Compose)
                tt.BeginAnimation(TranslateTransform.YProperty, CreateZoomAnimation(-1 * (MousePos.Y * newzoom - PhysPos.Y)), HandoffBehavior.Compose)

                st.BeginAnimation(ScaleTransform.ScaleXProperty, CreateZoomAnimation(newzoom), HandoffBehavior.Compose)
                st.BeginAnimation(ScaleTransform.ScaleYProperty, CreateZoomAnimation(newzoom), HandoffBehavior.Compose)
            End If
        Catch ex As Exception

        End Try
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

    Private Sub MapsetDelete_Click(sender As Object, e As RoutedEventArgs)
        If Model.SelectedMap Is Nothing Then Exit Sub

        Model.Scenario.RemoveMap(Model.SelectedMap)
    End Sub

    Private Sub SaveCanExecute(sender As Object, e As CanExecuteRoutedEventArgs)
        e.CanExecute = True
        e.Handled = True
    End Sub

    Private Sub OpenCanExecute(sender As Object, e As CanExecuteRoutedEventArgs)
        e.CanExecute = True
        e.Handled = True
    End Sub

    Private Async Sub OpenExecuted(sender As Object, e As ExecutedRoutedEventArgs)
        Dim dlg As New Windows.Forms.OpenFileDialog
        If dlg.ShowDialog() = Forms.DialogResult.OK Then
            MapEditorControl.ZeldaFolder = Path.GetDirectoryName(Path.GetDirectoryName(dlg.FileName))

            Model.DeselectAll()

            Dim Scenario As New Scenario
            Await Scenario.LoadScenario(dlg.FileName)
            Model.Scenario = Scenario
        End If
        e.Handled = True
    End Sub

    Private Sub SaveExecuted(sender As Object, e As ExecutedRoutedEventArgs)
        Model.Scenario.SaveScenario()
        e.Handled = True
    End Sub

    Private Sub UndoCanExecute(sender As Object, e As CanExecuteRoutedEventArgs)
        e.CanExecute = UndoManager.CanUndo()
        e.Handled = True
    End Sub

    Private Sub UndoExecuted(sender As Object, e As ExecutedRoutedEventArgs)
        UndoManager.Undo(Model)
        e.Handled = True
    End Sub

    Private Sub RedoCanExecute(sender As Object, e As CanExecuteRoutedEventArgs)
        e.CanExecute = UndoManager.CanRedo()
        e.Handled = True
    End Sub

    Private Sub RedoExecuted(sender As Object, e As ExecutedRoutedEventArgs)
        UndoManager.Redo(Model)
        e.Handled = True
    End Sub

    Private _OldLayer As LayerType
    Public Sub StartTesting()
        Model.Scenario.SaveScenario()

        _OldLayer = Model.CurrentLayer
        Model.CurrentLayer = LayerType.TestingLayer

        Model.GameModel = New GameModel(Model.Scenario)
        Model.GameModel.Start()

        MapControl.TestView.Focus()
    End Sub

    Public Sub StopTesting()
        If Model.GameModel IsNot Nothing Then
            Model.GameModel.Pause()
            'Model.GameModel.Dispose()
            Model.GameModel.StopSimulation()

            Model.GameModel = Nothing
            Model.CurrentLayer = _OldLayer
        End If
    End Sub

    Private Sub MainWindow_Closed(sender As Object, e As EventArgs) Handles MyBase.Closed, MyBase.Closed
        StopTesting()
    End Sub
End Class
