Imports System.Collections.ObjectModel
Imports WabbitemuLib
Imports SPASM
Imports System.Runtime.InteropServices
Imports System.Threading
Imports System.ComponentModel

Public Class GameModel
    Inherits DependencyObject
    Implements INotifyPropertyChanged
    'Implements IDisposable

    Private _Calc As Wabbitemu
    Private _Asm As IZ80Assembler

    Private _RunningLock As New Object
    Private _Running As Boolean = True

    Public Const P_PUSHING As Byte = 1 << 2
    Public Const P_SHIELD As Byte = 1 << 4

    Public Shared ReadOnly ScreenXProperty As DependencyProperty =
        DependencyProperty.Register("ScreenX", GetType(Byte), GetType(GameModel))
    Public Shared ReadOnly ScreenYProperty As DependencyProperty =
        DependencyProperty.Register("ScreenY", GetType(Byte), GetType(GameModel))

    Public Shared ReadOnly CurrentMapProperty As DependencyProperty =
        DependencyProperty.Register("CurrentMap", GetType(Byte), GetType(GameModel))

    Public Shared ReadOnly DrawEntriesProperty As DependencyProperty =
        DependencyProperty.Register("DrawEntries", GetType(ObservableCollection(Of ZDrawEntry)), GetType(GameModel))

    Public Shared ReadOnly MapProperty As DependencyProperty =
        DependencyProperty.Register("Map", GetType(MapData), GetType(GameModel))

    Public ScreenX As Byte
    Public ScreenY As Byte

    Public GameFlags As Byte

    ' Public PlayerX As Byte
    'Public PlayerY As Byte

    Public Property DrawEntries
        Get
            Return GetValue(DrawEntriesProperty)
        End Get
        Set(value)
            SetValue(DrawEntriesProperty, value)
        End Set
    End Property

    Public Property Map As MapData
        Get
            Return GetValue(MapProperty)
        End Get
        Set(value As MapData)
            SetValue(MapProperty, value)
        End Set
    End Property

    Public ImageMap As New Dictionary(Of UShort, Integer)

    Private DrawQueueAddr As UShort
    Private DrawEntrySize As Byte
    Private ScreenPosAddr As UShort
    Private DrawEntryCountAddr As UShort
    Private MapDataAddr As UShort
    Private FlagsAddr As UShort
    Private CurrentMapAddr As UShort
    Private PrepareChangeAddr As UShort

    Private FrameProcessThread As New Thread(AddressOf FrameProcess)
    Private ProcessEvent As New AutoResetEvent(False)


    Public Property Scenario As Scenario

    Private Sub LaunchApp(Name As String)
        Const ProgToEdit = &H84BF
        Const RamCode = &H8100

        Dim AppCode() As Byte = {&HEF, &HD3, &H48, &HEF, &H51, &H4C}
        Dim NameBytes = System.Text.Encoding.ASCII.GetBytes(Name)
        _Calc.Break()
        _Calc.Memory.Write(ProgToEdit, NameBytes)
        _Calc.Memory.Write(RamCode, AppCode)
        _Calc.CPU.Halt = False
        _Calc.CPU.PC = RamCode
        _Calc.Run()
    End Sub

    Public Sub New(scenario As Scenario)
        _Calc = New Wabbitemu
        _Asm = New Z80Assembler

        _Asm.CurrentDirectory = MainWindow.ZeldaFolder

        _Asm.InputFile = "zelda_all.asm"
        _Asm.OutputFile = IO.Path.Combine(MainWindow.ZeldaFolder, "zelda.8xk")

        _Asm.IncludeDirectories.Add("defaults")
        _Asm.IncludeDirectories.Add("images")
        _Asm.IncludeDirectories.Add("maps")
        _Asm.IncludeDirectories.Add("scripts")

        _Asm.Assemble()
        Debug.Write(_Asm.StdOut.ReadAll())

        _Calc.LoadFile(MainWindow.RomPath)
        _Calc.LoadFile(_Asm.OutputFile)

        _Calc.Run()
        _Calc.Reset()

        _Calc.TurnCalcOn()
        _Calc.TurnCalcOn()

        LaunchApp("Zelda   ")

        DrawQueueAddr = _Asm.Labels("DRAW_QUEUE")
        DrawEntrySize = _Asm.Labels("DRAW_ENTRY_WIDTH")
        ScreenPosAddr = _Asm.Labels("SCREEN_XC")
        DrawEntryCountAddr = _Asm.Labels("DRAW_COUNT")
        MapDataAddr = _Asm.Labels("MAP_DATA")
        FlagsAddr = _Asm.Labels("GAME_FLAGS")
        CurrentMapAddr = _Asm.Labels("CURRENT_MAP")

        Me.Scenario = scenario

        For Each Define As String In SPASMHelper.Defines
            Dim DefineKey As String = Define.ToUpper()
            Debug.WriteLine("Processing: " & Define)
            If DefineKey Like "*_GFX?" Or DefineKey Like "*_GFX" Then
                Dim Address As UShort = _Asm.Labels(Define.ToUpper()) And &HFFFF
                If Not ImageMap.ContainsKey(Address) Then
                    ImageMap.Add(Address, SPASMHelper.Defines(Define))
                End If
            End If
        Next

        Dim ZeldaApp As ITIApplication = Nothing
        For Each App As ITIApplication In _Calc.Apps
            If App.Name Like "Zelda*" Then
                ZeldaApp = App
                Exit For
            End If
        Next

        Dim Page As Byte = _Asm.Labels("SORT_DONE") / &H10000

        _Calc.Break()

        Dim SortDone As New CalcAddress
        SortDone.Initialize(_Calc.Memory.Flash(ZeldaApp.Page.Index - Page), _Asm.Labels("SORT_DONE") And &HFFFF)
        _Calc.Breakpoints.Add(SortDone)

        AddHandler _Calc.Breakpoint, AddressOf Calc_Breakpoint

        PrepareChangeAddr = _Asm.Labels("PREPARE_CHANGE")
        Dim PrepareChangePage As Byte = PrepareChangeAddr / &H10000
        Dim PrepareChange As New CalcAddress
        PrepareChange.Initialize(_Calc.Memory.Flash(ZeldaApp.Page.Index - PrepareChangePage), PrepareChangeAddr And &HFFFF)
        _Calc.Breakpoints.Add(PrepareChange)

        _Calc.Run()
        '_Calc.Visible = True

        Map = New MapData(scenario, 0)

        DrawEntries = New ObservableCollection(Of ZDrawEntry)
        FrameProcessThread.Start()
    End Sub

    Public Sub Start()
        _Calc.Run()
    End Sub

    Public Sub Pause()
        _Calc.Break()
    End Sub

    Private Sub Calc_Breakpoint(Calc As Object, Breakpoint As Object)
        SyncLock _RunningLock
            If _Running Then
                If Breakpoint.Address.Address <> PrepareChangeAddr Then
                    UpdateModel(_Asm, Calc)
                End If

                Calc.Step()
                Calc.Run()
            End If
        End SyncLock
    End Sub

    Private Sub FrameProcess()
        While ProcessEvent.WaitOne() AndAlso _Running
            Me.Dispatcher.InvokeAsync(
                Sub()
                    Dim FrameDrawEntries As New ObservableCollection(Of ZDrawEntry)
                    While DrawEntryRawData.Any()
                        Dim Entry = ZDrawEntry.FromData(DrawEntryRawData.Take(DrawEntrySize).ToArray())

                        If ImageMap.ContainsKey(Entry.Image) Then
                            Entry.Image = ImageMap(Entry.Image)
                        Else
                            Entry.Image = 0
                        End If
                        FrameDrawEntries.Add(Entry)
                        DrawEntryRawData = DrawEntryRawData.Skip(DrawEntrySize)
                    End While

                    DrawEntries = FrameDrawEntries
                    SetValue(ScreenXProperty, ScreenX)
                    SetValue(ScreenYProperty, ScreenY)

                    If _NewMap <> _CurrentMap Then
                        _CurrentMap = _NewMap
                        RaisePropertyChanged("SourceMap")
                    End If

                    For i = 0 To 255
                        If Map.TileData(i) <> MapRawData(i) Then
                            Map.TileData(i) = MapRawData(i)
                        End If
                    Next
                End Sub)
        End While
    End Sub

    Private DrawEntryRawData As IEnumerable(Of Byte)
    Private MapRawData() As Byte

    Public Sub UpdateModel(Asm As IZ80Assembler, Calc As IWabbitemu)
        Dim Memory = Calc.Memory
        Dim DrawEntryCount = Memory.ReadByte(DrawEntryCountAddr)

        DrawEntryRawData = Memory.Read(DrawQueueAddr, DrawEntryCount * DrawEntrySize)
        MapRawData = Memory.Read(MapDataAddr, 256)

        Dim ScreenPos = Memory.Read(ScreenPosAddr, 2)
        ScreenX = ScreenPos(0)
        ScreenY = ScreenPos(1)

        GameFlags = Memory.ReadByte(FlagsAddr)
        _NewMap = Memory.ReadByte(CurrentMapAddr)

        ProcessEvent.Set()
    End Sub

    Private _CurrentMap As Integer = -1
    Private _NewMap As Integer = -1
    Public ReadOnly Property SourceMap As MapData
        Get
            If _CurrentMap > -1 Then
                Return Scenario.GetMap(_CurrentMap)
            Else
                Return Nothing
            End If
        End Get
    End Property

    Public Sub StopSimulation()
        RemoveHandler _Calc.Breakpoint, AddressOf Calc_Breakpoint

        SyncLock _RunningLock
            _Running = False
            _Calc.Break()
        End SyncLock

        ' Wait a few frame's worth for final breakpoints
        Thread.Sleep(300)

        _Calc.Break()

        _Calc = Nothing
        _Asm = Nothing

        ProcessEvent.Set()

        FrameProcessThread.Join()

        ' Wait for Wabbitemu to be collected
        System.GC.Collect()
        System.GC.WaitForPendingFinalizers()
    End Sub

    '#Region "IDisposable Support"
    '    Private disposedValue As Boolean ' To detect redundant calls

    '    ' IDisposable
    '    Protected Overridable Sub Dispose(disposing As Boolean)
    '        If Not Me.disposedValue Then
    '            If disposing Then
    '                RemoveHandler _Calc.Breakpoint, AddressOf Calc_Breakpoint

    '                SyncLock _RunningLock
    '                    _Running = False
    '                    _Calc.Break()
    '                End SyncLock

    '                ' Wait a few frame's worth for final breakpoints
    '                Thread.Sleep(300)

    '                _Calc.Break()

    '                _Calc = Nothing
    '                _Asm = Nothing

    '                ProcessEvent.Set()

    '                FrameProcessThread.Join()
    '            End If

    '            ' TODO: free unmanaged resources (unmanaged objects) and override Finalize() below.
    '            ' TODO: set large fields to null.
    '        End If
    '        Me.disposedValue = True
    '    End Sub

    '    ' TODO: override Finalize() only if Dispose(ByVal disposing As Boolean) above has code to free unmanaged resources.
    '    'Protected Overrides Sub Finalize()
    '    '    ' Do not change this code.  Put cleanup code in Dispose(ByVal disposing As Boolean) above.
    '    '    Dispose(False)
    '    '    MyBase.Finalize()
    '    'End Sub

    '    ' This code added by Visual Basic to correctly implement the disposable pattern.
    '    Public Sub Dispose() Implements IDisposable.Dispose
    '        ' Do not change this code.  Put cleanup code in Dispose(disposing As Boolean) above.
    '        Dispose(True)
    '        GC.SuppressFinalize(Me)
    '    End Sub
    '#End Region

    Public Event PropertyChanged(sender As Object, e As PropertyChangedEventArgs) Implements INotifyPropertyChanged.PropertyChanged
    Private Sub RaisePropertyChanged(PropName As String)
        RaiseEvent PropertyChanged(Me, New PropertyChangedEventArgs(PropName))
    End Sub
End Class
